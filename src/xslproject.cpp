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
	
	static QString getValue( const QDomDocument & document, const QString & node ) const;
	static void setValue( const QDomDocument & document, const QString & node, const QString & value );
	
	static QStringList loadList( QDomDocument document, const QString & list, const QString & element );
	static void saveList( const QDomDocument & document, const QString & list, const QString & element, const QStringList & slist );

	void loadSessionFile( const QString & fileName );
	void saveSessionFile( const QString & fileName );
	
	QString processPath( QString path );

	PrivateXSLProject& operator=( const PrivateXSLProject& p );

	QString m_fileName;
	int m_version;
	
	QStringList m_searchPathList, 
				m_webServiceLink, 
				m_lastOpenedFile;
	int m_indexOfSpecifiquePath;
	QList<XSLProject::structSession> m_sessions;
	QString m_projectName, m_defaultLang, m_defaultNav;
	QString m_projectPath, m_specifiquePathName, m_specifiquePrefix;
	XSLProject::ProjectOptions m_projectOptions;
	XSLProject::enumProjectRCS m_projectRCS;
	
private:
	XSLProject * m_parent;
};

PrivateXSLProject::PrivateXSLProject( XSLProject * parent ) {
	m_parent = parent;

	m_searchPathList.append( "./langues/<lang>/nav/<project>" );
	m_searchPathList.append( "./langues/<lang>/nav" );
	m_searchPathList.append( "./" );
	m_searchPathList.append( "./langues/<lang>" );
	m_searchPathList.append( "./langues" );

	m_specifiquePathName = global.m_config->config().project.defaultProjectPathName;
	m_projectOptions += hasSpecifique;
}

PrivateXSLProject& PrivateXSLProject::operator=( const PrivateXSLProject& p ) {
	m_searchPathList        = p.m_searchPathList; 
	m_webServiceLink 		= p.m_webServiceLink; 
	m_lastOpenedFile 		= p.m_lastOpenedFile;
	m_indexOfSpecifiquePath = p.m_indexOfSpecifiquePath;
	m_sessions				= p.m_sessions;
	m_projectName			= p.m_projectName;
	m_defaultLang			= p.m_defaultLang;
	m_defaultNav			= p.m_defaultNav;
	m_projectPath			= p.m_projectPath;
	m_specifiquePathName	= p.m_specifiquePathName;
	m_specifiquePrefix		= p.m_specifiquePrefix;
	m_projectOptions		= p.projectOptions;
	m_projectRCS			= p.m_projectRCS;
}

QString PrivateXSLProject::getValue( const QDomDocument & document, const QString & node ) const {
	QDomElement root = document.documentElement();
	QDomElement elt  = root.firstChildElement( node );

	if( ! elt.isNull() ){
		QDomNode node = elt.firstChild();
		while( ! ( node.isNull() || node.isText() ) ) node = node.nextSibling();
		QDomText text = node.toText();
		return text.nodeValue();
	} else
		return QString();
}

void PrivateXSLProject::setValue( const QDomDocument & document, const QString & node, const QString & value ) {
	QDomElement root = document.documentElement();
	QDomElement elt  = root.firstChildElement( node );
	QDomText    text;

	if( elt.isNull() ){
		elt = document.createElement( node );
		root.appendChild( elt );
	} else {
		QDomNode node = elt.firstChild();
		while( ! ( node.isNull() || node.isText() ) ) node = node.nextSibling();
		text = node.toText();
	}
	
	if( text.isNull() ) {
		text = document.createTextNode( value );
		elt.appendChild( text );
	} else
		text.setData( value );
}

void PrivateXSLProject::loadSessionFile( const QString & fileName ) {
	QFile file( fileName );
	
	try {
		m_lastOpenedFile.clear();
		m_sessions.clear();

		QDomDocument sessionDocument;

		// Open the file
		if ( ! file.open(QFile::ReadOnly | QFile::Text) ) 
			throw XSLProjectException( XSLProject::tr("Can't open session file." ) );

		// Load XML Document
		QString errorStr;
		int errorLine;
		int errorColumn;  
		if ( !sessionDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn) ) 
			throw XSLProjectException( XSLProject::tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));

		QDomElement rootSession = sessionDocument.documentElement();
  
		// Test if Project File
		if( rootSession.tagName() != "Session" ) 
			throw XSLProjectException( XSLProject::tr("The file isn't a XINX Project Session file") );
	
		QDomElement sessionNode = rootSession.firstChildElement( "Opened" );
		if( ! sessionNode.isNull() ) {
			QDomElement editor = sessionNode.firstChildElement( "Editor" );
			while( ! editor.isNull() ) {
				QString sessionText;
				QDomNode node = elt.firstChild();
				while( ! ( node.isNull() || node.isText() ) ) node = node.nextSibling();
				QDomText text = node.toText();
				if( ! text.isNull() ) sessionText += text.data();
				
				structSession session;
				session.filename = editor.attribute( "filename" );
				session.storedSession = QByteArray::fromHex( sessionText );
				editor = editor.nextSiblingElement( "Editor" );
			}
		}
	
		QDomElement lastOpenedFile = rootSession.firstChildElement( "LastOpenedFile" );
		if( ! lastOpenedFile.isNull() ) {
			QDomElement file = lastOpenedFile.firstChildElement( "File" );
			while( ! file.isNull() ) {
				m_lastOpenedFile.append( file.attribute( "name" ) );
				
				file = file.nextSiblingElement( "File" );
			}
		}
	} catch( XSLProjectException ) {
	}
}

