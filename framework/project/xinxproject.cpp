/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Qt header
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QApplication>

// Xinx header
#include "project/xinxproject.h"
#include "core/xinxcore.h"
#include "core/xinxconfig.h"
#include "contentview/contentviewcache.h"

#define XINX_PROJECT_VERSION_0 0
#define XINX_PROJECT_VERSION_1 1
#define XINX_PROJECT_VERSION_2 2
#define XINX_PROJECT_VERSION_3 3
#define XINX_PROJECT_VERSION_4 4
#define XINX_PROJECT_VERSION 4

/* Static member */

XINXProjectManager * XINXProjectManager::s_self = 0;

/* XinxProjectException */

XinxProjectException::XinxProjectException( const QString & message, bool wizard ) : XinxException( message ), m_wizard( wizard ) {
}

bool XinxProjectException::startWizard() const {
	return m_wizard;
}

/* XinxProjectSessionEditor */

XinxProjectSessionEditor::XinxProjectSessionEditor( XinxProjectSession * parent ) : QObject( parent ), m_parent( parent ) {
	parent->m_sessions.append( this );
}

XinxProjectSessionEditor::~XinxProjectSessionEditor() {
	m_parent->m_sessions.removeAll( this );
}

void XinxProjectSessionEditor::loadFromNode( const QDomElement & element ) {
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

void XinxProjectSessionEditor::saveToNode( QDomDocument & document, QDomElement & element ) {
	foreach( const QString & key, m_properties.keys() ) {
		QDomElement propertyElement = element.firstChildElement( key );
		if( ! propertyElement.isNull() ) element.removeChild( propertyElement );

		QDomText text = document.createTextNode( m_properties[ key ].toString() );

		propertyElement = document.createElement( key );
		propertyElement.appendChild( text );
		propertyElement.setAttribute( "type", m_properties[ key ].typeName() );
		element.appendChild( propertyElement );
	}
}

void XinxProjectSessionEditor::writeProperty( const QString & key, QVariant value ) {
	m_properties[ key ] = value;
}

QVariant XinxProjectSessionEditor::readProperty( const QString & key ) {
	return m_properties.value( key );
}

/* XinxProjectSession */

XinxProjectSession::XinxProjectSession() {

}

XinxProjectSession::XinxProjectSession( const QString & filename ) {
	loadFromFile( filename );
}

XinxProjectSession::~XinxProjectSession() {
	qDeleteAll( m_sessions );
}

void XinxProjectSession::loadFromFile( const QString & filename ) {
	m_filename = filename;
	QFile file( filename );

	m_lastOpenedFile.clear();
	qDeleteAll( m_sessions );

	QDomDocument document;

	// Open the file
	if ( ! file.open(QFile::ReadOnly | QFile::Text) )
		throw XinxProjectException( tr("Can't open session file." ) );

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;
	if ( ! document.setContent(&file, true, &errorStr, &errorLine, &errorColumn) )
		throw XinxProjectException( tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr) );

	QDomElement rootSession = document.documentElement();

	// Test if Project File
	if( rootSession.tagName() != "Session" )
		throw XinxProjectException( tr("The file isn't a XINX Project Session file") );

	QDomElement sessionNode = rootSession.firstChildElement( "Opened" );
	if( ! sessionNode.isNull() ) {
		QDomElement editor = sessionNode.firstChildElement( "Editor" );
		while( ! editor.isNull() ) {
			(new XinxProjectSessionEditor( this ))->loadFromNode( editor );
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

void XinxProjectSession::saveToFile( const QString & filename ) {
	QDomDocument document( "Session" );

	QDomElement root = document.createElement( "Session" );
	document.appendChild( root );

	QDomElement sessions = document.createElement( "Opened" );
	root.appendChild( sessions );

	foreach( XinxProjectSessionEditor* session, m_sessions ) {
		QDomElement editor = document.createElement( "Editor" );
		sessions.appendChild( editor );

		session->saveToNode( document, editor );
	}

	QDomElement lastOpenedFile = document.createElement( "LastOpenedFile" );
	root.appendChild( lastOpenedFile );

	foreach( const QString & name, m_lastOpenedFile ) {
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
		throw XinxProjectException( tr("Cannot write file %1:\n%2.").arg( m_filename ).arg( file.errorString()) );

	// Save the content
	QTextStream text( &file );
	document.save( text, IndentSize );
}

QStringList & XinxProjectSession::lastOpenedFile() {
	return m_lastOpenedFile;
}

const QList<XinxProjectSessionEditor*> & XinxProjectSession::serializedEditors() const {
	return m_sessions;
}

/* PrivateXinxProject */

class PrivateXinxProject {
public:
	PrivateXinxProject( XinxProject * parent );
	~PrivateXinxProject();

	static QString getValue( const QDomDocument & document, const QString & node );
	static void setValue( QDomDocument & document, const QString & node, const QString & value );

	static QStringList loadList( QDomDocument document, const QString & list, const QString & element );
	static void saveList( QDomDocument & document, const QString & list, const QString & element, const QStringList & slist );

	QString processPath( QString path );

	PrivateXinxProject& operator=( const PrivateXinxProject& p );

	XinxProjectSession * m_session;

	QString m_fileName;
	int m_version;

	QStringList m_searchPathList, m_specifiquePrefixes, m_preloadedFiles;
	int m_indexOfSpecifiquePath;
	QString m_projectName, m_defaultLang, m_defaultNav;
	QString m_projectPath, m_specifiquePathName, m_specifiquePrefix;
	QString m_logProjectDirectory;
	XinxProject::ProjectOptions m_projectOptions;
	QString m_projectRCS;

	QHash<QString,QVariant> m_properties;

	ContentViewCache * m_cache;
private:
	XinxProject * m_parent;
};

PrivateXinxProject::PrivateXinxProject( XinxProject * parent ) {
	m_parent = parent;

	m_cache = 0;

	m_defaultLang = "FRA";
	m_defaultNav  = "NAV/MIC";

	m_searchPathList.append( "langue/<lang>/nav/<project>" );
	m_searchPathList.append( "langue/<lang>/nav" );
	m_searchPathList.append( "." );
	m_searchPathList.append( "langue/<lang>" );
	m_searchPathList.append( "langue" );
	m_indexOfSpecifiquePath = 0;

	m_specifiquePathName = XINXConfig::self()->config().project.defaultProjectPathName;
	m_projectOptions |= XinxProject::hasSpecifique;

	m_preloadedFiles.append( "langue/js/generix.js" );

	m_session = new XinxProjectSession();
}

PrivateXinxProject::~PrivateXinxProject() {
	delete m_session;
}

PrivateXinxProject& PrivateXinxProject::operator=( const PrivateXinxProject& p ) {
	m_searchPathList        = p.m_searchPathList;
	m_indexOfSpecifiquePath = p.m_indexOfSpecifiquePath;
	m_projectName		= p.m_projectName;
	m_defaultLang		= p.m_defaultLang;
	m_defaultNav		= p.m_defaultNav;
	m_projectPath		= p.m_projectPath;
	m_specifiquePathName	= p.m_specifiquePathName;
	m_specifiquePrefix	= p.m_specifiquePrefix;
	m_projectOptions	= p.m_projectOptions;
	m_projectRCS		= p.m_projectRCS;
	m_logProjectDirectory   = p.m_logProjectDirectory;
	m_preloadedFiles	= p.m_preloadedFiles;

	return *this;
}

QString PrivateXinxProject::getValue( const QDomDocument & document, const QString & node ) {
	QDomElement root = document.documentElement();
	QDomElement elt  = root.firstChildElement( node );
	return elt.text();
}

void PrivateXinxProject::setValue( QDomDocument & document, const QString & node, const QString & value ) {
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

QStringList PrivateXinxProject::loadList( QDomDocument document, const QString & list, const QString & element ) {
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

void PrivateXinxProject::saveList( QDomDocument & document, const QString & list, const QString & element, const QStringList & slist ) {
	QDomElement root = document.documentElement();
	QDomElement dlist = document.createElement( list );
	root.appendChild( dlist );

	foreach( const QString & s, slist ) {
		QDomElement delement = document.createElement( element );
		dlist.appendChild( delement );

		QDomText text = document.createTextNode( s );
		delement.appendChild( text );
	}
}

QString PrivateXinxProject::processPath( QString path ) {
	path.replace( "<lang>", m_defaultLang.toLower() );
	path.replace( "<LANG>", m_defaultLang.toUpper() );
	path.replace( "<nav>", m_defaultNav.toLower() );
	path.replace( "<NAV>", m_defaultNav.toUpper() );
	path.replace( "<project>", m_specifiquePathName.toLower() );
	path.replace( "<PROJECT>", m_specifiquePathName.toUpper() );
	path.replace( "<prefix>", m_specifiquePrefix.toLower() );
	path.replace( "<PREFIX>", m_specifiquePrefix.toUpper() );

	return QDir( m_parent->projectPath() ).absoluteFilePath( path );
}

/* XinxProject */

XinxProject::XinxProject() : QObject() {
	d = new PrivateXinxProject( this );
}

XinxProject::XinxProject( const XinxProject & object ) : QObject() {
	d = new PrivateXinxProject( this );
	*d = *(object.d);
}

XinxProject::XinxProject( const QString & project ) : QObject() {
	d = new PrivateXinxProject( this );
	loadFromFile( project );
}

XinxProject::~XinxProject() {
	delete d;
}

XinxProject& XinxProject::operator=(const XinxProject& p) {
	*d = *(p.d);
	return *this;
}

void XinxProject::loadFromFile( const QString & filename ) {
	QFile file(filename);
	QDomDocument document;

	// Open the file
	if (! file.open(QFile::ReadOnly | QFile::Text) )
		throw XinxProjectException( tr("Cannot read file %1:\n%2.").arg( filename ).arg( file.errorString() ) );

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;
	if ( ! document.setContent( &file, true, &errorStr, &errorLine, &errorColumn ) )
		throw XinxProjectException( tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr) );

	QDomElement root = document.documentElement();

	// Test if Project File
	if( ( root.tagName() != "XinxProject" ) && ( root.tagName() != "XSLProject" ) )
		throw XinxProjectException( tr("The file isn't a XINX Project. The root name must be \"XinxProject\", not \"%1\".").arg( root.tagName() ) );

	d->m_version = PrivateXinxProject::getValue( document, "xinx_version" ).isEmpty() ? 0 : PrivateXinxProject::getValue( document, "xinx_version" ).toInt();
	if( d->m_version > XINX_PROJECT_VERSION )
		throw XinxProjectException( tr("The file is a too recent XINX Project") );
	if( d->m_version < XINX_PROJECT_VERSION )
		throw XinxProjectException( tr("The project can't be opened. Please use the XINX Project Wizard."), true );


	d->m_fileName = filename;

	// Load values
	d->m_projectName         = PrivateXinxProject::getValue( document, "name" );
	d->m_defaultLang         = PrivateXinxProject::getValue( document, "lang" );
	d->m_defaultNav          = PrivateXinxProject::getValue( document, "nav" );
	d->m_projectPath         = QFileInfo( d->m_fileName ).absoluteDir().absoluteFilePath( PrivateXinxProject::getValue( document, "project" ) );
	d->m_specifiquePrefix    = PrivateXinxProject::getValue( document, "prefix" );
	d->m_projectRCS 		 = PrivateXinxProject::getValue( document, "rcs" );
	QString path;

	QStringList spl = PrivateXinxProject::loadList( document, "paths", "path" );
	if( spl.size() > 0 ) {
		d->m_searchPathList = spl;
		d->m_indexOfSpecifiquePath = PrivateXinxProject::getValue( document, "indexOfSpecifiquePath" ).toInt();
		if( ( d->m_indexOfSpecifiquePath < 0 ) || ( d->m_indexOfSpecifiquePath >= spl.size() ) ) d->m_indexOfSpecifiquePath = 0;
	} else {
		d->m_indexOfSpecifiquePath = 0;
	}
	d->m_specifiquePathName = PrivateXinxProject::getValue( document, "specifiquePathName" );
	d->m_projectOptions = ProjectOptions( PrivateXinxProject::getValue( document, "options" ).toInt() );
	d->m_logProjectDirectory = QFileInfo( d->m_fileName ).absoluteDir().absoluteFilePath( PrivateXinxProject::getValue( document, "logProjectDirectory" ) );
	d->m_specifiquePrefixes = PrivateXinxProject::loadList( document, "prefixes", "prefix" );

	if( d->m_specifiquePrefixes.size() == 0 )
		d->m_specifiquePrefixes.append( d->m_specifiquePrefix );

	d->m_preloadedFiles = PrivateXinxProject::loadList( document, "preloadedFiles", "file" );

	QDomElement propertiesElement = root.firstChildElement( "properties" );

	d->m_properties.clear();
	QDomElement node = propertiesElement.firstChildElement();
	while( ! node.isNull() ) {
		QString type  = node.attribute( "type", "QString" ),
				name  = node.tagName(),
				value = node.text();

		if( ! value.isEmpty() ) {
			QVariant::Type variantType = QVariant::nameToType( qPrintable( type ) );
			QVariant p( variantType );
			p.setValue( value );
			d->m_properties[ name ] = p;
		}

		node = node.nextSiblingElement();
	}

	if( ! QDir( d->m_projectPath ).exists() )
		throw XinxProjectException( tr( "Project path (%1) don't exists." ).arg( projectPath() ) );

	try {
		d->m_session->loadFromFile( d->m_fileName + ".session" );
	} catch( XinxProjectException e ) {
		qWarning( qPrintable( e.getMessage() ) );
	}

	emit changed();
}

void XinxProject::saveToFile( const QString & filename ) {
	if( ! filename.isEmpty() ) d->m_fileName = filename;
	if( d->m_fileName.isEmpty() ) return;

	// Save the session file
	d->m_session->saveToFile( d->m_fileName + ".session" );

	// Save attributes
	QDomDocument document( "XinxProject" );
	QDomElement root = document.createElement( "XinxProject" );
	document.appendChild( root );

	PrivateXinxProject::setValue( document, "xinx_version", QString( "%1" ).arg( XINX_PROJECT_VERSION ) );
	PrivateXinxProject::setValue( document, "name", d->m_projectName );
	PrivateXinxProject::setValue( document, "lang", d->m_defaultLang );
	PrivateXinxProject::setValue( document, "nav", d->m_defaultNav );
	PrivateXinxProject::setValue( document, "project", QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( d->m_projectPath ) );
	PrivateXinxProject::setValue( document, "prefix", d->m_specifiquePrefix );
	PrivateXinxProject::setValue( document, "rcs", d->m_projectRCS );
	PrivateXinxProject::saveList( document, "prefixes", "prefix", d->m_specifiquePrefixes );
	PrivateXinxProject::saveList( document, "paths", "path", d->m_searchPathList );
	PrivateXinxProject::saveList( document, "preloadedFiles", "file", d->m_preloadedFiles );
	PrivateXinxProject::setValue( document, "indexOfSpecifiquePath", QString("%1").arg( d->m_indexOfSpecifiquePath ) );
	PrivateXinxProject::setValue( document, "specifiquePathName", d->m_specifiquePathName );
	PrivateXinxProject::setValue( document, "options", QString("%1").arg( d->m_projectOptions ) );
	PrivateXinxProject::setValue( document, "logProjectDirectory", QFileInfo( d->m_fileName ).absoluteDir().relativeFilePath( d->m_logProjectDirectory ) );

	QDomElement propertiesElement = document.createElement( "properties" );
	root.appendChild( propertiesElement );

	foreach( const QString & key, d->m_properties.keys() ) {
		QDomElement propertyElement = document.createElement( key );
		QDomText text = document.createTextNode( d->m_properties[ key ].toString() );
		propertyElement.appendChild( text );
		propertyElement.setAttribute( "type", d->m_properties[ key ].typeName() );
		propertiesElement.appendChild( propertyElement );
	}

	// Open the file
	static const int IndentSize = 3;
	QFile file( d->m_fileName );
	if ( !file.open(QFile::WriteOnly | QFile::Text) )
		throw XinxProjectException( QObject::tr("Cannot write file %1:\n%2.").arg( d->m_fileName ).arg( file.errorString()) );

	QTextStream text( &file );
	document.save( text, IndentSize );
}

void XinxProject::saveOnlySession() {
	if( d->m_fileName.isEmpty() ) return;
	d->m_session->saveToFile();
}

XinxProject::ProjectOptions & XinxProject::options() {
	return d->m_projectOptions;
}

void XinxProject::setOptions( XinxProject::ProjectOptions options ) {
	if( d->m_projectOptions != options ) {
		d->m_projectOptions = options;
		emit changed();
	}
}

const QString & XinxProject::projectRCS() const {
	return d->m_projectRCS == "no" ? d->m_projectRCS = QString() : d->m_projectRCS;
}

void XinxProject::setProjectRCS( const QString & value ) {
	if( d->m_projectRCS != value ) {
		d->m_projectRCS = value;
		emit changed();
	}
}

QString XinxProject::projectName() const {
	return d->m_projectName;
}

void XinxProject::setProjectName( const QString & value ) {
	if( d->m_projectName != value ) {
		d->m_projectName = value;
		emit changed();
	}
}

QString XinxProject::defaultLang() const {
	return d->m_defaultLang;
}

void XinxProject::setDefaultLang( const QString & value ) {
	if( d->m_defaultLang != value ) {
		d->m_defaultLang = value;
		emit changed();
	}
}

QString XinxProject::defaultNav() const {
	return d->m_defaultNav;
}

void XinxProject::setDefaultNav( const QString & value ) {
	if( d->m_defaultNav != value ) {
		d->m_defaultNav = value;
		emit changed();
	}
}

QString XinxProject::projectPath() const {
	return QDir( d->m_projectPath ).absolutePath();
}

void XinxProject::setProjectPath( const QString & value ) {
	if( d->m_projectPath != value ) {
		d->m_projectPath = value;
		emit changed();
	}
}

void XinxProject::setSpecifiquePathName( const QString & value ) {
	if( d->m_specifiquePathName != value ) {
		d->m_specifiquePathName = value;
		emit changed();
	}
}

QString XinxProject::specifiquePathName() {
	return d->m_specifiquePathName;
}

QString XinxProject::specifiquePrefix() const {
	return d->m_specifiquePrefix;
}

void XinxProject::setSpecifiquePrefix( const QString & value ) {
	if( d->m_specifiquePrefix != value ) {
		d->m_specifiquePrefix = value;
		if( ! d->m_specifiquePrefixes.contains( value ) )
			d->m_specifiquePrefixes.append( value );
		emit changed();
	}
}

QStringList & XinxProject::specifiquePrefixes() {
	return d->m_specifiquePrefixes;
}

QStringList & XinxProject::searchPathList() {
	return d->m_searchPathList;
}

QString XinxProject::processedSpecifiquePath() const {
	Q_ASSERT( d->m_indexOfSpecifiquePath >= 0 );
	Q_ASSERT( d->m_searchPathList.size() > 0 );

	return d->processPath( d->m_searchPathList.at( d->m_indexOfSpecifiquePath ) );
}

QStringList XinxProject::processedSearchPathList() {
	QStringList list;
	foreach( const QString & path, searchPathList() ) {
		list << d->processPath( path );
	}
	return list;
}

int XinxProject::indexOfSpecifiquePath() const {
	return d->m_indexOfSpecifiquePath;
}

void XinxProject::setIndexOfSpecifiquePath( int value ) {
	d->m_indexOfSpecifiquePath = value;
}

QStringList & XinxProject::preloadedFiles() {
	return d->m_preloadedFiles;
}

void XinxProject::preloadFilesCache() {
	if( ! d->m_cache ) {
		d->m_cache = new ContentViewCache( this );
		d->m_cache->initializeCache( qApp->activeWindow() ? qApp->activeWindow() : 0 );
	} else
		d->m_cache->initializeCache( qApp->activeWindow() ? qApp->activeWindow() : 0 );
}

ContentViewCache * XinxProject::preloadedFilesCache() {
	if( ! d->m_cache ) {
		preloadFilesCache();
	}
	return d->m_cache;
}

const QString & XinxProject::fileName() const {
	return d->m_fileName;
}

void XinxProject::setLogProjectDirectory( const QString & value ) {
	if( d->m_logProjectDirectory != value ) {
		d->m_logProjectDirectory = value;
		emit changed();
	}
}

const QString & XinxProject::logProjectDirectory() {
	return d->m_logProjectDirectory;
}

XinxProjectSession * XinxProject::session() const {
	return d->m_session;
}

void XinxProject::writeProperty( const QString & key, QVariant value ) {
	if( d->m_properties.value( key ) != value ) {
		d->m_properties[ key ] = value;
		emit changed();
	}
}

QVariant XinxProject::readProperty( const QString & key ) {
	return d->m_properties.value( key );
}

bool XinxProject::blockSignals( bool block ) {
	bool result = QObject::blockSignals( block );
	if( ! block ) emit changed();
	return result;
}


/* XINXProjectManager */

XINXProjectManager::XINXProjectManager() : m_project(0) {

}

XINXProjectManager::~XINXProjectManager() {
	if( s_self == this )
		s_self = 0;
	delete m_project;
}

XINXProjectManager * XINXProjectManager::self() {
	if( s_self == 0 ) {
		s_self = new XINXProjectManager();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

void XINXProjectManager::setCurrentProject( XinxProject * project ) {
	if( m_project )
		m_project->disconnect( this );

	m_project = project;
	XINXConfig::self()->config().project.lastOpenedProject = project->fileName();
	m_project->preloadedFilesCache();

	connect( m_project, SIGNAL(changed()), this, SIGNAL(changed()) );
	emit changed();
}

XinxProject * XINXProjectManager::project() const {
	return m_project;
}

void XINXProjectManager::deleteProject() {
	delete m_project;
	XINXConfig::self()->config().project.lastOpenedProject = QString();
	m_project = NULL;
	emit changed();
}
