/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke   *
 *   ulrich.vdh@free.fr   *
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

// Xinx header
#include "projectconverter.h"

// Qt header
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QVariant>
#include <QTextStream>

/* Editor */

class Editor {
public:
	void loadFromVersion1( QDomElement & element );
	void loadFromVersion2( QDomElement & element );
	
	void saveToCurrentVersion( QDomDocument & document, QDomElement & element );
private:
	void getSerializedData( QDataStream & stream, int & type, QVariant & data );

	enum serializedDatas {
		SERIALIZED_FILENAME         = 1001, ///< The serialized filename
		SERIALIZED_HIGHLIGHTER_TYPE = 1002, ///< The highlighter type
		SERIALIZED_FILE_TYPE        = 1003, ///< The file type
		SERIALIZED_POSITION         = 1004, ///< The position of the text
		SERIALIZED_MODIFIED         = 1005, ///< The state of the editor : Is it modified ?
		SERIALIZED_CONTENT          = 1006, ///< The saved content.
		SERIALIZED_BOOKMARK         = 1007, ///< The bookmark
		SERIALIZED_ENDOFFILEEDITOR  = 1999, ///< The last element;
        SERIALIZED_SERVICES         = 2001, ///< The serialized services name
        SERIALIZED_ACTIONS          = 2002, ///< The serialized actions name
        SERIALIZED_PARAMS           = 2003, ///< The serialized params name
        SERIALIZED_KEY              = 2004, ///< The serialized params name
        SERIALIZED_VALUE            = 2005, ///< The serialized params name
        SERIALIZED_ENDOFWSEDITOR    = 2999  ///< The last element;
    };	
	void saveField( QDomDocument & document, QDomElement & element, QString name, QVariant value );
	
	bool m_isServices;
	
	QString m_fileName, m_content;
	int m_position;
	bool m_isModified;
	
	QList<int> m_bookmarks;
	
	QString m_service, m_action, m_param;
	QList< QPair<QString,QString> > m_servicesContent;
};

void Editor::getSerializedData( QDataStream & stream, int & type, QVariant & data ) {
	stream >> type;
	stream >> data;
}

void Editor::loadFromVersion1( QDomElement & element ) {
	m_position = element.attribute( "position" ).toInt();
	m_fileName = element.attribute( "filename" );
	m_isModified = (bool)element.attribute( "ismodified" ).toInt();
	
	if( element.attribute( "class" ) == "WebServicesEditor" ) {
		m_isServices = true;
		
		m_service = element.attribute( "service" );
		m_action  = element.attribute( "action" );
		m_param   = element.attribute( "param" );
		
		QDomElement co = element.firstChildElement();
		while( ! co.isNull() ) {
			m_servicesContent.append( qMakePair( co.tagName(), co.text() ) );
			
			co = co.nextSiblingElement();
		}
	} else {
		m_isServices = false;
		m_content = element.text();
	}
}

void Editor::loadFromVersion2( QDomElement & element ) {
	QByteArray datas = QByteArray::fromBase64( element.text().toUtf8() );
	QDataStream stream( datas ); 
	QString className;
	stream >> className;
	m_isServices = className == "WebServicesEditor";
	int type;
	QVariant value;
	QString key;
	do {
		getSerializedData( stream, type, value );
		switch( type ) {
		case SERIALIZED_FILENAME:
			m_fileName = value.toString();
			break;
		case SERIALIZED_HIGHLIGHTER_TYPE:
			break;
		case SERIALIZED_FILE_TYPE:
			break;
		case SERIALIZED_POSITION:
			m_position = value.toInt();
			break;
		case SERIALIZED_MODIFIED:
			m_isModified = value.toBool();
			break;
		case SERIALIZED_CONTENT:
			m_content = value.toString();
			break;
		case SERIALIZED_BOOKMARK:
			m_bookmarks.append( value.toInt() );
			break;
		case SERIALIZED_ENDOFFILEEDITOR:
			break;
		case SERIALIZED_SERVICES:
			m_service = value.toString();
			break;
		case SERIALIZED_ACTIONS:
			m_action = value.toString();
			break;
		case SERIALIZED_PARAMS:
			m_param = value.toString();
			break;
		case SERIALIZED_KEY:
			key = value.toString();
			break;
		case SERIALIZED_VALUE:
			m_servicesContent.append( qMakePair( key, value.toString() ) );
			break;
		case SERIALIZED_ENDOFWSEDITOR:
			break;
		}
	} while( ( type != Editor::SERIALIZED_ENDOFFILEEDITOR ) || ( m_isServices && type != Editor::SERIALIZED_ENDOFWSEDITOR )  );
}

