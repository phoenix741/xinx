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
#include "kcolorcomboplugin.h"
#include "kcolorcombo.h"

// Qt header
#include <QtPlugin>

/* KColorComboPlugin */

KColorComboPlugin::KColorComboPlugin( QObject *parent ) : QObject( parent ) {
	m_initialized = false;
}

void KColorComboPlugin::initialize( QDesignerFormEditorInterface *core ) {
	if( m_initialized )
		return;

	m_initialized = true;
}

bool KColorComboPlugin::isInitialized() const {
	return m_initialized;
}

bool KColorComboPlugin::isContainer() const {
	return false;
}

QIcon KColorComboPlugin::icon() const {
	return QIcon();
}

QString KColorComboPlugin::toolTip() const {
	return "";
}

QString KColorComboPlugin::whatsThis() const {
	return "";
}

QString KColorComboPlugin::group() const {
	return "XINX Widget";
}

QString KColorComboPlugin::name() const {
	return "KColorCombo";
}

QString KColorComboPlugin::includeFile() const {
	return "kcolorcombo.h";
}

QString KColorComboPlugin::domXml() const {
	return  "<widget class=\"KColorCombo\" name=\"kColorCombo\"/>";
}

QWidget * KColorComboPlugin::createWidget( QWidget *parent ) {
	return new KColorCombo( parent );
}

