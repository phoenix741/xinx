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

#ifndef __SNIPETDOCKWIDGET_P_H__
#define __SNIPETDOCKWIDGET_P_H__
#pragma once

// Xinx header
#include <snipets/snipetdockitemmodel.h>
#include <ui_snipetlistwidget.h>

// Qt header
#include <QDockWidget>
#include <QString>
#include <QToolBox>
#include <QHash>

class SnipetDockWidget;
class AbstractEditor;
class RecursiveSortFilterProxyModel;

class SnipetDockWidgetPrivate : public QObject
{
	Q_OBJECT
public:
	SnipetDockWidgetPrivate(SnipetDockWidget * parent = 0);
	virtual ~SnipetDockWidgetPrivate();

	void init();

	AbstractEditor * m_editor;
	RecursiveSortFilterProxyModel * m_snipetFilterModel;
	QScopedPointer<Ui::SnipetsDockWidget> _ui;
	SnipetDockItemModel * m_snipetModel;
	QDockWidget * _dock;
	SnipetDockWidget * _widget;
public slots:
	void createSnipet();
	void customizeSnipet();
	void filterChanged(const QString & filterText);
	void callSnipet(const QModelIndex & index);
};

#endif /*__SNIPETDOCKWIDGET_P_H__*/
