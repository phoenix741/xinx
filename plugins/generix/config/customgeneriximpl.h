/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#pragma once
#ifndef CUSTOMGENERIXIMPL_H
#define CUSTOMGENERIXIMPL_H

// Xinx header
#include "ui_customgenerix.h"
#include <plugins/interfaces/gui.h>

class CustomGenerixImpl : public QWidget, public Ui::CustomGenerix, public IXinxPluginConfigurationPage
{
	Q_OBJECT
public:
	CustomGenerixImpl(QWidget * parent = 0);
	~CustomGenerixImpl();

	virtual QPixmap image();
	virtual QString name();

	virtual QWidget * settingsDialog();
	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual bool isSettingsValid();
	virtual bool isVisible();
private:
};

#endif /* CUSTOMGENERIXIMPL_H */














