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
#include <QDomDocument>

/* ProjectConverter */

ProjectConverter::ProjectConverter( const QString & filename ) : m_filename( filename ), m_version( XINX_PROJECT_VERSION_0 ) {
	m_type = tr("XINX Project file");
	
	QFile file( filename );
	
	if( file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
		QDomDocument document;
		if(! document.setContent( &file ) ) throw XinxException( "Not an xml file." );
		QDomElement rootElement = document.documentElement();
		if( rootElement.tagName() != "XSLProject" ) throw XinxException( "Not a managed project file." );
		
		QDomElement version = rootElement.firstChildElement( "xinx_version" );
		if( ! version.isNull() )
			m_version = version.text().toInt();
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