void Editor::saveField( QDomDocument & document, QDomElement & element, QString name, QVariant value ) {
	QDomElement newElement = document.createElement( name );
	newElement.setAttribute( "type", value.typeName() );
	QDomText t = document.createTextNode( value.toString() );
	newElement.appendChild( t );
	element.appendChild( newElement );
}

void Editor::saveToCurrentVersion( QDomDocument & document, QDomElement & element ) {
	element.setTagName( "Editor" );
	
	saveField( document, element, "FileName", m_fileName );
	saveField( document, element, "Position", m_position );
	saveField( document, element, "Modified", m_isModified );
	saveField( document, element, "BookmarkCount", m_bookmarks.size() );
	
	for( int i = 0; i < m_bookmarks.size(); i++ ) {
		saveField( document, element, "Bookmark_" + i, m_bookmarks.at( i ) );
	}
	
	if( m_isServices ) {
		saveField( document, element, "ClassName", "WebServicesEditor" );

		saveField( document, element, "Service", m_service );
		saveField( document, element, "Action", m_action );
		saveField( document, element, "Param", m_param );
		
		for( int i = 0; i < m_servicesContent.size(); i++ ) {
			saveField( document, element, "Key_" + i, m_servicesContent.at( i ).first );
			saveField( document, element, "Value_" + i, m_servicesContent.at( i ).second );
		}
	} else {
		saveField( document, element, "ClassName", "FileEditor" );
		if( ! m_content.isEmpty() )
			saveField( document, element, "Content", m_content );
	}
}

/* ProjectConverter */

ProjectConverter::ProjectConverter( const QString & filename ) : m_filename( filename ), m_version( XINX_PROJECT_VERSION_0 ), m_nbSession( 0 )  {
	m_type = tr("XINX Project file");
	
	QFile file( filename );
	
	if( file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
		if(! m_projectDocument.setContent( &file ) ) throw XinxException( "Not an xml file." );
		QDomElement rootElement = m_projectDocument.documentElement();
		if( rootElement.tagName() != "XSLProject" ) throw XinxException( "Not a managed project file." );
		
		QDomElement version = rootElement.firstChildElement( "xinx_version" );
		if( ! version.isNull() )
			m_version = version.text().toInt();
	
		emit setMaximum( m_version - XINX_PROJECT_VERSION );
		
		// Lecture du fichier de session, mais s'il n'est pas là, pas d'erreur.
		if( m_version > XINX_PROJECT_VERSION_0 ) {
			QFile sessionFile( filename + ".session" );
			if( sessionFile.open( QIODevice::ReadOnly | QIODevice::Text ) && m_sessionDocument.setContent( &sessionFile ) ) {
				QDomElement sessionRootElement = m_sessionDocument.documentElement();
				QDomElement session = sessionRootElement.firstChildElement( "Opened" );
				if( sessionRootElement.tagName() != "Session" ) return;
				QString ed = m_version == XINX_PROJECT_VERSION_1 ? "editor" : "Editor";
				
				session = session.firstChildElement( ed );
				while( ! session.isNull() ) {
					m_nbSession++;
					session = session.nextSiblingElement( ed );
				}
			}
			emit setMaximum( m_nbSession + m_version - XINX_PROJECT_VERSION );
		}
	}
}

void ProjectConverter::save() {
	QFile file( m_filename );
	if( file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
		QTextStream text( &file );
		m_projectDocument.save( text, 2 );
	}
	QFile session( m_filename + ".session" );
	if( session.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
		QTextStream text( &session );
		m_sessionDocument.save( text, 2 );
	}
}

ProjectConverter::~ProjectConverter() {
	
}
	
