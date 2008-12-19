/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#include "xmlschema.h"

/* XsdNodeItem */

XsdItem::XsdItem( XmlSchemaFile * schema, QObject * parent ) : QObject( parent ), m_schema( schema ) {

}

XmlSchemaFile * XsdItem::schema() const {
	return m_schema;
}

/* XsdAttributeItem */

XsdAttributeItem::XsdAttributeItem( XmlSchemaFile * schema, QObject * parent, const QString & name, const QString & value, const QString & use ) : XsdItem( schema, parent ), m_name( name ), m_value( value ), m_use( use ) {

}

const QString & XsdAttributeItem::name() const {
	return m_name;
}

void XsdAttributeItem::setName( const QString & value ) {
	m_name = value;
}

const QString & XsdAttributeItem::value() const {
	return m_value;
}

void XsdAttributeItem::setValue( const QString & value ) {
	m_value = value;
}

const QString & XsdAttributeItem::use() const {
	return m_use;
}

void XsdAttributeItem::setUse( const QString & value ) {
	m_use = value;
}

/* XsdElementItem */

XsdElementItem::XsdElementItem( XmlSchemaFile * schema, QObject * parent, const QString & name, const QString & type, int minOccurs, int maxOccurs ) : XsdItem( schema, parent ), m_name( name ), m_type( type ), m_minOccurs( minOccurs ), m_maxOccurs( maxOccurs ) {

}

const QString & XsdElementItem::name() const {
	return m_name;
}

void XsdElementItem::setName( const QString & value ) {
	m_name = value;
}

const QString & XsdElementItem::type() const {
	return m_type;
}

void XsdElementItem::setType( const QString & value ) {
	m_type = value;
}

int XsdElementItem::minOccurs() const {
	return m_minOccurs;
}

void XsdElementItem::setMinOccurs( int value ) {
	m_minOccurs = value;
}

int XsdElementItem::maxOccurs() const {
	return m_maxOccurs;
}

void XsdElementItem::setMaxOccurs( int value ) {
	m_maxOccurs = value;
}

/* XsdSequenceItem */

XsdSequenceItem::XsdSequenceItem( XmlSchemaFile * schema, QObject * parent ) : XsdElementItem( schema, parent ) {

}

QList<XsdElementItem*> XsdSequenceItem::elements() const {
	QObjectList childrens = children();
	QList<XsdElementItem*> list;
	foreach( QObject * child, childrens ) {
		if( qobject_cast<XsdElementItem*>( child ) ) {
			list << qobject_cast<XsdElementItem*>( child );
		}
	}
	return list;
}

/* XsdChoiceItem */
XsdChoiceItem::XsdChoiceItem( XmlSchemaFile * schema, QObject * parent ) : XsdSequenceItem( schema, parent ) {

}


/* XmlSchemaFile */

XmlSchemaFile::XmlSchemaFile() : XsdItem( this ) {

}

XmlSchemaFile::XmlSchemaFile( QFile * file ) : XsdItem( this ) {
	loadFromFile( file );
}

void XmlSchemaFile::loadFromFile( const QString & filename ) {

}

void XmlSchemaFile::loadFromFile( QFile * file ) {

}
