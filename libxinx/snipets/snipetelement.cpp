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

#include "snipetelement.h"

/* SnipetElement */

SnipetElement::SnipetElement() {
	m_script = "function isEnabled() {\n\treturn true;\n}\n";
}

const QString & SnipetElement::name() const {
	return m_name;
}

void SnipetElement::setName( const QString & value ) {
	m_name = value;
}

const QString & SnipetElement::description() const {
	return m_description;
}

void SnipetElement::setDescription( const QString & value ) {
	m_description = value;
}

const QString & SnipetElement::key() const {
	return m_key;
}

void SnipetElement::setKey( const QString & value ) {
	m_key = value;
}

const QString & SnipetElement::icon() const {
	return m_icon;
}

void SnipetElement::setIcon( const QString & value ) {
	m_icon = value;
}

bool SnipetElement::isAutomatiqueCall() const {
	return m_isAutomatiqueCall;
}

void SnipetElement::setAutomatiqueCall( bool value ) {
	m_isAutomatiqueCall = value;
}

const QStringList & SnipetElement::extentionsList() const {
	return m_extentionsList;
}

void SnipetElement::setExtentionsList( const QStringList & value ) {
	m_extentionsList = value;
}

const QString & SnipetElement::text() const {
	return m_text;
}

void SnipetElement::setText( const QString & value ) {
	m_text = value;
}

const QString & SnipetElement::script() const {
	return m_script;
}

void SnipetElement::setScript( const QString & value ) {
	m_script = value;
}

QList< QPair<QString,QString> > & SnipetElement::params() {
	return m_params;
}

const QList< QPair<QString,QString> > & SnipetElement::params() const {
	return m_params;
}

QDomElement SnipetElement::saveToDom( QDomDocument & document ) const {
	QDomElement s = document.createElement( "Snipet" );

	s.setAttribute( "name", m_name );
	s.setAttribute( "key", m_key );
	s.setAttribute( "icon", m_icon );
	s.setAttribute( "automatique", m_isAutomatiqueCall );

	QDomElement description = document.createElement( "Description" );
	s.appendChild( description );
	QDomText text = document.createTextNode( m_description );
	description.appendChild( text );

	/*
	QDomElement category = document.createElement( "Category" );
	s.appendChild( category );
	QDomText text = document.createTextNode( snipet.category() );
	description.appendChild( text );
	*/

	QDomElement textElement = document.createElement( "Text" );
	s.appendChild( textElement );
	text = document.createTextNode( snipet.text() );
	textElement.appendChild( text );

	QPair<QString,QString> params;
	foreach( params, params() ) {
		QDomElement param = document.createElement( "Param" );
		s.appendChild( param );
		param.setAttribute( "name", params.first );
		param.setAttribute( "value", params.second );
	}

	foreach( const QString ext, m_extentionsList ) {
		QDomElement extentionElement = document.createElement( "Extention" );
		s.appendChild( extentionElement );
		extentionElement.setAttribute( "value", ext );
	}
}

void SnipetElement::loadFromDom( const QDomElement & element ) {
	setName( element.attribute( "name" ) );
	setKey( element.attribute( "key" ) );
	setIcon( element.attribute( "icon" ) );

	QDomElement description = snipet.firstChildElement( "Description" );
	newSnipet.setDescription( description.text() );

	QDomElement textElement = snipet.firstChildElement( "Text" );
	newSnipet.setText( textElement.text() );

	QDomElement param = snipet.firstChildElement( "Param" );
	while( ! param.isNull() ) {
		newSnipet.params().append( param.attribute( "name" ) );
		param = param.nextSiblingElement( "Param" );
	}

	setType( snipet.attribute( "type" ) );
	setCategory( snipet.attribute( "category" ) );
}


bool SnipetElement::operator==( const SnipetElement & s ) const {
	return ( m_key == s.m_key );
}

bool SnipetElement::operator<( const SnipetElement & s ) const {
	/*	if( m_category < s.m_category ) return true;
	if( m_category > s.m_category ) return false;*/
	if( m_name < s.m_name )         return true;
	if( m_name > s.m_name )         return false;
	return false;
}
