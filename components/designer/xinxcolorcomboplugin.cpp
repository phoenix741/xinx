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
#include "xinxcolorcomboplugin.h"
#include "xinxcolorcombo.h"

// Qt header
#include <QtPlugin>

/* XinxColorComboPlugin */

XinxColorComboPlugin::XinxColorComboPlugin( QObject *parent ) : QObject( parent ) {
	m_initialized = false;
}

void XinxColorComboPlugin::initialize( QDesignerFormEditorInterface *core ) {
	if( m_initialized )
		return;

	m_initialized = true;
}

bool XinxColorComboPlugin::isInitialized() const {
	return m_initialized;
}

bool XinxColorComboPlugin::isContainer() const {
	return false;
}

QIcon XinxColorComboPlugin::icon() const {
	return QIcon();
}

QString XinxColorComboPlugin::toolTip() const {
	return "";
}

QString XinxColorComboPlugin::whatsThis() const {
	return "";
}

QString XinxColorComboPlugin::group() const {
	return "XINX Widget";
}

QString XinxColorComboPlugin::name() const {
	return "XinxColorCombo";
}

QString XinxColorComboPlugin::includeFile() const {
	return "xinxcolorcombo.h";
}

QString XinxColorComboPlugin::domXml() const {
	return  "<widget class=\"XinxColorCombo\" name=\"xinxColorCombo\"/>";
}

QWidget * XinxColorComboPlugin::createWidget( QWidget *parent ) {
	return new XinxColorCombo( parent );
}

