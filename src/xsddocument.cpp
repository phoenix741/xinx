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
 
#include "xsddocument.h"

#include <QDomDocument>

/* XSDSchema */

XSDSchema::XSDSchema( const QString & fileName ) {
	loadFileByName( fileName );
}

void XSDSchema::loadFileByName( const QString & fileName ) {
	QDomDocument document;
	
	QString errorMsg;
	int errorLine = 0, errorColumn = 0;
	if( ! document.setContent( fileName, true, &errorMsg, &errorLine, &errorColumn ) ) {
		// ERROR MSG
		return;
	}
	
	loadElement( document.documentElement() );
}

void XSDSchema::loadElement( const QDomElement & element ) {
	QDomElement node = element.firstChildElement();
	while( ! node.isNull() ) {
		if( ( node.prefix() == "xs" ) && ( node.tagName() == "include" ) ) {
			loadFileByName( node.attribute( "schemaLocation" ) );
		} else
		if( ( node.prefix() == "xs" ) && ( node.tagName() == "element" ) ) {
			XSDElement * element = new XSDElement( node );
			m_elements.insert( element->name(), element );
		} else
		if( ( node.prefix() == "xs" ) && ( node.tagName() == "complexType" ) ) {
			XSDComplexType * element = new XSDComplexType( node );
			m_complexType.insert( element->name(), element );
		} else
		if( ( node.prefix() == "xs" ) && ( node.tagName() == "simpleType" ) ) {
			XSDSimpleType * element = new XSDSimpleType( node );
			m_simpleType.insert( element->name(), element );
		} 
		
		node = node.nextSiblingElement();
	}
}

/* XSDAnnotation */

void XSDAnnotation::loadElement( const QDomElement & element ) {
	QDomNode text = element.firstChild();
			
	while( ! text.isNull() ) {
		if( text.isText() ) 
			m_documentation += text.toText().data();
				
		text = text.nextSibling();
	}
}

/* XSDAnnotedObject */

void XSDAnnotedObject::loadElement( const QDomElement & element ) {
	QDomElement node = element.firstChildElement();
	
	while( ! node.isNull() ) {
		if( ( element.prefix() == "xs" ) && ( element.tagName() == "documentation" ) ) {
			m_annotation = new XSDAnnotation( node );
		}			
		node = node.nextSiblingElement();
	}
}

/* XSDAttribute */

void XSDAttribute::loadElement( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	m_typeName = element.attribute( "type" );
	m_defaultValue = element.attribute( "default" );
	m_use = element.attribute( "use" );
	
	// TODO : m_type
}

