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
#include <QValidator>
#include <QApplication>

/* XSDObject */

XSDObject::XSDObject() {
}

XSDObject::~XSDObject() {
	
}

void XSDObject::loadElement( const QDomElement & element ) {
	Q_UNUSED( element );
}


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

XSDSchema::~XSDSchema() {
	qDeleteAll( m_elements.values() );
	qDeleteAll( m_complexType.values() );
	qDeleteAll( m_simpleType.values() );
}

/* XSDAnnotation */

XSDAnnotation::XSDAnnotation( const QDomElement & element ) { 
	loadElement( element ); 
}

void XSDAnnotation::loadElement( const QDomElement & element ) {
	QDomNode text = element.firstChild();
			
	while( ! text.isNull() ) {
		if( text.isText() ) 
			m_documentation += text.toText().data();
				
		text = text.nextSibling();
	}
}

/* XSDAnnotedObject */

XSDAnnotedObject::XSDAnnotedObject( const QDomElement & element ) { 
	loadElement( element ); 
}	

void XSDAnnotedObject::loadElement( const QDomElement & element ) {
	QDomElement node = element.firstChildElement();
	
	while( ! node.isNull() ) {
		if( ( element.prefix() == "xs" ) && ( element.tagName() == "documentation" ) ) {
			m_annotation = new XSDAnnotation( node );
		}			
		node = node.nextSiblingElement();
	}
}

XSDAnnotedObject::~XSDAnnotedObject() {
	delete m_annotation;
}

/* XSDAttribute */

XSDAttribute::XSDAttribute( const QDomElement & element ) { 
	loadElement( element ); 
}

XSDAttribute::~XSDAttribute() {
	delete m_type;
}

void XSDAttribute::loadElement( const QDomElement & element ) {
	m_name = element.attribute( "name" );
	m_typeName = element.attribute( "type" );
	m_defaultValue = element.attribute( "default" );
	m_use = element.attribute( "use" );
	
	m_type = new XSDAttributeType( element );
}

/* XSDAttributeType */

XSDAttributeType::XSDAttributeType( const QDomElement & element ) : XSDAnnotedObject( element ) { 
	loadElement( element ); 
}

XSDAttributeType::~XSDAttributeType() {
	delete m_restriction;
}

void XSDAttributeType::loadElement( const QDomElement & element ) {
	QDomElement node = element.firstChildElement( "restriction" );
	
	while( ! node.isNull() ) {
		if( node.prefix() == "xs" ) {
			m_restriction = new XSDRestriction( node );
			
			break;
		}
		node = node.nextSiblingElement( "restriction" );		
	}
}

/* XSDSimpleType */

XSDSimpleType::XSDSimpleType( const QDomElement & element ) { 
	loadElement( element ); 
}

XSDSimpleType::~XSDSimpleType() {
	delete m_type;
}

void XSDSimpleType::loadElement( const QDomElement & element )  {
	m_name = element.attribute( "name" );
	m_type = new XSDAttributeType( element );
}


/* Les Validators */

class QBoolValidator : public QValidator {
public:
	QBoolValidator( QObject * parent ) : QValidator( parent )  {};
	virtual QValidator::State validate ( QString & input, int & pos ) const;
};

QValidator::State QBoolValidator::validate ( QString & input, int & pos ) const {
	Q_UNUSED( pos );
	
	if( ( input == "true" ) || ( input == "false" ) )
		return QValidator::Acceptable;
	
	if( QString( "true" ).contains( input ) || QString( "false" ).contains( input ) )
		return QValidator::Intermediate;
		
	return QValidator::Invalid;
}

/* XSDRestriction */

XSDRestriction::XSDRestriction( const QDomElement & element ) { 
	loadElement( element ); 
}

void XSDRestriction::loadElement( const QDomElement & element ) {
	m_baseType = element.attributeNS( "xs", "base" );
	
	QDomElement node = element.firstChildElement();
	while( ! node.isNull() ) {
		if( node.prefix() == "xs" ) {
			m_properties.insert( node.tagName(), node.attribute( "value" ) );
		}
		
		node = node.nextSiblingElement();
	}
}

QString XSDRestriction::getProperty( const QString & property, XSDSchema * schema ) {
	if( ! m_properties.value( property ).isEmpty() ) {
		return m_properties.value( property );
	} else if( schema ) {
		XSDSimpleType * simpleType = schema->simpleType()[ m_baseType ];
		if( simpleType && simpleType->type() && simpleType->type()->restriction() )
			return simpleType->type()->restriction()->getProperty( property, schema );
	}
	return QString();
}

