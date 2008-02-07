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

#include "snipet.h"

/* PrivateSnipet */

class PrivateSnipet {
public:
	PrivateSnipet( Snipet * parent );
	virtual ~PrivateSnipet();
	
	QString m_text, m_name, m_description, m_icon, m_category, m_key;
	QStringList m_params;
	enum Snipet::SnipetType m_type;
private:
	Snipet * m_parent;
	friend class Snipet;
};

PrivateSnipet::PrivateSnipet( Snipet * parent ) : m_parent( parent ) {

}

PrivateSnipet::~PrivateSnipet() {
	
}

/* Snipet */

Snipet::Snipet() {
	d = new PrivateSnipet( this );
}

Snipet::Snipet( const Snipet & snipet ) : QObject( snipet.parent() ) {
	d = new PrivateSnipet( this );
	*d = *(snipet.d);
	d->m_parent = this;
}

	
Snipet::~Snipet() {
	delete d;
}
	
const QString & Snipet::name() const {
	return d->m_name;
}

void Snipet::setName( const QString & name ) {
	d->m_name = name;
	emit propertyChange();
}

const QString & Snipet::key() const {
	return d->m_key;
}
	 
void Snipet::setKey( const QString & key ) {
	d->m_key = key;
	emit propertyChange();
}

enum Snipet::SnipetType Snipet::type() const {
	return d->m_type;
}
	 
void Snipet::setType( enum Snipet::SnipetType type ) {
	d->m_type = type;
	emit propertyChange();
}
	
const QString & Snipet::description() const {
	return d->m_description;
}

void Snipet::setDescription( const QString & description ) {
	d->m_description = description;
	emit propertyChange();
}
	
const QString & Snipet::icon() const {
	return d->m_icon;
}

void Snipet::setIcon( const QString & icon ) {
	d->m_icon = icon;
	emit propertyChange();
}

const QString & Snipet::category() const {
	return d->m_category;
}
	
void Snipet::setCategory( const QString & category ) {
	d->m_category = category;
	emit categoryChange( category );
	emit propertyChange();
}

const QString & Snipet::text() const {
	return d->m_text;
}
	
void Snipet::setText( const QString & value ) {
	d->m_text = value;
	emit propertyChange();
}
	
QStringList & Snipet::params() {
	return d->m_params;
}
