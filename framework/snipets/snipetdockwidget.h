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

#ifndef __SNIPETDOCKWIDGET_H__
#define __SNIPETDOCKWIDGET_H__
#pragma once

// Xinx header
#include <snipets/snipetdockitemmodel.h>
#include <QWidget>

class QDockWidget;
class AbstractEditor;
class RecursiveSortFilterProxyModel;
class SnipetDockItemModel;
class SnipetDockWidgetPrivate;

/*!
 * This dock represent a Snipet Dock Widget to search and place snipet
 * in XINX.
 */
class SnipetDockWidget : public QWidget
{
	Q_OBJECT
public:
	SnipetDockWidget(QWidget * parent = 0);
	virtual ~SnipetDockWidget();

public slots:
	void setEditor(AbstractEditor * ed);

private:
	QScopedPointer<SnipetDockWidgetPrivate> d;
};

#endif /*__SNIPETDOCKWIDGET_H__*/