QList<QString> XSDRestriction::getProperties( const QString & property, XSDSchema * schema ) {
	QList<QString> properties;
	if( schema ) {
		XSDSimpleType * simpleType = schema->simpleType()[ m_baseType ];
		if( simpleType && simpleType->type() && simpleType->type()->restriction() )
			properties = simpleType->type()->restriction()->getProperties( property, schema );
	}
	properties += m_properties.values( property );
	
	return properties;
}

QList<QString> XSDRestriction::enumerations( XSDSchema * schema ) {
	return getProperties( "enumeration", schema );
}

QString XSDRestriction::whiteSpace( XSDSchema * schema ) {
	return getProperty( "whiteSpace", schema );
}

int XSDRestriction::minLength( XSDSchema * schema ) {
	return getProperty( "minLength", schema ).toInt();
}

int XSDRestriction::maxLength( XSDSchema * schema ) {
	return getProperty( "maxLength", schema ).toInt();
}

int XSDRestriction::length( XSDSchema * schema ) {
	return getProperty( "length", schema ).toInt();
}

QRegExp XSDRestriction::pattern( XSDSchema * schema ) {
	return QRegExp( getProperty( "pattern", schema ) );
}

int XSDRestriction::totalDigits( XSDSchema * schema ) {
	if( getProperty( "totalDigits", schema ).isEmpty() )
		return -1;
	else
		return getProperty( "totalDigits", schema ).toInt();
}

int XSDRestriction::fractionDigits( XSDSchema * schema ) {
	if( getProperty( "fractionDigits", schema ).isEmpty() )
		return -1;
	else
		return getProperty( "fractionDigits", schema ).toInt();
}

QString XSDRestriction::minInclusive( XSDSchema * schema ) {
	return getProperty( "minInclusive", schema );
}

QString XSDRestriction::minExclusive( XSDSchema * schema ) {
	return getProperty( "minExclusive", schema );
}

QString XSDRestriction::maxInclusive( XSDSchema * schema ) {
	return getProperty( "maxInclusive", schema );
}

QString XSDRestriction::maxExclusive( XSDSchema * schema ) {
	return getProperty( "maxExclusive", schema );
}

QValidator * XSDRestriction::validator() {
	if( (m_baseType == "integer")            || (m_baseType == "positiveInteger")    || (m_baseType == "negativeInteger") || 
		(m_baseType == "nonNegativeInteger") || (m_baseType == "nonPositiveInteger") || (m_baseType == "int") ||
		(m_baseType == "long")               || (m_baseType == "short")              || (m_baseType == "byte") || 
		(m_baseType == "unsignedInt")        ||
		(m_baseType == "unsignedLong")       || (m_baseType == "unsignedShort")      || (m_baseType == "unsignedByte") ) {
			
		QIntValidator * validator = new QIntValidator( qApp );
		QString min = minInclusive().isEmpty() ? minExclusive() : minInclusive(), 
				max = maxInclusive().isEmpty() ? maxExclusive() : maxInclusive();
		
		if( (m_baseType.left( 8 ) == "positive") || (m_baseType.left( 11 ) == "nonNegative") || (m_baseType.left(8) == "unsigned") )
			min = ( (!min.isEmpty()) && (min >= 0) ) ? min : "0";
		if( (m_baseType.left( 8 ) == "negative") || (m_baseType.left( 11 ) == "nonPositive") )
			max = ( (!max.isEmpty()) && (max <= 0) ) ? max : "0";
			
		if( ! min.isEmpty() )
			validator->setBottom( min.toInt() );
			
		if( ! max.isEmpty() )
			validator->setTop( max.toInt() );

		return validator;
	} else
	if( m_baseType == "boolean" ) {
		return new QBoolValidator( qApp );
	} else
	if( ( m_baseType == "decimal" ) || ( m_baseType == "float" ) || ( m_baseType == "double" ) ) {
		QDoubleValidator * validator = new QDoubleValidator( qApp );
		QString min = minInclusive().isEmpty() ? minExclusive() : minInclusive(), 
				max = maxInclusive().isEmpty() ? maxExclusive() : maxInclusive();
			
		if( ! min.isEmpty() )
			validator->setBottom( min.toDouble() );
			
		if( ! max.isEmpty() )
			validator->setTop( max.toDouble() );

		if( fractionDigits() != -1 )
			validator->setDecimals( fractionDigits() );		
			
		return validator;
	}
	return NULL;
}

/* XSDElement */

XSDElement::XSDElement( const QDomElement & element ) { 
	loadElement( element ); 
}

XSDElement::~XSDElement() {
	delete m_type;
}

/* XSDComplexContent */

XSDElementType::XSDElementType( const QDomElement & element ) : XSDAnnotedObject( element ) {
	loadElement( element );
}

XSDElementType::~XSDElementType() {
	qDeleteAll( m_types );
}

void XSDElementType::loadElement( const QDomElement & element ) {
	
}
