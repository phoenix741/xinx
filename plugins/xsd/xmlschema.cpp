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

void XsdItem::loadFromElement( const QDomElement & e ) {
	QDomElement annotation = e.firstChildElement( "annotation" );
	if( ! annotation.isNull() ) {
		XsdAnnotationItem * a = new XsdAnnotationItem( schema(), this );
		a->loadFromElement( annotation );
	}
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

void XsdAnnotationItem::loadFromElement( const QDomElement & e ) {
	QDomElement documentation = e.firstChildElement( "documentation" );
	setDocumentation( documentation.text() );
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

void XsdElementItem::loadFromElement( const QDomElement & e ) {
	XsdItem::loadFromElement( e );

	setName( e.attribute( "name" ) );
	setType( e.attribute( "type" ) );

	if( ! e.attribute( "minOccurs" ).isNull() )
		setMinOccurs( e.attribute( "minOccurs" ).toInt() );
	else
		setMinOccurs( 0 );

	if( ! e.attribute( "maxOccurs" ).isNull() )
		setMaxOccurs( e.attribute( "maxOccurs" ).toInt() );
	else
		setMaxOccurs( 1 );
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

void XsdRestrictionItem::loadFromElement( const QDomElement & e ) {
	XsdItem::loadFromElement( e );
	setBase( e.attribute( "base" ) );

	QDomElement node = e.firstChildElement();
	while( ! node.isNull() ) {
		if( node.nodeName() == "minLength" ) {
			setMinValue( node.attribute( "value" ) );
		} else if( node.nodeName() == "maxLength" ) {
			setMaxValue( node.attribute( "value" ) );
		} else if( node.nodeName() == "enumeration" ) {
			m_enumeration.append( node.attribute( "value" ) );
		} else if( node.nodeName() == "pattern" ) {
			setPattern( node.attribute( "value" ) );
		}

		node = node.nextSiblingElement();
	}
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

XsdElementItem * XsdSequenceItem::createXsdElementItem( XmlSchemaFile * schema, QObject * parent, const QDomElement & e ) {
	XsdElementItem * item = 0;
	if( e.nodeName() == "element" )
		item = new XsdElementItem( schema, parent );
	else if( e.nodeName() == "complexContent" )
		item = new XsdComplexeContentItem( schema, parent );
	else if( e.nodeName() == "sequence" )
		item = new XsdSequenceItem( schema, parent );
	else if( e.nodeName() == "choice" )
		item = new XsdChoiceItem( schema, parent );
	if( item )
		item->loadFromElement( e );
	return item;
}

void XsdSequenceItem::loadFromElement( const QDomElement & e ) {
	XsdElementItem::loadFromElement( e );

	QDomElement node = e.firstChildElement();
	while( ! node.isNull() ) {
		createXsdElementItem( schema(), this, node );

		node = node.nextSiblingElement();
	}

}

/* XsdChoiceItem */

XsdChoiceItem::XsdChoiceItem( XmlSchemaFile * schema, QObject * parent ) : XsdSequenceItem( schema, parent ) {

}

/* XsdComplexeContentItem */

XsdComplexeContentItem::XsdComplexeContentItem( XmlSchemaFile * schema, QObject * parent, const QString & base ) : XsdSequenceItem( schema, parent ), m_base( base ) {

}

const QString & XsdComplexeContentItem::base() const {
	return m_base;
}

void XsdComplexeContentItem::setBase( const QString & value ) {
	m_base = value;
}

void XsdComplexeContentItem::loadFromElement( const QDomElement & e ) {
	XsdSequenceItem::loadFromElement( e );
	setBase( e.attribute( "base" ) );
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

void XsdSimpleType::loadFromElement( const QDomElement & e ) {
	XsdItem::loadFromElement( e );
	setName( e.attribute( "name" ) );

	QDomElement restrictionElement = e.firstChildElement( "restriction" );

	XsdSimpleType * type = new XsdSimpleType( schema(), this );
	type->loadFromElement( e );

	XsdRestrictionItem * restriction = new XsdRestrictionItem( schema(), this );
	restriction->loadFromElement( restrictionElement );
}

/* XsdComplexeType */

XsdComplexeType::XsdComplexeType( XmlSchemaFile * schema, QObject * parent, const QString & name ) : XsdSimpleType( schema, parent, name ) {

}

/* XmlSchemaFile */

XmlSchemaFile::XmlSchemaFile() : XsdItem( this ) {

}

XmlSchemaFile::XmlSchemaFile( QIODevice * file ) : XsdItem( this ) {
	loadFromFile( file );
}

void XmlSchemaFile::loadFromFile( const QString & filename ) {
	QFile f( filename );
	if( f.open( QIODevice::ReadOnly ) ) {
		loadFromFile( &f );
	}
	f.close();
}

void XmlSchemaFile::loadFromFile( QIODevice * file, bool isInclude ) {
	QDomDocument doc;
	if( ! doc.setContent( file, true ) ) return;

	QDomElement xsSchema = doc.documentElement();
	QDomElement childElement = xsSchema.firstChildElement();
	while( ! childElement.isNull() ) {
		XsdItem * item = createNewItem( this, childElement, isInclude );

		childElement = childElement.nextSiblingElement();
	}
}

XsdItem * XmlSchemaFile::createNewItem( XsdItem * parent, const QDomElement & e, bool isInclude ) {
	QString elementName = e.nodeName();
	XsdItem * item = 0;

	if( elementName == "element" ) {
		item = new XsdElementItem( this, this );
	} else if( elementName == "complexType" ) {
		item = new XsdComplexeType( this, this );
	} else if( elementName == "simpleType" ) {
		item = new XsdSimpleType( this, this );
	} else if( ( elementName == "include" ) || ( elementName == "import" ) ) {

	}

	if( item ) {
		item->setReadOnly( isInclude );
		item->loadFromElement( e );
	}
	return item;
}
