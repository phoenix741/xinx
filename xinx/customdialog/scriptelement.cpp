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
#include "scriptelement.h"

/* ScriptElement */

ScriptElement::ScriptElement( const ScriptValue & script ) : m_script( script ) {
}

bool ScriptElement::isModifiable() const {
	return false;
}

bool ScriptElement::isConfigurable() const {
	return false;
}

QPixmap ScriptElement::pixmap() const {
	return QPixmap( ":/images/ecmascript.png" );
}

QString ScriptElement::name() const {
	return m_script.text().replace( "&", "" );
}

QString ScriptElement::author() const {
	return m_script.value().property( "author" ).toString();
}

QString ScriptElement::version() const {
	return m_script.value().property( "version" ).toString();
}

QString ScriptElement::licence() const {
	return m_script.value().property( "licence" ).toString();
}

QString ScriptElement::description() const {
	return tr("Author : ") + m_script.value().property( "author" ).toString();
}

ScriptValue & ScriptElement::script() {
	return m_script;
}
