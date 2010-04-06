/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

/*!
 * \ingroup ContentView2
 * \class TreeModel
 * \since 0.9.0.0
 *
 * \brief This class is used to show the content of the cache for a given file.
 *
 * The TreeModel is used to show the content view. This object shouldn't be used if
 * the file containt too much node.
 *
 * \image html abstracteditor1.png
 */

 /*!
  * \var TreeModel::list_name
  * \brief Return the name of the node (for used with sourceModel())
  */
 /*!
  * \var TreeModel::list_type
  * \brief Return the type of the node (for used with sourceModel())
  */
 /*!
  * \var TreeModel::list_icon
  * \brief Return the icon link of the node (for used with sourceModel())
  */
 /*!
  * \var TreeModel::list_display_name
  * \brief Return the displayed name of the node (for used with sourceModel())
  */
 /*!
  * \var TreeModel::list_tips
  * \brief Return the tips of the node (for used with sourceModel())
  */
 /*!
  * \var TreeModel::list_line
  * \brief Return the line of the node (for used with sourceModel())
  */
 /*!
  * \var TreeModel::list_id
  * \brief Return the id of the node (for used with sourceModel())
  */
 /*!
  * \var TreeModel::list_parent_id
  * \brief Return the parent id of the node (for used with sourceModel())
  */

/*!
 * \brief The class construct a TreeModel.
 *
 * To create an instance, this class need a database, used to select the content of the file.
 * A FileContainer is needed because, at the creation of the instance, the file in the database
 * can not be created.
 * \param db The database used to select the content.
 * \param container The file to show
 * \param parent The parent of the object.
 */
TreeModel::TreeModel(QSqlDatabase db, FileContainer container, QObject * parent) : TreeProxyItemModel(parent), m_rootId(-1), m_fileId(-1), m_container(container), m_db(db)
{
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel(this);
	setSourceModel(m_sourceModel);
}

//! Destroy this object
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

/*!
 * \brief This method return the QModelIndex, for a given \p node.
 *
 * This method used TreeProxyItemModel::index(int) with the id in paremeter
 * to find the QModelIndex.
 */
QModelIndex TreeModel::index(const ContentView2::Node & node) const
{
	if (! node.isValid()) return QModelIndex();

	return TreeProxyItemModel::index(node.nodeId());
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex & parent) const
{
	return TreeProxyItemModel::index(row, column, parent);
}

/*!
 * \brief Return the data for proxyIndex for the role \p role.
 *
 * This method return the data for the given role. This role can be
 * one of Node::RoleIndex. This method create Node object to access
 * to necessary information.
 */
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

//! Source model (methode provide for convenience)
QSqlQueryModel * TreeModel::sourceModel()
{
	return m_sourceModel;
}

//! Source model (methode provide for convenience)
QSqlQueryModel * TreeModel::sourceModel() const
{
	return m_sourceModel;
}

/*!
 * \brief Select in the database() all nodes for the file.
 *
 * If the file is valide (the file must be created in the database), this method
 * read all node for the file id.
 *
 * By merging with cv_link, this method search the parent_id. Datas are sorted by display_name
 * (case insensitive).
 *
 * The tree is created by calling TreeProxyItemModel::createMapping()
 */
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

//! This method return the id of the ContentView2::Node
int TreeModel::getUniqueIdentifier(const QModelIndex & sourceIndex) const
{
	QSqlRecord record = m_sourceModel->record(sourceIndex.row());
	int id = record.value(list_id).toInt();
	return id;
}

/*!
 * \brief This method return the parent id of the ContentView2::Node
 *
 * The id of the parent node can be finded in the cv_link table.
 */
int TreeModel::getParentUniqueIdentifier(const QModelIndex & sourceIndex) const
{
	QSqlRecord record = m_sourceModel->record(sourceIndex.row());
	int id = record.value(list_parent_id).toInt();
	return id;
}

//! Database where the model is connected (methode provide for convenience)
QSqlDatabase TreeModel::database()
{
	return m_db;
}

//! Database where the model is connected (methode provide for convenience)
QSqlDatabase TreeModel::database() const
{
	return m_db;
}


} // namespace ContentView2
