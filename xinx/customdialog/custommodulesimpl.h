/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Modules Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Modules Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU Modules Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef CUSTOMMODULESIMPL_H
#define CUSTOMMODULESIMPL_H
#pragma once

// Xinx header
#include "ui_custommodules.h"
#include <plugins/plugininterfaces.h>

class CustomModulesImpl : public QWidget, public Ui::CustomModules, public IXinxPluginConfigurationPage {
	Q_OBJECT
public:
	CustomModulesImpl( QWidget * parent = 0 );
	~CustomModulesImpl();

	virtual QPixmap image();
	virtual QString name();

	virtual QWidget * settingsDialog();
	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual bool isSettingsValid();
	virtual bool isVisible();
private slots:
	void aboutPlugin( PluginElement * plugin );
	void aboutScript( PluginElement * plugin );
};

#endif /* CUSTOMMODULESIMPL_H */














