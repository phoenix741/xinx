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

#ifndef CUSTOMSNIPETIMPL_H
#define CUSTOMSNIPETIMPL_H
#pragma once

// Xinx header
#include "ui_customsnipet.h"
#include <plugins/plugininterfaces.h>

class SnipetItemModel;
class RecursiveSortFilterProxyModel;

class CustomSnipetImpl : public QWidget, public Ui::CustomSnipet, public IXinxPluginConfigurationPage
{
	Q_OBJECT
public:
	CustomSnipetImpl(QWidget * parent = 0);
	~CustomSnipetImpl();

	virtual QPixmap image();
	virtual QString name();

	virtual QWidget * settingsDialog();
	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual bool isSettingsValid();
	virtual bool isVisible();
private slots:
	void on_m_importPushButton_clicked();
	void on_m_exportPushButton_clicked();

	void m_addSnipetPushButton_clicked();
	void m_addCategoryPushButton_clicked();
	void on_m_removePushButton_clicked();
	void on_m_modifyPushButton_clicked();
	void on_m_duplicatePushButton_clicked();

	void m_snipetTreeView_selectionChanged();
	void on_m_snipetTreeView_doubleClicked(const QModelIndex & index);
	void on_m_snipetFilterLineEdit_textChanged(const QString & filterText);
private:
	int getCategory(const QModelIndex & index);

	RecursiveSortFilterProxyModel * m_snipetFilterModel;
	SnipetItemModel * m_snipetModel;
};

#endif /* CUSTOMSNIPETIMPL_H */