void PrivateXSLProject::saveSessionFile( const QString & fileName ) {
	QDomDocument document( "Session" );
	
	QDomElement root = document.createElement( "Session" );
	document.appendChild( root );
	
	QDomeElement sessions = document.createElement( "Opened" );
	rootSession.appendChild( sessions );

	QDomElement lastOpenedFile = document.createElement( "LastOpenedFile" );
	rootSession.appendChild( lastOpenedFile );	

	foreach( QString name, m_lastOpenedFile ) {
		QDomElement file = document.createElement( "File" );
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
	document.save( text, IndentSize );
}

QStringList PrivateXSLProject::loadList( QDomDocument document, const QString & list, const QString & element ) {
	QDomElement root = document.documentElement();
	QDomElement dlist = root.firstChildElement( list );
	if( dlist.isNull() ) return QStringList();

	QStringList list;
	QDomElement delement = dlist.firstChildElement( element );
	while( ! delement.isNull() ) {
		list.append( delement.firstChild().toText().nodeValue() );
		delement = delement.nextSiblingElement( element );
	}
}

void PrivateXSLProject::saveList( const QDomDocument & document, const QString & list, const QString & element, const QStringList & slist ) {
	QDomElement root = document.documentElement();
	QDomElement dlist = document.createElement( list );
	root.appendChild( dlist );
	
	foreach( QString s, slist ) {
		QDomElement delement = document.createElement( element );
		dlist.appendChild( delement );
		
		QDomText text = document.createElement( s );
		delement.appendChild( text ); 
	}
}

QString PrivateXSLProject::processPath( QString path ) {
	path.replace( "<lang>", m_parent->defaultLang().toLower() );
	path.replace( "<LANG>", m_parent->defaultLang().toUpper() );
	path.replace( "<nav>", m_parent->defaultNav().toLower() );
	path.replace( "<NAV>", m_parent->defaultNav().toUpper() );
	path.replace( "<project>", m_parent->specifiqueProjectPathName().toLower() );
	path.replace( "<PROJECT>", m_parent->specifiqueProjectPathName().toUpper() );
	return path;
}

/* XSLProject */

XSLProject::XSLProject() : QObject() {
	d = new PrivateXSLProject( this );
}

XSLProject::XSLProject( const XSLProject & object ) : QObject() {
	d = new PrivateXSLProject( this );
	*d = *(object.d);
}

XSLProject::XSLProject( const QString & project ) : QObject() {
	d = new PrivateXSLProject( this );
	loadFromFile( project );
}

XSLProject::~XSLProject() {
	delete d;
}

XSLProject& XSLProject::operator=(const XSLProject& p) {
	*d = *(object.d);
}

void XSLProject::loadFromFile( const QString & filename ) {
	QFile file(filename);
	QDomDocument document;
	
	// Open the file
	if (! file.open(QFile::ReadOnly | QFile::Text) ) 
		throw XSLProjectException( tr("Cannot read file %1:\n%2.").arg( filename ).arg( file.errorString() ) );
	
	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if ( ! document.setContent( &file, true, &errorStr, &errorLine, &errorColumn ) )
		throw XSLProjectException( tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr) );
	
	QDomElement root = document.documentElement();
  
	// Test if Project File
	if( root.tagName() != "XSLProject" ) 
		throw XSLProjectException( tr("The file isn't a XINX Project") );
	
	d->m_version = PrivateXSLProject::getValue( document, "xinx_version" ).isEmpty() ? 0 : d->getValue( "xinx_version" ).toInt();
	if( d->m_version > XINX_PROJECT_VERSION ) 
		throw XSLProjectException( tr("The file is a too recent XINX Project") );
	
	d->m_fileName = filename;

	// Load values
	if( ! QDir( projectPath() ).exists() )
		throw XSLProjectException( tr( "Project path (%1) don't exists." ).arg( projectPath() ) );

	
	if( d->m_version < XINX_PROJECT_VERSION_1 ) {
		throw XSLProjectException( tr("Project file too older. Please use a version 0.6.9.3 to convert this project file or create new project file.") );
	} if( d->m_version < XINX_PROJECT_VERSION_2 ) {
		if( PrivateXSLProject::getValue( document, "type" ) == "services" )
			d->m_options = XSLProject::hasSpecifique | XSLProject::hasWebServices;
		else
			d->m_options = XSLProject::hasSpecifique;
			
		QString path = QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( PrivateXSLProject::getValue( document, "specifique" ) );

		d->m_searchPathList.append( path );
		d->m_searchPathList.append( "./langues/<lang>/nav" );
		d->m_searchPathList.append( "./" );
		d->m_searchPathList.append( "./langues/<lang>" );
		d->m_searchPathList.append( "./langues" );
	} else { 
		d->m_searchPathList = PrivateXSLProject::loadList( document, "paths", "path" );
	}

	d->m_webServiceLink = PrivateXSLProject::loadList( document, "webServiceLink", "link" );

	d->loadSessionFile( d->m_fileName + ".session" );
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