int ProjectConverter::version() const {
	return m_version;
}

QString ProjectConverter::type() const {
	return m_type;
}

int ProjectConverter::nbSession() const {
	return m_nbSession;
}

void ProjectConverter::process() {
	int process = 0;
	switch( m_version ) {
	case XINX_PROJECT_VERSION_0: {
		// specifique ==> lists
		QDomElement specifique = m_projectDocument.documentElement().firstChildElement( "specifique" );
		QString path = specifique.text();
		path = QFileInfo( m_filename ).absoluteDir().relativeFilePath( path );
		m_projectDocument.documentElement().removeChild( specifique );
		
		QStringList pathList;
		pathList << "langue/<lang>/nav/<project>" << "langue/<lang>/nav" << "." << "langue/<lang>" << "langue" << path;
		int index = pathList.size() - 1;
		
		QDomElement paths = m_projectDocument.createElement( "paths" );
		m_projectDocument.documentElement().appendChild( paths );
		foreach( QString p, pathList ) {
			QDomElement e = m_projectDocument.createElement( "path" );
			paths.appendChild( e );
			QDomText text = m_projectDocument.createTextNode( p );
			e.appendChild( text );
		}
		
		QDomElement indexOfSpecifiquePath = m_projectDocument.createElement( "indexOfSpecifiquePath" );
		m_projectDocument.documentElement().appendChild( indexOfSpecifiquePath );
		QDomText indexElement = m_projectDocument.createTextNode( QString( index ) );
		indexOfSpecifiquePath.appendChild( indexElement );
		
		emit setValue( process++ );
	}
	case XINX_PROJECT_VERSION_1: {
		int options = 1;
		// type ==> options
		QDomElement type = m_projectDocument.documentElement().firstChildElement( "type" );
		if( type.text() == "services" )
			options = 3;
		m_projectDocument.documentElement().removeChild( type );

		QDomElement opt = m_projectDocument.createElement( "options" );
		m_projectDocument.documentElement().appendChild( opt );
		QDomText text = m_projectDocument.createTextNode( QString( "%1" ).arg( options ) );
		opt.appendChild( text );
		
		QDomElement editor = m_sessionDocument.documentElement().firstChildElement( "Opened" ).firstChildElement( "editor" );
		
		while( ! editor.isNull() ) {
			Editor e;
			e.loadFromVersion1( editor );
			e.saveToCurrentVersion( m_sessionDocument, editor );
			emit setValue( process++ );

			editor = editor.nextSiblingElement( "editor" );
		}
		
		emit setValue( process++ );
	}
	case XINX_PROJECT_VERSION_2: {
		if( m_version == XINX_PROJECT_VERSION_2 ) { // Not a previous version, only this version
			QDomElement editor = m_sessionDocument.documentElement().firstChildElement( "Opened" ).firstChildElement( "Editor" );
			
			while( ! editor.isNull() ) {
				Editor e;
				e.loadFromVersion2( editor );
				e.saveToCurrentVersion( m_sessionDocument, editor );
				emit setValue( process++ );
				
				editor = editor.nextSiblingElement( "Editor" );
			}
		}
		
		QString prefix = m_projectDocument.documentElement().firstChildElement( "prefix" ).text();
		QDomElement prefixesElement = m_projectDocument.createElement( "prefixes" );
		m_projectDocument.documentElement().appendChild( prefixesElement );
		QDomElement prefixElement = m_projectDocument.createElement( "prefix" );
		prefixesElement.appendChild( prefixElement );
		QDomText prefixText = m_projectDocument.createTextNode( prefix );
		prefixElement.appendChild( prefixText );
		
		emit setValue( process++ );
	}
	}
	
	QDomElement version_xml = m_projectDocument.documentElement().firstChildElement( "xinx_version" );
	m_projectDocument.documentElement().removeChild( version_xml );
	
	version_xml = m_projectDocument.createElement( "xinx_version" );
	QDomText version_text = m_projectDocument.createTextNode( QString("%1").arg(XINX_PROJECT_VERSION) );
	m_projectDocument.documentElement().appendChild( version_xml );
	version_xml.appendChild( version_text );

	emit setValue( process++ );
}

