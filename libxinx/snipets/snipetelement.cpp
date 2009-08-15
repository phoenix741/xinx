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

// Xinx header
#include "snipetelement.h"

// Qt header
#include <QtSql>

/* SnipetElement */

SnipetElement::SnipetElement( int id ) : m_isCached( false ), m_id( id ) {

}

QString SnipetElement::name() const {
	loadCache();

	return m_cachedData.name;
}

void SnipetElement::setName( const QString & value ) {
	if( m_cachedData.name != value ) {
		m_cachedData.name = value;
		saveCache();
	}
}

QString SnipetElement::description() const {
	loadCache();

	return m_cachedData.description;
}

void SnipetElement::setDescription( const QString & value ) {
	if( m_cachedData.description != value ) {
		m_cachedData.description = value;
		saveCache();
	}
}

QString SnipetElement::key() const {
	loadCache();

	return m_cachedData.key;
}

void SnipetElement::setKey( const QString & value ) {
	if( m_cachedData.key != value ) {
		m_cachedData.key = value;
		saveCache();
	}
}

QString SnipetElement::icon() const {
	loadCache();

	return m_cachedData.icon;
}

void SnipetElement::setIcon( const QString & value ) {
	if( m_cachedData.icon != value ) {
		m_cachedData.icon = value;
		saveCache();
	}
}

bool SnipetElement::isAutomatiqueCall() const {
	loadCache();

	return m_cachedData.automatic;
}

void SnipetElement::setAutomatiqueCall( bool value ) {
	if( m_cachedData.automatic != value ) {
		m_cachedData.automatic = value;
		saveCache();
	}
}

QStringList SnipetElement::extentionsList() const {
	return m_extentionsList;
}

void SnipetElement::setExtentionsList( const QStringList & value ) {
	m_extentionsList = value;
}

QString SnipetElement::text() const {
	loadCache();

	return m_cachedData.text;
}

void SnipetElement::setText( const QString & value ) {
	if( m_cachedData.text != value ) {
		m_cachedData.text = value;
		saveCache();
	}
}

QString SnipetElement::script() const {
	loadCache();

	return m_cachedData.script;
}

void SnipetElement::setScript( const QString & value ) {
	if( m_cachedData.availableScript != value ) {
		m_cachedData.availableScript = value;
		saveCache();
	}
}

QList< QPair<QString,QString> > & SnipetElement::params() {
	return m_params;
}

const QList< QPair<QString,QString> > & SnipetElement::params() const {
	return m_params;
}

void SnipetElement::loadCache() {
	if( ! m_isCached ) {
		QSqlDatabase db = QSqlDatabase::database( "SNIPET" );
		QSqlQuery query( "SELECT name, description, shortcut, icon, auto, text, available_script, order, category_id FROM snipet WHERE id=:id", db );
		query.bindValue( ":id", m_id );
		query.exec();

		if( query.next() ) {
			m_cachedData.name = query.value( 0 ).toString();
			m_cachedData.description = query.value( 1 ).toString();
			m_cachedData.shortcut =  = query.value( 2 ).toString();
			m_cachedData.icon = query.value( 3 ).toString();
			m_cachedData.automatic = query.value( 4 ).toInt() == 1 ? true : false;
			m_cachedData.text = query.value( 5 ).toString();
			m_cachedData.availableScript = query.value( 6 ).toString();
			m_cachedData.order = query.value( 7 ).toInt();
			m_cachedData.catogaryId = query.value( 8 ).toInt();

			m_isCached = true;
		} else
			qWarning( qPrintable( tr("The snipet id %1 isn't found in the database.").arg( m_id ) ) );
	}
}

