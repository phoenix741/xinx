/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CUSTOMGENERALIMPL_H
#define CUSTOMGENERALIMPL_H
#pragma once

// Xinx header
#include "ui_customgeneral.h"
#include <plugins/interfaces/gui.h>

class CustomGeneralImpl : public QWidget, public Ui::CustomGeneral, public IXinxPluginConfigurationPage
{
	Q_OBJECT
public:
	CustomGeneralImpl(QWidget * parent = 0);
	~CustomGeneralImpl();

	virtual QPixmap image();
	virtual QString name();

	virtual QWidget * settingsDialog();
	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual bool isSettingsValid(QString & message);
	virtual bool isVisible();
};

#endif /* CUSTOMGENERALIMPL_H */














