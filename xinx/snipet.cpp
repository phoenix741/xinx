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

#include "snipet.h"

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

const QString & Snipet::type() const {
	return m_type;
}

void Snipet::setType( const QString & type ) {
	m_type = type;
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

const QString & Snipet::category() const {
	return m_category;
}

void Snipet::setCategory( const QString & category ) {
	m_category = category;
}

const QString & Snipet::text() const {
	return m_text;
}

void Snipet::setText( const QString & value ) {
	m_text = value;
}

QStringList & Snipet::params() {
	return m_params;
}

const QStringList & Snipet::params() const {
	return m_params;
}

bool Snipet::operator==( const Snipet & s ) const {
	return ( m_name == s.m_name )
		&& ( m_key == s.m_key )
		&& ( m_category == s.m_category )
		&& ( m_text == s.m_text )
		&& ( m_description == s.m_description )
		&& ( m_icon == s.m_icon )
		&& ( m_type == s.m_type )
		&& ( m_params == s.m_params );
}

bool Snipet::operator<( const Snipet & s ) const {
	return ( m_category < s.m_category ) && ( m_name < s.m_name );
}