void SnipetElement::saveCache() {
	QSqlDatabase db = QSqlDatabase::database( "SNIPET" );
	QSqlQuery query( "UPDATE snipet SET name=:name, description=:description, shortcut=:shortcut, icon=:icon, auto=:auto, text=:text, available_script=:available_script, order=:order, category_id=:catogary_id WHERE id=:id", db );
	query.bindValue( ":id", m_id );
	query.bindValue( ":name", m_cachedData.name );
	query.bindValue( ":description", m_cachedData.description );
	query.bindValue( ":shortcut", m_cachedData.shortcut );
	query.bindValue( ":icon", m_cachedData.icon );
	query.bindValue( ":auto", m_cachedData.automatic );
	query.bindValue( ":text", m_cachedData.text );
	query.bindValue( ":available_script", m_cachedData.availableScript );
	query.bindValue( ":order", m_cachedData.order );
	query.bindValue( ":category_id", m_cachedData.catogaryId );
	query.exec();
}

QDomElement SnipetElement::saveToDom( QDomDocument & document ) const {
	QDomElement s = document.createElement( "Snipet" );

	s.setAttribute( "name", m_name );
	s.setAttribute( "key", m_key );
	s.setAttribute( "icon", m_icon );
	s.setAttribute( "automatique", m_isAutomatiqueCall ? "true" : "false" );

	QDomElement description = document.createElement( "Description" );
	s.appendChild( description );
	QDomText text = document.createTextNode( m_description );
	description.appendChild( text );

	QDomElement currentNode = s;
	foreach( const QString & cat, m_categories ) {
		QDomElement category = document.createElement( "Category" );
		currentNode.appendChild( category );
		category.setAttribute( "name", cat );

		currentNode = category;
	}

	QDomElement textElement = document.createElement( "Text" );
	s.appendChild( textElement );
	text = document.createTextNode( m_text );
	textElement.appendChild( text );

	QPair<QString,QString> param;
	foreach( param, params() ) {
		QDomElement paramNode = document.createElement( "Param" );
		s.appendChild( paramNode );
		paramNode.setAttribute( "name", param.first );
		paramNode.setAttribute( "defaultValue", param.second );
	}

	foreach( const QString ext, m_extentionsList ) {
		QDomElement extentionElement = document.createElement( "Extention" );
		s.appendChild( extentionElement );
		extentionElement.setAttribute( "value", ext );
	}
	return s;
}

void SnipetElement::loadFromDom( const QDomElement & element ) {
	setName( element.attribute( "name" ) );
	setKey( element.attribute( "key" ) );
	setIcon( element.attribute( "icon" ) );
	QString cat = element.attribute( "category" );
	m_categories.clear();
	if( ! cat.isEmpty() ) {
		m_categories.append( cat );
	}
	QDomElement categoryNode = element.firstChildElement( "Category" );
	while( ! categoryNode.isNull() ) {
		m_categories.append( element.attribute( "name" ) );
		categoryNode = categoryNode.firstChildElement( "Category" );
	}
	setAutomatiqueCall( element.attribute( "automatique" ) == "true" );

	m_extentionsList.clear();
	QString ext = element.attribute( "type" );
	if( ! ext.isEmpty() ) {
		m_extentionsList.append( ext );
	}
	QDomElement extentionNode = element.firstChildElement( "Extention" );
	while( ! extentionNode.isNull() ) {
		m_extentionsList.append( extentionNode.attribute( "value" ) );
		extentionNode = extentionNode.nextSiblingElement( "Extention" );
	}

	QDomElement description = element.firstChildElement( "Description" );
	setDescription( description.text() );

	QDomElement textElement = element.firstChildElement( "Text" );
	setText( textElement.text() );

	QDomElement param = element.firstChildElement( "Param" );
	while( ! param.isNull() ) {
		m_params.append( qMakePair( param.attribute( "name" ), param.attribute( "defaultValue" ) ) );
		param = param.nextSiblingElement( "Param" );
	}
}

int SnipetElement::order() const {
	loadCache();

	return m_cachedData.order;
}

void SnipetElement::setOrder( int value ) {
	m_order = value;
}

bool SnipetElement::operator==( const SnipetElement & s ) const {
	return ( m_cachedData.key == s.m_cachedData.key );
}

bool SnipetElement::operator<( const SnipetElement & s ) const {
	if( m_order < s.m_order ) return true;
	if( m_order > s.m_order ) return false;
	if( m_name < s.m_name )   return true;
	if( m_name > s.m_name )   return false;
	return false;
}
