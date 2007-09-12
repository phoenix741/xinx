/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
// Qt header
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>

// Xinx header
#include "xslproject.h"
#include "webservices.h"
#include "globals.h"
#include "xinxconfig.h"

#define XINX_PROJECT_VERSION_1 1
#define XINX_PROJECT_VERSION_2 2
#define XINX_PROJECT_VERSION 2

/* XSLProjectException */

XSLProjectException::XSLProjectException( const QString & message ) {
	m_message = message;
}

const QString & XSLProjectException::getMessage() const {
	return m_message;
}

/* PrivateXSLProject */

class PrivateXSLProject {
public:
	PrivateXSLProject( XSLProject * parent );
	
	QString getValue( const QString & node ) const;
	void setValue( const QString & node, const QString & value );
	
	void loadSessionFile( const QString & fileName );
	void saveSessionFile( const QString & fileName );

	void loadWebServicesLink();
	void saveWebServicesLink();

	void loadSearchPath();
	void saveSearchPath();

	QString processPath( QString path );

	QString m_fileName;
	int m_version;
	
	QDomDocument m_projectDocument;
	QDomDocument m_sessionDocument;
	QDomElement m_sessionNode, m_rootSession;
	
	QStringList m_searchPathList, m_webServiceLink, m_lastOpenedFile;
private:
	XSLProject * m_parent;
};

PrivateXSLProject::PrivateXSLProject( XSLProject * parent ) {
	m_parent = parent;

	QDomElement root = m_projectDocument.createElement( "XSLProject" );
	m_projectDocument.appendChild( root );
	
	m_rootSession = m_sessionDocument.createElement( "Session" );
	m_sessionDocument.appendChild( m_rootSession );
	
	m_sessionNode = m_sessionDocument.createElement( "Opened" );
	m_rootSession.appendChild( m_sessionNode );
	
	m_searchPathList.append( "./langues/<lang>/nav/<project>" );
	m_searchPathList.append( "./langues/<lang>/nav" );
	m_searchPathList.append( "./" );
	m_searchPathList.append( "./langues/<lang>" );
	m_searchPathList.append( "./langues" );

	setValue( "specifiqueProjectPathName", global.m_config->config().project.defaultProjectPathName );
}

QString PrivateXSLProject::getValue( const QString & node ) const {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( node );

	if( ! elt.isNull() ){
		QDomNode node = elt.firstChild();
		while( ! ( node.isNull() || node.isText() ) ) node = node.nextSibling();
		QDomText text = node.toText();
		return text.nodeValue();
	} else
		return QString();
}

void PrivateXSLProject::setValue( const QString & node, const QString & value ) {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( node );
	QDomText    text;

	if( elt.isNull() ){
		elt = m_projectDocument.createElement( node );
		root.appendChild( elt );
	} else {
		QDomNode node = elt.firstChild();
		while( ! ( node.isNull() || node.isText() ) ) node = node.nextSibling();
		text = node.toText();
	}
	
	if( text.isNull() ) {
		text = m_projectDocument.createTextNode( value );
		elt.appendChild( text );
	} else
		text.setData( value );
}

void PrivateXSLProject::loadSessionFile( const QString & fileName ) {
	QFile file( fileName );
	
	try {
		m_sessionDocument = QDomDocument( "Session" );

		// Open the file
		if ( ! file.open(QFile::ReadOnly | QFile::Text) ) 
			throw XSLProjectException( XSLProject::tr("Can't open session file." ) );

		// Load XML Document
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if (!m_sessionDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) 
			throw XSLProjectException( XSLProject::tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));

		m_rootSession = m_sessionDocument.documentElement();
  
		// Test if Project File
		if( m_rootSession.tagName() != "Session" ) {
			throw XSLProjectException( XSLProject::tr("The file isn't a XINX Project Session file") );
		}
	
		m_sessionNode = m_rootSession.firstChildElement( "Opened" );
	
		m_lastOpenedFile.clear();
		QDomElement lastOpenedFile = m_rootSession.firstChildElement( "lastOpenedFile" );
		if( ! lastOpenedFile.isNull() ) {
			QDomElement file = lastOpenedFile.firstChildElement( "file" );
			while( ! file.isNull() ) {
				m_lastOpenedFile.append( file.attribute( "name" ) );
				
				file = file.nextSiblingElement( "file" );
			}
		}
	} catch( XSLProjectException ) {
		m_sessionDocument = QDomDocument( "Session" );

		m_rootSession = m_sessionDocument.createElement( "Session" );
		m_sessionDocument.appendChild( m_rootSession );
	
		m_sessionNode = m_sessionDocument.createElement( "Opened" );
		m_rootSession.appendChild( m_sessionNode );
	}
}

