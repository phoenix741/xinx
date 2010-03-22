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

#ifndef __SNIPETDOCKWIDGET_H__
#define __SNIPETDOCKWIDGET_H__
#pragma once

// Xinx header
#include <snipets/snipetdockitemmodel.h>
#include <editors/abstracteditor.h>
#include <ui_snipetlistwidget.h>

// Qt header
#include <QDockWidget>
#include <QString>
#include <QToolBox>
#include <QHash>

// Dock
#include <dtoolview.h>

class RecursiveSortFilterProxyModel;
class SnipetDockItemModel;

/*!
 * This dock represent a Snipet Dock Widget to search and place snipet
 * in XINX.
 */
class SnipetDockWidget : public DToolView
{
	Q_OBJECT
public:
	SnipetDockWidget(QWidget * parent = 0);
	virtual ~SnipetDockWidget();

public slots:
	void setEditor(AbstractEditor * ed);
private slots:
	void createSnipet();
	void customizeSnipet();
	void filterChanged(const QString & filterText);
	void callSnipet(const QModelIndex & index);
private:
	void init();

	AbstractEditor * m_editor;
	RecursiveSortFilterProxyModel * m_snipetFilterModel;
	SnipetDockItemModel * m_snipetModel;
	Ui::SnipetsDockWidget * m_dock;
};

#endif /*__SNIPETDOCKWIDGET_H__*/
