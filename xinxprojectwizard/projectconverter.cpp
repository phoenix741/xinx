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

/* Editor */

class Editor {
public:
	QString m_fileName, m_content;
	int m_position;
	bool m_isModified;
	
	QList<int> m_bookmarks;
private:
};

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
				session = session.firstChildElement( "Editor" );
				while( ! session.isNull() ) {
					m_nbSession++;
					session = session.nextSiblingElement( "Editor" );
				}
			}
			emit setMaximum( ( m_nbSession + 1 ) * ( m_version - XINX_PROJECT_VERSION ) );
		}
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
		int options = 1;
		// type ==> options
		QDomElement type = m_projectDocument.documentElement().firstChildElement( "type" );
		if( type.text() == "services" )
			options = 3;
		m_projectDocument.documentElement().removeChild( type );

		QDomElement opt = m_projectDocument.createElement( "options" );
		m_projectDocument.documentElement().appendChild( opt );
		QDomText text = m_projectDocument.createTextNode( QString( options ) );
		opt.appendChild( text );
		
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
		
		// Lecture de opened...
		
		emit setValue( process++ );
	}
	case XINX_PROJECT_VERSION_1: {
		emit setValue( process++ );
	}
	case XINX_PROJECT_VERSION_2: {
		if( m_version == XINX_PROJECT_VERSION_2 ) { // Not a previous version, only this version
			
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
}