void PrivateXSLProject::saveSessionFile( const QString & fileName ) {
	QDomElement lastOpenedFile = m_rootSession.firstChildElement( "lastOpenedFile" );
	if( ! lastOpenedFile.isNull() ) 
		m_rootSession.removeChild( lastOpenedFile );

	lastOpenedFile = m_sessionDocument.createElement( "lastOpenedFile" );
	m_rootSession.appendChild( lastOpenedFile );	

	foreach( QString name, m_lastOpenedFile ) {
		QDomElement file = m_sessionDocument.createElement( "file" );
		file.setAttribute( "name", name );
		lastOpenedFile.appendChild( file );
	}
	
	static const int IndentSize = 3;
	QFile file( fileName );
	
	// Open the file
	if ( ! file.open(QFile::WriteOnly | QFile::Text) ) 
		throw XSLProjectException( XSLProject::tr("Cannot write file %1:\n%2.").arg( m_fileName ).arg( file.errorString()) );
		
	// Save the content
	QTextStream text( &file );
	m_sessionDocument.save( text, IndentSize );
}

void PrivateXSLProject::loadWebServicesLink() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "webServiceLink" );
	
	m_webServiceLink.clear();
	
	if( elt.isNull() ) return;
		
	QDomElement file = elt.firstChildElement( "link" );
	
	while ( ! file.isNull() ) {
		m_webServiceLink.append( file.firstChild().toText().nodeValue() );	
		
		file = file.nextSiblingElement( "link" );
	}
}

void PrivateXSLProject::saveWebServicesLink() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "webServiceLink" );
	
	if( ! elt.isNull() )
		root.removeChild( elt );

	elt = m_projectDocument.createElement( "webServiceLink" );
	root.appendChild( elt );
	
	foreach(QString filename, m_webServiceLink) {
		QDomElement file = m_projectDocument.createElement( "link" );
		elt.appendChild( file );
		
		QDomText text = m_projectDocument.createTextNode( filename );
		file.appendChild( text );
	}
}

void PrivateXSLProject::loadSearchPath() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "searchPath" );
	
	m_searchPathList.clear();
	
	if( elt.isNull() ) return;
		
	QDomElement file = elt.firstChildElement( "path" );
	
	while ( ! file.isNull() ) {
		m_searchPathList.append( file.firstChild().toText().nodeValue() );	
		
		file = file.nextSiblingElement( "path" );
	}
}

void PrivateXSLProject::saveSearchPath() {
	QDomElement root = m_projectDocument.documentElement();
	QDomElement elt  = root.firstChildElement( "searchPath" );
	
	if( ! elt.isNull() )
		root.removeChild( elt );

	elt = m_projectDocument.createElement( "searchPath" );
	root.appendChild( elt );
	
	foreach( QString filename, m_searchPathList ) {
		QDomElement file = m_projectDocument.createElement( "path" );
		elt.appendChild( file );
		
		QDomText text = m_projectDocument.createTextNode( filename );
		file.appendChild( text );
	}
}

QString PrivateXSLProject::processPath( QString path ) {
	path.replace( '<lang>', m_parent->defaultLang().toLower() );
	path.replace( '<LANG>', m_parent->defaultLang().toUpper() );
	path.replace( '<nav>', m_parent->defaultNav().toLower() );
	path.replace( '<NAV>', m_parent->defaultNav().toUpper() );
	path.replace( '<project>', m_parent->specifiqueProjectPathName().toLower() );
	path.replace( '<PROJECT>', m_parent->specifiqueProjectPathName().toUpper() );
	return path;
}

/* XSLProject */

XSLProject::XSLProject() : QObject() {
	d = new PrivateXSLProject( this );
}

XSLProject::XSLProject( const XSLProject & object ) : QObject() {
	d = new PrivateXSLProject( this );

	d->m_fileName 		  = object.d->m_fileName;
	d->m_projectDocument = object.d->m_projectDocument;
	d->m_sessionDocument = object.d->m_sessionDocument;
	d->m_sessionNode     = object.d->m_sessionNode;
}

