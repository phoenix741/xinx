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

#include "snipet.h"

/* Snipet::Parameter */

bool Snipet::Parameter::operator==( const Snipet::Parameter & p ) const {
	return ( name == p.name )
	    && ( defaultValue == p.defaultValue );
}


/* Snipet */

const QString & Snipet::name() const {
	return m_name;
}

void Snipet::setName( const QString & name ) {
	m_name = name;
}

const QString & Snipet::key() const {
	return m_key;
}

void Snipet::setKey( const QString & key ) {
	m_key = key;
}

bool Snipet::callIsAutomatic() const {
	return m_callIsAutomatic;
}

void Snipet::setCallIsAutomatic( bool value ) {
	m_callIsAutomatic = value;
}

QString Snipet::type() const {
	return m_extentions.size() ? m_extentions.at( 0 ) : QString();
}

void Snipet::setType( const QString & type ) {
	if( m_extentions.size() ) {
		m_extentions[0] = type;
	} else {
		m_extentions << type;
	}
}

QStringList & Snipet::extentions() {
	return m_extentions;
}

const QStringList & Snipet::extentions() const {
	return m_extentions;
}

const QString & Snipet::description() const {
	return m_description;
}

void Snipet::setDescription( const QString & description ) {
	m_description = description;
}

const QString & Snipet::icon() const {
	return m_icon;
}

void Snipet::setIcon( const QString & icon ) {
	m_icon = icon;
}

QString Snipet::category() const {
	return m_categories.size() ? m_categories.at( 0 ) : QString();
}

void Snipet::setCategory( const QString & category ) {
	if( m_categories.size() ) {
		m_categories[0] = category;
	} else {
		m_categories.append( category );
	}
}
	
QStringList & Snipet::categories() {
	return m_categories;
}
	
const QStringList & Snipet::categories() const {
	return m_categories;
}
	
const QString & Snipet::text() const {
	return m_text;
}

void Snipet::setText( const QString & value ) {
	m_text = value;
}

QList<Snipet::Parameter> & Snipet::params() {
	return m_params;
}

const QList<Snipet::Parameter> & Snipet::params() const {
	return m_params;
}

const QString & Snipet::availableScript() const {
	return m_availableScript;
}

void Snipet::setAvailableScript( const QString & value ) {
	m_availableScript = value;
}

bool Snipet::operator==( const Snipet & s ) const {
	return ( m_name == s.m_name )
		&& ( m_key == s.m_key )
		&& ( m_categories == s.m_categories )
		&& ( m_text == s.m_text )
		&& ( m_description == s.m_description )
		&& ( m_icon == s.m_icon )
		&& ( m_extentions == s.m_extentions )
		&& ( m_params == s.m_params );
}

bool Snipet::operator<( const Snipet & s ) const {
	if( m_name < s.m_name )         return true;
	if( m_name > s.m_name )         return false;
	return false;
}
