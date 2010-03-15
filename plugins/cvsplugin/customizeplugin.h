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

#ifndef CUSTOMIZEPLUGIN_H_
#define CUSTOMIZEPLUGIN_H_
#pragma once

// Xinx header
#include "ui_customize.h"
#include "plugins/plugininterfaces.h"
#include "pluginsettings.h"

class CustomizePlugin : public QWidget, public Ui::CustomizePlugin, public IXinxPluginConfigurationPage
{
	Q_OBJECT
public:
	CustomizePlugin(PluginSettings * settings, QWidget * parent = 0);
	virtual ~CustomizePlugin();

	virtual QPixmap image();
	virtual QString name();

	virtual QWidget * settingsDialog();
	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual bool isSettingsValid();
	virtual bool isVisible();

private:
	PluginSettings * m_settings;
};

#endif /*CUSTOMIZEPLUGIN_H_*/
