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

// Xinx header
#include "xmlschema.h"

// Qt header
#include <QFile>
#include <QDomDocument>

/* XsdNodeItem */

XsdItem::XsdItem( XmlSchemaFile * schema, QObject * parent ) : QObject( parent ), m_readOnly( qobject_cast<XsdItem*>( parent ) ? qobject_cast<XsdItem*>( parent )->m_readOnly : true ), m_schema( schema ) {

}

XmlSchemaFile * XsdItem::schema() const {
	return m_schema;
}

bool XsdItem::readOnly() const {
	return m_readOnly;
}

void XsdItem::setReadOnly( bool readonly ) {
	m_readOnly = readonly;
}

/* XsdAnnotationItem */

XsdAnnotationItem::XsdAnnotationItem( XmlSchemaFile * schema, QObject * parent, const QString & documentation ) : XsdItem( schema, parent ), m_documentation( documentation ) {

}

const QString & XsdAnnotationItem::documentation() const {
	return m_documentation;
}

void XsdAnnotationItem::setDocumentation( const QString & value ) {
	m_documentation = value;
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

/* XsdRestrictionItem */

XsdRestrictionItem::XsdRestrictionItem( XmlSchemaFile * schema, QObject * parent, const QString & base ) : XsdItem( schema, parent ), m_base( base ) {

}

const QString & XsdRestrictionItem::base() {
	return m_base;
}

void XsdRestrictionItem::setBase( const QString & value ) {
	m_base = value;
}

const QString & XsdRestrictionItem::minValue() const {
	return m_minValue;
}

void XsdRestrictionItem::setMinValue( const QString & value ) {
	m_minValue = value;
}

const QString & XsdRestrictionItem::maxValue() const {
	return m_maxValue;
}

void XsdRestrictionItem::setMaxValue( const QString & value ) {
	m_maxValue = value;
}

const QStringList & XsdRestrictionItem::enumeration() const {
	return m_enumeration;
}

void XsdRestrictionItem::setEnumeration( const QStringList & value ) {
	m_enumeration = value;
}

const QString & XsdRestrictionItem::pattern() const {
	return m_pattern;
}

void XsdRestrictionItem::setPattern( const QString & pattern ) {
	m_pattern = pattern;
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

/* XsdSimpleType */

XsdSimpleType::XsdSimpleType( XmlSchemaFile * schema, QObject * parent, const QString & name ) : XsdItem( schema, parent ), m_name( name ) {

}

const QString & XsdSimpleType::name() const {
	return m_name;
}

void XsdSimpleType::setName( const QString & value ) {
	m_name = value;
}

/* XsdComplexeType */

XsdComplexeType::XsdComplexeType( XmlSchemaFile * schema, QObject * parent, const QString & name, const QString & inheritsFrom ) : XsdSimpleType( schema, parent, name ), m_inheritsFrom( inheritsFrom ) {

}

const QString & XsdComplexeType::inheritsFrom() const {
	return m_inheritsFrom;
}

void XsdComplexeType::setInheritsFrom( const QString & value ) {
	m_inheritsFrom = value;
}

/* XmlSchemaFile */

XmlSchemaFile::XmlSchemaFile() : XsdItem( this ) {

}

XmlSchemaFile::XmlSchemaFile( QFile * file ) : XsdItem( this ) {
	loadFromFile( file );
}

void XmlSchemaFile::loadFromFile( const QString & filename ) {
	QFile f( filename );
	if( f.open( QIODevice::ReadOnly ) ) {
		loadFromFile( &f );
	}
	f.close();
}

void XmlSchemaFile::loadFromFile( QFile * file, bool isInclude ) {
	QDomDocument doc;
	if( ! doc.setContent( file, true ) ) return;

	QDomElement xsSchema = doc.documentElement();
	QDomElement childElement = xsSchema.firstChildElement();
	while( ! childElement.isNull() ) {
		XsdItem * item = createNewItem( this, childElement, isInclude );

		childElement = childElement.nextSiblingElement();
	}
}

XsdAnnotationItem * XmlSchemaFile::createXsdAnnotation( XsdItem * parent, const QDomElement & e ) {
	QDomElement documentation = e.firstChildElement( "documentation" );
	return new XsdAnnotationItem( this, parent, documentation.text() );
}

XsdSimpleType * XmlSchemaFile::createNewSimpleType( XsdItem * parent, const QDomElement & e ) {

}

XsdItem * XmlSchemaFile::createNewItem( XsdItem * parent, const QDomElement & e, bool isInclude ) {
	QString elementName = e.nodeName();
	XsdItem * item = 0;

	if( elementName == "complexType" ) {

	} else if( elementName == "simpleType" ) {
	} else if( elementName == "element" ) {
	} else if( ( elementName == "include" ) || ( elementName == "import" ) ) {
	}

	if( item ) {
		item->setReadOnly( isInclude );
		QDomElement annotation = e.firstChildElement( "annotation" );
		if( ! annotation.isNull() )
			createXsdAnnotation( item, annotation );
	}
	return item;
}
