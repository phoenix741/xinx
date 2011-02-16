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

// Xinx header
#include "categoryitemmodel.h"
#include "snipetmanager.h"

// Qt header
#include <QIcon>
#include <QColor>
#include <QFont>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlError>

/* CategoryItemModel */

/*!
 * \ingroup Snipets
 * \class CategoryItemModel
 * \since 0.9.0.0
 *
 * \brief This model show the categories' snipet tree.
 *
 * This model is used to show only the list of snipet's category. This can be used
 * to select a category in the list.
 * If you add or remove a category, the method select() must be called.
 *
 * \image html categoryitemmodel.png
 */

/*!
 * \enum CategoryItemModel::CategoryItemRole
 * \brief Available role in the for the method CategoryItemModel::data()
 */

/*!
 * \var CategoryItemModel::CategoryItemModel CategoryItemModel::CategoryIdRole
 * \brief Return the id role of the object QModelIndex
 */

/*!
 * \brief Create the category model.
 * \param db Make selection of snipet on this database
 * \param parent The parent category.
 */
CategoryItemModel::CategoryItemModel(QSqlDatabase db, QObject * parent) : TreeProxyItemModel(parent), m_db(db)
{
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel(this);
	setSourceModel(m_sourceModel);
}

//! Destroy the category model.
CategoryItemModel::~CategoryItemModel()
{
}

/*!
 * \brief Select all categories in the source model
 *
 * This method select all the categories from the source model
 * and re-create the internal structure of the TreeProxyItemModel by calling
 * TreeProxyItemModel::createMapping()
 */
void CategoryItemModel::select()
{
	// Set the query used all snipet
	m_sourceModel->setQuery(
		"SELECT id, parent_id, name "
		"FROM categories "
		"ORDER BY name", m_db
	);

	// Define name for header column
	m_sourceModel->setHeaderData(list_id, Qt::Horizontal, tr("Id"));
	m_sourceModel->setHeaderData(list_parentid, Qt::Horizontal, tr("Parent"));
	m_sourceModel->setHeaderData(list_name, Qt::Horizontal, tr("Name"));

	// Initialize the mapping
	createMapping();
}


int CategoryItemModel::proxyColumnToSource(int proxyColumn) const
{
	if (proxyColumn == 0)
		return list_name;
	return -1;
}

int CategoryItemModel::sourceColumnToProxy(int sourceColumn) const
{
	if (sourceColumn == list_name)
		return 0;
	return -1;
}

/*!
 * \brief Give the unique identifier for a given source index.
 *
 * This method give the ID of the snipet
 * \sa getTreeModelIdentifier(), getParentUniqueIdentifier()
 */
int CategoryItemModel::getUniqueIdentifier(const QModelIndex & sourceIndex) const
{
	QSqlRecord record = m_sourceModel->record(sourceIndex.row());
	return record.value(list_id).toInt();
}

/*!
 * \brief Give the unique identifier for a given source index.
 *
 * This method give the PARENT_ID of the snipet
 * \sa getTreeModelIdentifier(), getUniqueIdentifier()
 */
int CategoryItemModel::getParentUniqueIdentifier(const QModelIndex & sourceIndex) const
{
	QSqlRecord record = m_sourceModel->record(sourceIndex.row());
	return record.value(list_parentid).toInt();
}

QModelIndex CategoryItemModel::mapFromSource(const QModelIndex & sourceIndex) const
{
	QModelIndex index = mapFromSource(sourceIndex);
	int column = sourceColumnToProxy(index.column());
	if (column == -1) return QModelIndex();

	return TreeProxyItemModel::createIndex(index.row(), column, index.internalPointer());
}

QModelIndex CategoryItemModel::mapToSource(const QModelIndex & proxyIndex) const
{
	QModelIndex index = proxyIndex;
	int column = proxyColumnToSource(index.column());
	if (column == -1) return QModelIndex();

	return TreeProxyItemModel::mapToSource(createIndex(index.row(), column, index.internalPointer()));
}

int CategoryItemModel::columnCount(const QModelIndex & index) const
{
	Q_UNUSED(index);

	return 1;
}

Qt::ItemFlags CategoryItemModel::flags(const QModelIndex & index) const
{
	if (index.isValid())
	{
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return 0;
}

QVariant CategoryItemModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid()) return QVariant();

	if (role == Qt::DecorationRole)
	{
		if (index.column() == 0)
		{
			return QIcon(":/images/folder.png");
		}
	}
	else if (role == CategoryItemModel::CategoryIdRole)
	{
		QModelIndex sourceIndex = mapToSource(index);
		QSqlRecord record = m_sourceModel->record(sourceIndex.row());
		return record.value(list_id);
	}

	return TreeProxyItemModel::data(index, role);
}
