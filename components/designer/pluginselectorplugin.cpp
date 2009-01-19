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
#include "pluginselectorplugin.h"
#include "pluginselector.h"

// Qt header
#include <QtPlugin>

/* PluginSelectorPlugin */

PluginSelectorPlugin::PluginSelectorPlugin( QObject *parent ) : QObject( parent ) {
	m_initialized = false;
}

void PluginSelectorPlugin::initialize( QDesignerFormEditorInterface *core ) {
	if( m_initialized )
		return;

	m_initialized = true;
}

bool PluginSelectorPlugin::isInitialized() const {
	return m_initialized;
}

bool PluginSelectorPlugin::isContainer() const {
	return false;
}

QIcon PluginSelectorPlugin::icon() const {
	return QIcon();
}

QString PluginSelectorPlugin::toolTip() const {
	return "";
}

QString PluginSelectorPlugin::whatsThis() const {
	return "";
}

QString PluginSelectorPlugin::group() const {
	return "XINX Widget";
}

QString PluginSelectorPlugin::name() const {
	return "PluginSelector";
}

QString PluginSelectorPlugin::includeFile() const {
	return "pluginselector.h";
}

QString PluginSelectorPlugin::domXml() const {
	return  "<widget class=\"PluginSelector\" name=\"pluginSelector\"/>";
}

QWidget * PluginSelectorPlugin::createWidget( QWidget *parent ) {
	return new PluginSelector( parent );
}

