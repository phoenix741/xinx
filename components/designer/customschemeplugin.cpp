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
#include "customschemeplugin.h"
#include "customschemeimpl.h"

// Qt header
#include <QtPlugin>

/* CustomSchemeWidgetPlugin */

CustomSchemeWidgetPlugin::CustomSchemeWidgetPlugin( QObject *parent ) : QObject( parent ) {
	m_initialized = false;
}

void CustomSchemeWidgetPlugin::initialize( QDesignerFormEditorInterface *core ) {
	if( m_initialized )
		return;

	m_initialized = true;
}

bool CustomSchemeWidgetPlugin::isInitialized() const {
	return m_initialized;
}

bool CustomSchemeWidgetPlugin::isContainer() const {
	return false;
}

QIcon CustomSchemeWidgetPlugin::icon() const {
	return QIcon();
}

QString CustomSchemeWidgetPlugin::toolTip() const {
	return "";
}

QString CustomSchemeWidgetPlugin::whatsThis() const {
	return "";
}

QString CustomSchemeWidgetPlugin::group() const {
	return "XINX Widget";
}

QString CustomSchemeWidgetPlugin::name() const {
	return "CustomSchemeImpl";
}

QString CustomSchemeWidgetPlugin::includeFile() const {
	return "customschemeimpl.h";
}

QString CustomSchemeWidgetPlugin::domXml() const {
	return  "<widget class=\"CustomSchemeImpl\" name=\"customScheme\"/>";
}

QWidget * CustomSchemeWidgetPlugin::createWidget( QWidget *parent ) {
	return new CustomSchemeImpl( parent );
}