XSLProject::XSLProject( const QString & project ) : QObject() {
	d = new PrivateXSLProject( this );
	
	loadFromFile( project );
}

XSLProject::~XSLProject() {
	delete d;
}
	
void XSLProject::loadFromFile( const QString & filename ) {
	QFile file(filename);
	
	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
		throw XSLProjectException( tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()) );
	
	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if (!d->m_projectDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
		throw XSLProjectException( tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr) );
	
	QDomElement root = d->m_projectDocument.documentElement();
  
	// Test if Project File
	if( root.tagName() != "XSLProject" ) 
		throw XSLProjectException( tr("The file isn't a XINX Project") );
	
	d->m_version  = d->getValue( "xinx_version" ).isEmpty() ? 0 : d->getValue( "xinx_version" ).toInt();
	if( d->m_version > XINX_PROJECT_VERSION ) 
		throw XSLProjectException( tr("The file is a too recent XINX Project") );
	
	d->m_fileName = filename;
	d->loadWebServicesLink();
	d->loadSearchPath();

	if( d->m_version < XINX_PROJECT_VERSION_1 ) {
		clearSessionNode();
		d->m_rootSession = d->m_sessionDocument.createElement( "Session" );
		d->m_sessionDocument.appendChild( d->m_rootSession );
	
		d->m_sessionNode = d->m_sessionDocument.createElement( "Opened" );
		d->m_rootSession.appendChild( d->m_sessionNode );
		QDomElement elt = root.firstChildElement( "openedElementCount" );
		if( ! elt.isNull() ) {
			QDomElement file = elt.firstChildElement( "file" );
			while ( ! file.isNull() ) {
				QDomElement node = d->m_sessionDocument.createElement( "editor" );
				node.setAttribute( "filename", file.firstChild().toText().nodeValue() );
				node.setAttribute( "position", 0 );
				d->m_sessionNode.appendChild( node );
		
				file = file.nextSiblingElement( "file" );
			}
		}
	} if( d->m_version < XINX_PROJECT_VERSION_2 ) {
		if( d->getValue( "type" ) == "services" )
			d->setValue( "options", QString("%1").arg( XSLProject::hasSpecifique | XSLProject::hasWebServices ) );
		else
			d->setValue( "options", QString("%1").arg( XSLProject::hasSpecifique ) );
			
		QString path = QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( d->getValue( "specifique" ) );
		d->setValue( "specifique", path );		
		d->setValue( "specifiqueProjectPathName", global.m_config->config().project.defaultProjectPathName );

		d->m_searchPathList.append( path );
		d->m_searchPathList.append( "./langues/<lang>/nav" );
		d->m_searchPathList.append( "./" );
		d->m_searchPathList.append( "./langues/<lang>" );
		d->m_searchPathList.append( "./langues" );
	} else 
		d->loadSessionFile( d->m_fileName + ".session" );
	
	if( ! QDir( projectPath() ).exists() )
		throw XSLProjectException( tr( "Project path (%1) don't exists." ).arg( projectPath() ) );
}

void XSLProject::saveToFile( const QString & filename ) {
	if( ! filename.isEmpty() ) {
		QString ppath = projectPath(),
				spath = specifPath();		
		d->m_fileName = filename;
		setProjectPath( ppath );
		setSpecifPath( spath );
	}
	if( d->m_fileName.isEmpty() ) return;
	
	saveOnlySession();
	
	if( d->m_version != XINX_PROJECT_VERSION_2 ) {
		d->setValue( "xinx_version", QString( "%1" ).arg( XINX_PROJECT_VERSION ) );
	}
	if( d->m_version < XINX_PROJECT_VERSION_1 ) {
		QDomElement root = d->m_projectDocument.documentElement();
		QDomElement oldSession = root.firstChildElement( "openedElementCount" );
		root.removeChild( oldSession );
	} 
	if( d->m_version < XINX_PROJECT_VERSION_2 ) {
		QDomElement root = d->m_projectDocument.documentElement();
		QDomElement oldType = root.firstChildElement( "type" );
		root.removeChild( oldType );
		QDomElement oldVersion = root.firstChildElement( "version" );
		root.removeChild( oldVersion );
	}
	
	setProjectPath( projectPath() );
	setSpecifPrefix( specifPrefix() );


	d->saveWebServicesLink();
	d->saveSearchPath();
	
	static const int IndentSize = 3;

	QFile file( d->m_fileName );
	
	// Open the file
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		throw XSLProjectException( QObject::tr("Cannot write file %1:\n%2.").arg( d->m_fileName ).arg( file.errorString()) );
		return;
	}
	QTextStream text( &file );
	
	d->m_projectDocument.save( text, IndentSize );
}

