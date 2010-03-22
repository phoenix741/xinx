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

#ifndef __FILECONTENTDOCKWIDGET_H__
#define __FILECONTENTDOCKWIDGET_H__
#pragma once

// Qt header
#include <QDockWidget>
#include <QString>
#include <QModelIndex>

// Dock
#include <dtoolview.h>

class QAbstractItemModel;
class QTreeView;

/*!
 * This dock contains the content of the current file editor in a tree.
 * This dock permit the user to brownse the content of the file, and go inside import.
 */
class FileContentDockWidget : public DToolView
{
	Q_OBJECT
public:
	FileContentDockWidget(QWidget * parent = 0);
	virtual ~FileContentDockWidget();
public slots:
	/*!
	 * Update the tree with this model. The model comes from the editor
	 * \param model Model contains the content of the editor. If NULL, the editor is deleted.
	 */
	void updateModel(QAbstractItemModel * model);

	/*!
	 * When the position in the editor change this slot can be used to tell what
	 * index must be selected.
	 * \param index the index in the ContentViewModel that must be selected.
	 */
	void positionChanged(const QModelIndex & index);
signals:
	/*!
	 * Signal emited when the user double-click on the editor. The goal is open the file
	 * if necessary, and change the line.
	 * \param name Name of the file to open
	 * \param line Line to change.
	 */
	void open(const QString & name, int line);

private slots:
	void contentTreeViewDblClick(QModelIndex index);
private:
	void init();

	QTreeView * m_contentTreeView;
	QAbstractItemModel * m_model;
};

#endif // __FILECONTENTDOCKWIDGET_H__
