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

#ifndef CONTENTVIEW2TREEMODEL_H
#define CONTENTVIEW2TREEMODEL_H

#pragma once

// Xinx header
#include <core/lib-config.h>
#include <utils/treeproxyitemmodel.h>
#include <contentview2/contentview2node.h>
#include <contentview2/contentview2file.h>

// Qt header
#include <QAbstractItemModel>

class QSqlQueryModel;

namespace ContentView2
{

class LIBEXPORT TreeModel : public TreeProxyItemModel
{
	Q_OBJECT
public:
	TreeModel(QSqlDatabase db, FileContainer container, QObject * parent = 0);
	virtual ~TreeModel();

	virtual QModelIndex index(const ContentView2::Node & node) const;
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;

	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
	virtual QModelIndex mapToSource(const QModelIndex & proxyIndex) const;

	QSqlQueryModel * sourceModel();
	QSqlQueryModel * sourceModel() const;
public slots:
	void select();

protected:
	virtual int getUniqueIdentifier(const QModelIndex & sourceIndex) const;
	virtual int getParentUniqueIdentifier(const QModelIndex & sourceIndex) const;

	enum
	{
		list_name         = 0,
		list_type         = 1,
		list_icon         = 2,
		list_display_name = 3,
		list_tips         = 4,
		list_line         = 5,
		list_id           = 6,
		list_parent_id    = 7
	};

	QSqlDatabase database();
	QSqlDatabase database() const;
private:
	int proxyColumnToSource(int proxyColumn) const;
	int sourceColumnToProxy(int sourceColumn) const;

	int m_rootId, m_fileId;
	FileContainer m_container;
	QSqlDatabase m_db;
	QSqlQueryModel * m_sourceModel;
};

} // namespace ContentView2

#endif // CONTENTVIEW2TREEMODEL_H