void XSLProject::saveOnlySession() {
	if( d->m_fileName.isEmpty() ) return;
	d->saveSessionFile( d->m_fileName + ".session" );
}
	
XSLProject::ProjectOptions XSLProject::options() const {
	QString value = d->getValue( "options" );
	return ProjectOptions( value.toInt() );
}

void XSLProject::setOptions( XSLProject::ProjectOptions options ) {
	d->setValue( "options", QString( "%1" ).arg( options ) );
}
	
XSLProject::enumProjectRCS XSLProject::projectRCS() const {
	QString value = d->getValue( "rcs" );
	if( value == "cvs" ) 
		return CVS; 	
	else
	if( value == "subversion" ) 
		return SUBVERSION; 	
	else
		return NORCS;
}

void XSLProject::setProjectRCS( const XSLProject::enumProjectRCS & value ) {
	switch( value ) {
	case CVS:
		d->setValue( "rcs", "cvs" );	
		break;
	case SUBVERSION:
		d->setValue( "rcs", "subversion" );	
		break;
	default:
		d->setValue( "rcs", "no" );	
	}
}

QString XSLProject::projectName() const {
	return d->getValue( "name" );	
}

void XSLProject::setProjectName( const QString & value ) {
	d->setValue( "name", value );	
}
	
QString XSLProject::defaultLang() const {
	return d->getValue( "lang" );
}

void XSLProject::setDefaultLang( const QString & value ) {
	d->setValue( "lang", value );
}
	
QString XSLProject::defaultNav() const {
	return d->getValue( "nav" );
}

void XSLProject::setDefaultNav( const QString & value ) {
	d->setValue( "nav", value );
}
	
QString XSLProject::projectPath() const {
	QString path = d->getValue( "project" );

	if( QDir( path ).isAbsolute() )
		return path;
	else
		return QFileInfo( d->m_fileName ).absoluteDir().absoluteFilePath( path );
}

void XSLProject::setProjectPath( const QString & value ) {
	d->setValue( "project", QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( value ) );
}
	
QString XSLProject::processedSpecifPath() const {
	return d->processPath( specifPath() );
}

QString XSLProject::specifPath() const {
	return d->getValue( "specifique" );
}

void XSLProject::setSpecifPath( const QString & value ) {
	d->setValue( "specifique", value );
}

void XSLProject::setSpecifiqueProjectPathName( const QString & value ) {
	d->setValue( "specifiqueProjectPathName", value );
}

QString XSLProject::specifiqueProjectPathName() {
	return d->getValue( "specifiqueProjectPathName" );
} 

QString XSLProject::specifPrefix() const {
	return d->getValue( "prefix" );
}
	
void XSLProject::setSpecifPrefix( const QString & value ) {
	d->setValue( "prefix", value );
}

void XSLProject::clearSessionNode() {
	d->m_rootSession.removeChild( d->m_sessionNode );
	d->m_sessionNode = d->m_sessionDocument.createElement( "Opened" );
	d->m_rootSession.appendChild( d->m_sessionNode );
}

const QString & XSLProject::fileName() const {
	return d->m_fileName;
}

QStringList & XSLProject::serveurWeb() { 
	return d->m_webServiceLink; 
}

QStringList & XSLProject::searchPathList() {
	return d->m_searchPathList;
}

QStringList XSLProject::processedSearchPathList() {
	QStringList list;
	foreach( QString path, searchPathList() ) {
		list << d->processPath( path );
	}
	return list;
}

QDomDocument & XSLProject::sessionDocument() { 
	return d->m_sessionDocument;
}

QDomElement & XSLProject::sessionNode() { 
	return d->m_sessionNode; 
}

QStringList & XSLProject::lastOpenedFile() { 
	return d->m_lastOpenedFile; 
}
