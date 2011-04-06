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

#ifndef CUSTOMDIALOGIMPL_H
#define CUSTOMDIALOGIMPL_H
#pragma once

// Xinx header
#include "ui_customdialog.h"
#include <core/xinxconfig.h>

class CustomDialogImplPrivate;
class PluginElement;
class RecursiveSortFilterProxyModel;
class IXinxPluginConfigurationPage;

class CustomDialogImpl : public QDialog
{
	Q_OBJECT
public:
	CustomDialogImpl(QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint);
	~CustomDialogImpl();

	void loadConfig();
	void saveConfig();

	int execUniquePage(int page);
public slots:
	virtual void accept();
private:
	QScopedPointer<CustomDialogImplPrivate> d;
};

#endif
