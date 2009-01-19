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

#ifndef __P_FILECONTENTDOCKWIDGET_H__
#define __P_FILECONTENTDOCKWIDGET_H__

// Xinx header
#include "../filecontentdockwidget.h"

// Qt header
#include <QTreeView>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

class PrivateFileContentDockWidget : public QObject {
	Q_OBJECT
public:
	PrivateFileContentDockWidget( FileContentDockWidget * parent );
	~PrivateFileContentDockWidget();
	
	QTreeView * m_contentTreeView;
	QAbstractItemModel * m_model;
	QSortFilterProxyModel * m_sortModel;
public slots:
	void contentTreeViewDblClick( QModelIndex index );
private:
	FileContentDockWidget * m_parent;
};

#endif // __P_FILECONTENTDOCKWIDGET_H__
