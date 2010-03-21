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

// Xinx header
#include "contentview2treemodel.h"
#include "contentview2/contentview2cache.h"
#include "contentview2/contentview2manager.h"
#include "project/xinxproject.h"

// Qt header
#include <QIcon>
#include <QColor>
#include <QFont>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include <QMimeData>
#include <QSqlQueryModel>

namespace ContentView2
{

TreeModel::TreeModel(QSqlDatabase db, FileContainer container, QObject * parent) : TreeProxyItemModel(parent), m_rootId(-1), m_fileId(-1), m_container(container), m_db(db)
{
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel(this);
	setSourceModel(m_sourceModel);
}

TreeModel::~TreeModel()
{
}

int TreeModel::proxyColumnToSource(int proxyColumn) const
{
	switch (proxyColumn)
	{
	case 0 :
		return list_display_name;
	default:
		return -1;
	}
}

int TreeModel::sourceColumnToProxy(int sourceColumn) const
{
	switch (sourceColumn)
	{
	case list_display_name :
		return 0;
	default:
		return -1;
	}
}

/// For the given source index, this method return the corresponding index in the proxy
QModelIndex TreeModel::mapFromSource(const QModelIndex & sourceIndex) const
{
	QModelIndex index = mapFromSource(sourceIndex);
	int column = sourceColumnToProxy(index.column());
	if (column == -1) return QModelIndex();

	return TreeProxyItemModel::createIndex(index.row(), column, index.internalPointer());
}

QModelIndex TreeModel::mapToSource(const QModelIndex & proxyIndex) const
{
	QModelIndex index = proxyIndex;
	int column = proxyColumnToSource(index.column());
	if (column == -1) return QModelIndex();

	return TreeProxyItemModel::mapToSource(createIndex(index.row(), column, index.internalPointer()));
}

int TreeModel::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return 1;
}

QModelIndex TreeModel::index(const ContentView2::Node & node) const
{
	if (! node.isValid()) return QModelIndex();

	return TreeProxyItemModel::index(node.nodeId());
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex & parent) const
{
	return TreeProxyItemModel::index(row, column, parent);
}

QVariant TreeModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid()) return QVariant();

	if (role == ContentView2::Node::NODE_ID)
	{
		QModelIndex sourceIndex = mapToSource(index);
		if (! sourceIndex.isValid()) return QVariant();

		QSqlRecord record = m_sourceModel->record(sourceIndex.row());
		return record.value(list_id);
	}
	if (role == ContentView2::Node::NODE_TYPE)
	{
		QModelIndex sourceIndex = mapToSource(index);
		if (! sourceIndex.isValid()) return QVariant();

		QSqlRecord record = m_sourceModel->record(sourceIndex.row());
		return record.value(list_type);
	}
	else if (role == Qt::DecorationRole)
	{
		if (index.column() == 0)
		{
			QModelIndex sourceIndex = mapToSource(index);
			QSqlRecord record = sourceModel()->record(sourceIndex.row());
			return QIcon(record.value(list_icon).toString());
		}
	}
	else if (role == Qt::ToolTipRole)
	{
		if (index.column() == 0)
		{
			QModelIndex sourceIndex = mapToSource(index);
			QSqlRecord record = sourceModel()->record(sourceIndex.row());
			return record.value(list_tips).toString();
		}
	}
	else if (role >= ContentView2::Node::NODE_USER_VALUE)
	{
		QModelIndex sourceIndex = mapToSource(index);
		if (! sourceIndex.isValid()) return QVariant();

		QSqlRecord record = m_sourceModel->record(sourceIndex.row());
		ContentView2::Node node(database(), record.value(list_id).toUInt());
		return node.data(role);
	}

	return TreeProxyItemModel::data(index, role);
}

QSqlQueryModel * TreeModel::sourceModel()
{
	return m_sourceModel;
}

QSqlQueryModel * TreeModel::sourceModel() const
{
	return m_sourceModel;
}

void TreeModel::select()
{
	if (m_container.isValid(m_db))
	{
		m_container.reload(m_db);
		QSqlQuery query(m_db);

		// Set the query used all snipet
		query.prepare(
		    "SELECT cv_node.name, cv_node.type, cv_node.icon, cv_node.display_name, "
		    "cv_node.tips, cv_node.line, "
		    "case when cv_node.id=:root_id1 then 0 else cv_node.id end as id, "
		    "case when cv_link.parent_id=:root_id2 then 0 when cv_node.id=:root_id3 then -1 else cv_link.parent_id end as parent_id "
		    "FROM cv_node left join cv_link on cv_link.child_id=cv_node.id "
		    "WHERE cv_node.file_id=:file_id "
		    "ORDER BY lower(cv_node.display_name)"
		);

		query.bindValue(":root_id1", m_container.file(m_db).rootId());
		query.bindValue(":root_id2", m_container.file(m_db).rootId());
		query.bindValue(":root_id3", m_container.file(m_db).rootId());
		query.bindValue(":file_id",  m_container.file(m_db).fileId());
		bool result = query.exec();
		Q_ASSERT_X(result, "TreeModel::select", qPrintable(query.lastError().text()));
		m_sourceModel->setQuery(query);

		while (m_sourceModel->canFetchMore())
			m_sourceModel->fetchMore();

		m_sourceModel->setHeaderData(list_display_name, Qt::Horizontal, tr("Name"));

		// Initialize the mapping
		createMapping();
	}
}

int TreeModel::getUniqueIdentifier(const QModelIndex & sourceIndex) const
{
	QSqlRecord record = m_sourceModel->record(sourceIndex.row());
	int id = record.value(list_id).toInt();
	return id;
}

int TreeModel::getParentUniqueIdentifier(const QModelIndex & sourceIndex) const
{
	QSqlRecord record = m_sourceModel->record(sourceIndex.row());
	int id = record.value(list_parent_id).toInt();
	return id;
}

QSqlDatabase TreeModel::database()
{
	return m_db;
}

QSqlDatabase TreeModel::database() const
{
	return m_db;
}


} // namespace ContentView2
