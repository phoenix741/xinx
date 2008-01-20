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

#define XINX_PROJECT_VERSION_0 0
#define XINX_PROJECT_VERSION_1 1
#define XINX_PROJECT_VERSION_2 2
#define XINX_PROJECT_VERSION 2

/* XSLProjectException */

XSLProjectException::XSLProjectException( const QString & message ) : XinxException( message ) {
}

/* XSLProjectSessionEditor */

XSLProjectSessionEditor::XSLProjectSessionEditor( XSLProjectSession * parent ) : QObject( parent ), m_parent( parent ) {
	parent->m_sessions.append( this );
}

XSLProjectSessionEditor::~XSLProjectSessionEditor() {
	m_parent->m_sessions.removeAll( this );
}

void XSLProjectSessionEditor::loadFromNode( const QDomElement & element ) {
	m_properties.clear();
	QDomElement node = element.firstChildElement();
	while( ! node.isNull() ) {
		QString name  = node.tagName(),
				value = node.text();
		
		if( ! value.isEmpty() )
			m_properties[ name ] = value;
		
		node = node.nextSiblingElement();
	}
}

void XSLProjectSessionEditor::saveToNode( QDomDocument & document, QDomElement & element ) {
	foreach( QString key, m_properties.keys() ) {
		QDomElement propertyElement = element.firstChildElement( key );
		if( ! propertyElement.isNull() ) element.removeChild( propertyElement );

		QDomText text = document.createTextNode( m_properties[ key ].toString() );
		
		propertyElement = document.createElement( key );
		propertyElement.appendChild( text );
		propertyElement.setAttribute( "type", m_properties[ key ].typeName() );
		element.appendChild( propertyElement );
	}
}

void XSLProjectSessionEditor::writeProperty( const QString & key, QVariant value ) {
	m_properties[ key ] = value;
}

QVariant XSLProjectSessionEditor::readProperty( const QString & key ) {
	return m_properties.value( key );
}

/* XSLProjectSession */

XSLProjectSession::XSLProjectSession() {

}

XSLProjectSession::XSLProjectSession( const QString & filename ) {
	loadFromFile( filename );
}

