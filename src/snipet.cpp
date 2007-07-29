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
	
	QString m_text;
	QStringList m_params;
private:
	Snipet * m_parent;
};

PrivateSnipet::PrivateSnipet( Snipet * parent ) : m_parent( parent ) {

}

PrivateSnipet::~PrivateSnipet() {
	
}

/* Snipet */

Snipet::Snipet() {
	d = new PrivateSnipet( this );
}
	
Snipet::~Snipet() {
	delete d;
}
	
const QString & Snipet::text() const {
	return d->m_text;
}
	
void Snipet::setText( const QString & value ) {
	d->m_text = value;
}
	
QStringList & Snipet::params() {
	return d->m_params;
}
