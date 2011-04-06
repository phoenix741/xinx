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

#ifndef CUSTOMDIALOGIMPL_P_H
#define CUSTOMDIALOGIMPL_P_H
#pragma once

#include <QDialog>
#include <QMultiHash>

namespace Ui {
	class CustomDialog;
}

class IXinxPluginConfigurationPage;
class PluginElement;

//! \private
class CustomDialogImplPrivate : public QObject
{
	Q_OBJECT
public:
    CustomDialogImplPrivate(QObject* parent = 0);
    virtual ~CustomDialogImplPrivate();

	QMultiHash<PluginElement*,IXinxPluginConfigurationPage*> _pluginsPages;
	QList<IXinxPluginConfigurationPage*> _pages;
	QScopedPointer<Ui::CustomDialog> _ui;
public slots:
	void updateOkButton();
};

#endif // CUSTOMDIALOGIMPL_P_H