void XSLProjectSession::loadFromFile( const QString & filename ) {
	m_filename = filename;
	QFile file( filename );
	
	m_lastOpenedFile.clear();
	qDeleteAll( m_sessions );
	
	QDomDocument document;

	// Open the file
	if ( ! file.open(QFile::ReadOnly | QFile::Text) ) 
		throw XSLProjectException( tr("Can't open session file." ) );

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if ( ! document.setContent(&file, true, &errorStr, &errorLine, &errorColumn) ) 
		throw XSLProjectException( tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr) );

	QDomElement rootSession = document.documentElement();
  
	// Test if Project File
	if( rootSession.tagName() != "Session" ) 
		throw XSLProjectException( tr("The file isn't a XINX Project Session file") );

	QDomElement sessionNode = rootSession.firstChildElement( "Opened" );
	if( ! sessionNode.isNull() ) {
		QDomElement editor = sessionNode.firstChildElement( "Editor" );
		while( ! editor.isNull() ) {
			(new XSLProjectSessionEditor( this ))->loadFromNode( editor );
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
}

void XSLProjectSession::saveToFile( const QString & filename ) {
	QDomDocument document( "Session" );
	
	QDomElement root = document.createElement( "Session" );
	document.appendChild( root );
	
	QDomElement sessions = document.createElement( "Opened" );
	root.appendChild( sessions );
	
	foreach( XSLProjectSessionEditor* session, m_sessions ) {
		QDomElement editor = document.createElement( "Editor" );
		sessions.appendChild( editor );
		
		session->saveToNode( document, editor );
	}

	QDomElement lastOpenedFile = document.createElement( "LastOpenedFile" );
	root.appendChild( lastOpenedFile );	

	foreach( QString name, m_lastOpenedFile ) {
		QDomElement file = document.createElement( "File" );
		file.setAttribute( "name", name );
		lastOpenedFile.appendChild( file );
	}
	
	QDomElement signets = document.createElement( "bookmarks" );
	root.appendChild( signets );
	
	static const int IndentSize = 3;
	if( ! filename.isEmpty() ) m_filename = filename;
	QFile file( m_filename );
	
	// Open the file
	if ( ! file.open(QFile::WriteOnly | QFile::Text) ) 
		throw XSLProjectException( tr("Cannot write file %1:\n%2.").arg( m_filename ).arg( file.errorString()) );
		
	// Save the content
	QTextStream text( &file );
	document.save( text, IndentSize );
}

QStringList & XSLProjectSession::lastOpenedFile() {
	return m_lastOpenedFile;
}

const QList<XSLProjectSessionEditor*> & XSLProjectSession::serializedEditors() const {
	return m_sessions;
}

/* PrivateXSLProject */

class PrivateXSLProject {
public:
	PrivateXSLProject( XSLProject * parent );
	~PrivateXSLProject();
	
	static QString getValue( const QDomDocument & document, const QString & node );
	static void setValue( QDomDocument & document, const QString & node, const QString & value );
	
	static QStringList loadList( QDomDocument document, const QString & list, const QString & element );
	static void saveList( QDomDocument & document, const QString & list, const QString & element, const QStringList & slist );

	QString processPath( QString path );

	PrivateXSLProject& operator=( const PrivateXSLProject& p );

	XSLProjectSession * m_session;
	
	QString m_fileName;
	int m_version;
	
	QStringList m_searchPathList, 
				m_webServiceLink; 
	int m_indexOfSpecifiquePath;
	QString m_projectName, m_defaultLang, m_defaultNav;
	QString m_projectPath, m_specifiquePathName, m_specifiquePrefix;
	QString m_logProjectDirectory;
	XSLProject::ProjectOptions m_projectOptions;
	XSLProject::enumProjectRCS m_projectRCS;
private:
	XSLProject * m_parent;
};

PrivateXSLProject::PrivateXSLProject( XSLProject * parent ) {
	m_parent = parent;

	m_defaultLang = "FRA";
	m_defaultNav  = "NAV/MIC";

	m_searchPathList.append( "langue/<lang>/nav/<project>" );
	m_searchPathList.append( "langue/<lang>/nav" );
	m_searchPathList.append( "." );
	m_searchPathList.append( "langue/<lang>" );
	m_searchPathList.append( "langue" );
	m_indexOfSpecifiquePath = 0;

	m_specifiquePathName = global.m_config->config().project.defaultProjectPathName;
	m_projectOptions |= XSLProject::hasSpecifique;

	m_session = new XSLProjectSession();
}

PrivateXSLProject::~PrivateXSLProject() {
	delete m_session;
}

PrivateXSLProject& PrivateXSLProject::operator=( const PrivateXSLProject& p ) {
	m_searchPathList        = p.m_searchPathList; 
	m_webServiceLink 		= p.m_webServiceLink; 
	m_indexOfSpecifiquePath = p.m_indexOfSpecifiquePath;
	m_projectName			= p.m_projectName;
	m_defaultLang			= p.m_defaultLang;
	m_defaultNav			= p.m_defaultNav;
	m_projectPath			= p.m_projectPath;
	m_specifiquePathName	= p.m_specifiquePathName;
	m_specifiquePrefix		= p.m_specifiquePrefix;
	m_projectOptions		= p.m_projectOptions;
	m_projectRCS			= p.m_projectRCS;
	m_logProjectDirectory   = p.m_logProjectDirectory;
	
	return *this;
}

QString PrivateXSLProject::getValue( const QDomDocument & document, const QString & node ) {
	QDomElement root = document.documentElement();
	QDomElement elt  = root.firstChildElement( node );
	return elt.text();
}

void PrivateXSLProject::setValue( QDomDocument & document, const QString & node, const QString & value ) {
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

QStringList PrivateXSLProject::loadList( QDomDocument document, const QString & list, const QString & element ) {
	QDomElement root = document.documentElement();
	QDomElement dlist = root.firstChildElement( list );
	if( dlist.isNull() ) return QStringList();

	QStringList result;
	QDomElement delement = dlist.firstChildElement( element );
	while( ! delement.isNull() ) {
		result.append( delement.firstChild().toText().nodeValue() );
		delement = delement.nextSiblingElement( element );
	}
	
	return result;
}

void PrivateXSLProject::saveList( QDomDocument & document, const QString & list, const QString & element, const QStringList & slist ) {
	QDomElement root = document.documentElement();
	QDomElement dlist = document.createElement( list );
	root.appendChild( dlist );
	
	foreach( QString s, slist ) {
		QDomElement delement = document.createElement( element );
		dlist.appendChild( delement );
		
		QDomText text = document.createTextNode( s );
		delement.appendChild( text ); 
	}
}

QString PrivateXSLProject::processPath( QString path ) {
	path.replace( "<lang>", m_defaultLang.toLower() );
	path.replace( "<LANG>", m_defaultLang.toUpper() );
	path.replace( "<nav>", m_defaultNav.toLower() );
	path.replace( "<NAV>", m_defaultNav.toUpper() );
	path.replace( "<project>", m_specifiquePathName.toLower() );
	path.replace( "<PROJECT>", m_specifiquePathName.toUpper() );
	return QFileInfo( m_fileName ).absoluteDir().absoluteFilePath( path );
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
	*d = *(p.d);
	return *this;
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
			
	d->m_version = PrivateXSLProject::getValue( document, "xinx_version" ).isEmpty() ? 0 : PrivateXSLProject::getValue( document, "xinx_version" ).toInt();
	if( d->m_version > XINX_PROJECT_VERSION ) 
		throw XSLProjectException( tr("The file is a too recent XINX Project") );
	
	d->m_fileName = filename;

	// Load values
	d->m_projectName         = PrivateXSLProject::getValue( document, "name" );
	d->m_defaultLang         = PrivateXSLProject::getValue( document, "lang" );
	d->m_defaultNav          = PrivateXSLProject::getValue( document, "nav" );
	d->m_projectPath         = QFileInfo( d->m_fileName ).absoluteDir().absoluteFilePath( PrivateXSLProject::getValue( document, "project" ) );
	d->m_specifiquePrefix    = PrivateXSLProject::getValue( document, "prefix" );
	QString projectRCSString = PrivateXSLProject::getValue( document, "rcs" );
	if( projectRCSString == "cvs" ) {
		d->m_projectRCS      = XSLProject::CVS;
	} else if( projectRCSString == "subversion" ) {
		d->m_projectRCS      = XSLProject::SUBVERSION;
	} else { // In case it's no or a previous version
		d->m_projectRCS      = XSLProject::NORCS;
	} 
	d->m_webServiceLink = PrivateXSLProject::loadList( document, "webServiceLink", "link" );
	QString path;
	switch( d->m_version ) {
	case XINX_PROJECT_VERSION_0:
		// Warning : Informations about session is destroyed.
	case XINX_PROJECT_VERSION_1:
		if( PrivateXSLProject::getValue( document, "type" ) == "services" )
			d->m_projectOptions = XSLProject::hasSpecifique | XSLProject::hasWebServices;
		else
			d->m_projectOptions = XSLProject::hasSpecifique;
			
		path = QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( PrivateXSLProject::getValue( document, "specifique" ) );
		d->m_searchPathList.append( path );
		d->m_indexOfSpecifiquePath = d->m_searchPathList.size() - 1;
		break;
	case XINX_PROJECT_VERSION_2:
		d->m_searchPathList = PrivateXSLProject::loadList( document, "paths", "path" );
		d->m_indexOfSpecifiquePath = PrivateXSLProject::getValue( document, "indexOfSpecifiquePath" ).toInt();
		d->m_specifiquePathName = PrivateXSLProject::getValue( document, "specifiquePathName" );
		d->m_projectOptions = ProjectOptions( PrivateXSLProject::getValue( document, "options" ).toInt() );
		d->m_logProjectDirectory = QFileInfo( d->m_fileName ).absoluteDir().absoluteFilePath( PrivateXSLProject::getValue( document, "logProjectDirectory" ) );
		break;
	}

	if( ! QDir( d->m_projectPath ).exists() )
		throw XSLProjectException( tr( "Project path (%1) don't exists." ).arg( projectPath() ) );

	d->m_session->loadFromFile( d->m_fileName + ".session" );
}

void XSLProject::saveToFile( const QString & filename ) {
	if( ! filename.isEmpty() ) d->m_fileName = filename;
	if( d->m_fileName.isEmpty() ) return;
	
	// Save the session file
	d->m_session->saveToFile();
	
	// Save attributes
	QDomDocument document( "XSLProject" );
	QDomElement root = document.createElement( "XSLProject" );
	document.appendChild( root );	
	
	PrivateXSLProject::setValue( document, "xinx_version", QString( "%1" ).arg( XINX_PROJECT_VERSION ) );
	PrivateXSLProject::setValue( document, "name", d->m_projectName );
	PrivateXSLProject::setValue( document, "lang", d->m_defaultLang );
	PrivateXSLProject::setValue( document, "nav", d->m_defaultNav );
	PrivateXSLProject::setValue( document, "project", QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( d->m_projectPath ) );
	PrivateXSLProject::setValue( document, "prefix", d->m_specifiquePrefix );
	switch( d->m_projectRCS ) {
	case XSLProject::NORCS:
		PrivateXSLProject::setValue( document, "rcs", "no" );
		break;
	case XSLProject::CVS:
		PrivateXSLProject::setValue( document, "rcs", "cvs" );
		break;
	case XSLProject::SUBVERSION:
		PrivateXSLProject::setValue( document, "rcs", "subversion" );
		break;
	}
	PrivateXSLProject::saveList( document, "webServiceLink", "link", d->m_webServiceLink );
	PrivateXSLProject::saveList( document, "paths", "path", d->m_searchPathList );
	PrivateXSLProject::setValue( document, "indexOfSpecifiquePath", QString("%1").arg( d->m_indexOfSpecifiquePath ) );
	PrivateXSLProject::setValue( document, "specifiquePathName", d->m_specifiquePathName );
	PrivateXSLProject::setValue( document, "options", QString("%1").arg( d->m_projectOptions ) );
	PrivateXSLProject::setValue( document, "logProjectDirectory", QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( d->m_logProjectDirectory ) );

	// Open the file
	static const int IndentSize = 3;
	QFile file( d->m_fileName );
	if ( !file.open(QFile::WriteOnly | QFile::Text) ) 
		throw XSLProjectException( QObject::tr("Cannot write file %1:\n%2.").arg( d->m_fileName ).arg( file.errorString()) );
		
	QTextStream text( &file );
	document.save( text, IndentSize );
}

void XSLProject::saveOnlySession() {
	if( d->m_fileName.isEmpty() ) return;
	d->m_session->saveToFile();
}
	
XSLProject::ProjectOptions & XSLProject::options() {
	return d->m_projectOptions;
}

void XSLProject::setOptions( XSLProject::ProjectOptions options ) {
	d->m_projectOptions = options;
}
	
XSLProject::enumProjectRCS XSLProject::projectRCS() const {
	return d->m_projectRCS;
}

void XSLProject::setProjectRCS( const XSLProject::enumProjectRCS & value ) {
	d->m_projectRCS = value;
}

QString XSLProject::projectName() const {
	return d->m_projectName;
}

void XSLProject::setProjectName( const QString & value ) {
	d->m_projectName = value;	
}
	
QString XSLProject::defaultLang() const {
	return d->m_defaultLang;
}

void XSLProject::setDefaultLang( const QString & value ) {
	d->m_defaultLang = value;
}
	
QString XSLProject::defaultNav() const {
	return d->m_defaultNav;
}

void XSLProject::setDefaultNav( const QString & value ) {
	d->m_defaultNav = value;
}
	
QString XSLProject::projectPath() const {
	return d->m_projectPath;
}

void XSLProject::setProjectPath( const QString & value ) {
	d->m_projectPath = value;
}

void XSLProject::setSpecifiquePathName( const QString & value ) {
	d->m_specifiquePathName = value;
}

QString XSLProject::specifiquePathName() {
	return d->m_specifiquePathName;
} 

QString XSLProject::specifiquePrefix() const {
	return d->m_specifiquePrefix;
}
	
void XSLProject::setSpecifiquePrefix( const QString & value ) {
	d->m_specifiquePrefix = value;
}

QStringList & XSLProject::searchPathList() {
	return d->m_searchPathList;
}

QString XSLProject::processedSpecifiquePath() const {
	return d->processPath( d->m_searchPathList.at( d->m_indexOfSpecifiquePath ) );
}

QStringList XSLProject::processedSearchPathList() {
	QStringList list;
	foreach( QString path, searchPathList() ) {
		list << d->processPath( path );
	}
	return list;
}

int XSLProject::indexOfSpecifiquePath() const {
	return d->m_indexOfSpecifiquePath;
}

void XSLProject::setIndexOfSpecifiquePath( int value ) {
	d->m_indexOfSpecifiquePath = value;
}

const QString & XSLProject::fileName() const {
	return d->m_fileName;
}

QStringList & XSLProject::serveurWeb() { 
	return d->m_webServiceLink; 
}

void XSLProject::setLogProjectDirectory( const QString & value ) {
	d->m_logProjectDirectory = value;
}

const QString & XSLProject::logProjectDirectory() {
	return d->m_logProjectDirectory;
}

XSLProjectSession * XSLProject::session() const {
	return d->m_session;
}

