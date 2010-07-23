/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#ifndef CUSTOMCONTENTVIEWIMPL_H
#define CUSTOMCONTENTVIEWIMPL_H

// Xinx header
#include "ui_customcontentview.h"
#include <plugins/plugininterfaces.h>

class CustomContentViewImpl : public QWidget, public Ui::CustomContentView, public IXinxPluginConfigurationPage
{
	Q_OBJECT
public:
	CustomContentViewImpl(QWidget *parent = 0);
	virtual ~CustomContentViewImpl();

	virtual QPixmap image();
	virtual QString name();

	virtual QWidget * settingsDialog();
	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual bool isSettingsValid();
	virtual bool isVisible();

private:
	void updateInformations();
private slots:
	void on_m_reindexBaseBtn_clicked();
	void on_m_compactBaseBtn_clicked();
};

#endif // CUSTOMCONTENTVIEWIMPL_H
