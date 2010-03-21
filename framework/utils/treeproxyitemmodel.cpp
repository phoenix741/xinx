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
#include "core/exceptions.h"
#include "treeproxyitemmodel.h"

// Qt header
#include <QVariant>
#include <QDebug>

/* TreeProxyItemModel */

/*!
 * \class TreeProxyItemModel
 * \brief Class to transform a table model to a tree model
 *
 * This class is used to transform a table model (with only row and column) to a tree model. To do this
 * the subclass must redefine getParentUniqueIdentifier() and getUniqueIdentifier() to give for a QModelIndex
 * in the source model.
 * This proxy read all the content of the table and generate a tree. If there is a large amount of data or if the
 * medium of the source model is slow, another approach must be study.
 *
 * This model have been made to used with a source model connected to a little Sqlite base.
 *
 * In debug mode the internal structure is printed with qDebug() when error is triggered by ExceptionManager.
 */

/*!
 * \fn virtual int TreeProxyItemModel::getUniqueIdentifier( const QModelIndex & sourceIndex ) const = 0
 * \brief Method to reimplement to \e TreeProxyItemModel know the identifer for a source line
 *
 * This method must be reimplemented to give a unique identifer for a given line in the source model.
 * The column in the index must be ignore.
 */

/*!
 * \fn virtual int TreeProxyItemModel::getParentUniqueIdentifier( const QModelIndex & sourceIndex ) const = 0
 * \brief Method to reimplement to \e TreeProxyItemModel know the parent identifer for a source line
 *
 * This method must be reimplemented to give a unique identifer for the parent of a given line in the source model.
 * The column in the index must be ignore.
 */

/*!
 * \brief Create the TreeProxyItemModel
 *
 * Create a TreeProxyItemModel class.
 * By default the model is not reseted before createMapping fill the internal structure. But if you want speed up
 * the createMapping method, you can switch setResetModel to true.
 * If the model is resseted, all persistent index is lost.
 */
TreeProxyItemModel::TreeProxyItemModel(QObject * parent) : QAbstractProxyModel(parent), m_sourceColumnCount(0), m_resetModel(false)
{
#ifndef _XINX_RELEASE_MODE_
//  connect( ExceptionManager::self(), SIGNAL(errorTriggered()), this, SLOT(printMapping()) );
#endif
}

/// Destroy the TreeProxyItemModel class
TreeProxyItemModel::~TreeProxyItemModel()
{
	qDeleteAll(m_idMapping);
}

/*!
 * \brief Set the value of the source model (See QAbstractProxyModel)
 *
 * Set the source model to \e sourceModel and create the mapping for this model, if the sourceModel is not empty.
 * If \e sourceModel is null, QAbstractProxModel use a empty model.
 */
void TreeProxyItemModel::setSourceModel(QAbstractItemModel * sourceModel)
{
	QAbstractProxyModel::setSourceModel(sourceModel);
	createMapping();
}

/*!
 * \brief Return the value of the resetModel property
 *
 * Return true if the model is reseted when the model is filling
 */
bool TreeProxyItemModel::resetModel() const
{
	return m_resetModel;
}

/*!
 * \brief Set the value of resetModel property
 *
 * Set it to true, if you want increase performance and don't care of persistent index.
 * Else if you set it to false, only inserted and deleted row are updated.
 */
void TreeProxyItemModel::setResetModel(bool value)
{
	m_resetModel = value;
}

/*!
 * \internal
 * Get the Mapping structure for the id \e id.
 * If the id isn't in the HashTable, the stucture is automatically created and initialised.
 */
TreeProxyItemModel::Mapping * TreeProxyItemModel::getMapping(int id) const
{
	Mapping * mapping = m_idMapping.value(id, 0);
	if (! mapping)
	{
		mapping = new Mapping;
		mapping->id = id;
		mapping->parentId = -1;
		m_idMapping.insert(id, mapping);
	}
	return mapping;
}

/*!
 * \internal
 * Seatch the mapping for a given index. The index is relative to the tree model.
 */
TreeProxyItemModel::Mapping * TreeProxyItemModel::getMapping(const QModelIndex & index) const
{
	if (! index.isValid()) return getMapping(0);

	Mapping * parentMapping = static_cast<Mapping*>(index.internalPointer());
	Q_ASSERT(parentMapping);

	int id = parentMapping->childs.at(index.row());
	Mapping * childMapping = getMapping(id);
	Q_ASSERT(childMapping);

	return childMapping;
}

/*!
 * \internal
 * This method is used to know if the element is attached to
 * the root element (root element has id 0).
 * This must be done because, QSortFilterProxyModel don't work
 * if a row is added and childs are attached to the row.
 */
bool TreeProxyItemModel::isAttachedToRoot(Mapping * mapping)
{
	if (mapping->parentId == -1) return false;
	if (mapping->parentId == 0) return true;

	Mapping * parentMapping = m_idMapping.value(mapping->parentId);
	return isAttachedToRoot(parentMapping);
}

/*!
 * \internal
 * Unassign a mapping objet from it's parrent. As consequence, the object
 * will be removed from the view. The object can be re-assigned or deleted
 * after unassigning.
 */
void TreeProxyItemModel::unassignId(Mapping * mapping)
{
	Q_ASSERT(mapping);
	if (mapping->parentId < 0) return /* Already unassigned */;

	int oldParentId = mapping->parentId;
	if (m_idMapping.value(mapping->parentId, 0) > 0)
	{
		// Test obligatoire pour eviter le cas o� le parent a d�j� �t� d�ssasign�.
		Mapping * parentMapping = getMapping(oldParentId);
		int row = parentMapping->childs.indexOf(mapping->id);
		Q_ASSERT(row >= 0);

		beginRemoveRows(index(oldParentId), row, row);

		parentMapping->childs.remove(row);
	}

	mapping->parentId = -1;

	if (m_idMapping.value(oldParentId, 0) > 0)
		endRemoveRows();

}

/*!
 * \internal
 * Assign an object to a parentId. If the parentId is already attached to
 * root, this object will be added to the view by the method validAssigment().
 * Else this object will be queue, and assigned with it's parent.
 */
void TreeProxyItemModel::assignId(Mapping * mapping, int parentId)
{
	Q_ASSERT(mapping);
	Q_ASSERT(parentId >= 0);

	Mapping * parentMapping = getMapping(parentId);
	parentMapping->new_childs.append(mapping->id);
	mapping->parentId = parentId;

	//if( ( mapping->parentId >= 0 ) || ( mapping->id == 0 ) ) {
	//}
	if (isAttachedToRoot(mapping))
		validAssignement(parentMapping);
}

/*!
 * \internal
 * This method valid an assignement for an id attached (directly or
 * indirectly) to the root tree. Assignement for all child of the object
 * are valided too.
 */
void TreeProxyItemModel::validAssignement(Mapping * mapping)
{
	if (mapping->new_childs.size() > 0)
	{
		int row = mapping->childs.size();
		beginInsertRows(index(mapping->id), row, row + mapping->new_childs.size() - 1);
		mapping->childs << mapping->new_childs;
		mapping->new_childs.clear();
		endInsertRows();
	}

	foreach(int childId, mapping->childs)
	{
		Q_ASSERT(m_idMapping.value(childId));
		Mapping * child = getMapping(childId);
		validAssignement(child);
	}
}

/*!
 * \brief Change the parent of an identifier
 *
 * Change the parent identifier \e paren_id for a given identifier \e id.
 * Change the parent to -1 is equivalent to unassign an id. Change the
 * parent to an unassigned id, is equivalent to assign an id.
 */
void TreeProxyItemModel::setParentId(int id, int parentId)
{
	Mapping * mapping = getMapping(id);
	Q_ASSERT(mapping);

	if (parentId != mapping->parentId)
	{
		unassignId(mapping);
		if (parentId != -1)
		{
			assignId(mapping, parentId);
		}
	}
}

/*!
 * \brief Create the internal structure.
 *
 * Create the mapping of all index in the source model to the generate a tree.
 * In normal case, this method should not be call.
 */
void TreeProxyItemModel::createMapping()
{
	if (m_resetModel || (m_sourceColumnCount != sourceModel()->columnCount()))
	{
		// The model completly change
		qDeleteAll(m_idMapping);
		m_idMapping.clear();
		m_sourceColumnCount = sourceModel()->columnCount();
		m_id2IndexMapping.clear();
		m_index2IdMapping.clear();

		reset();
	}

	QList<int> currentIds = m_idMapping.keys();
	QList< QPair<int,int> > idsList;
	currentIds.removeAll(0);

	m_id2IndexMapping.clear();
	m_index2IdMapping.clear();

	int source_rows = sourceModel()->rowCount();

	for (int i = 0; i < source_rows; ++i)
	{
		QModelIndex index = sourceModel()->index(i, 0);
		int id            = getUniqueIdentifier(index);
		int parentId      = getParentUniqueIdentifier(index);

		m_id2IndexMapping[ id ] = i;
		m_index2IdMapping[ i  ] = id;

		idsList.append(qMakePair(id, parentId));
		currentIds.removeAll(id);
	}

	foreach(int id, currentIds)
	{
		setParentId(id, -1);
		IndexMap::iterator it = m_idMapping.find(id);
		delete it.value();
		m_idMapping.erase(it);
	}

	for (int i = 0; i < source_rows; ++i)
	{
		int id            = idsList.at(i).first;
		int parentId      = idsList.at(i).second;

		setParentId(id, parentId);
	}
}

#ifndef _XINX_RELEASE_MODE_
/*!
 * \internal
 * Print all the tree generated internally to the log file. This tree, is generated when
 * error occure.
 * In release mode this method is not necessary (and not usable).
 * \param id The root id where to start the tree (0 by default)
 * \param niveau The niveau where we are (0 by default). Used in recursivity.
 */
void TreeProxyItemModel::printMapping(int id, int niveau) const
{
	Mapping * m = getMapping(id);
	qDebug() << "Object: " << metaObject()->className() << ", Niveau : " << niveau << ", Id : " << m->id << ", Parent : " << m->parentId << ", childs : " << m->childs.size() << ", string : " << index(id).data().toString();

	foreach(int child, m->childs)
	{
		printMapping(child, niveau + 1);
	}
}
#endif /*_XINX_RELEASE_MODE_*/

/// For the given source index, this method return the corresponding index in the proxy (See QAbstractProxyModel)
QModelIndex TreeProxyItemModel::mapFromSource(const QModelIndex & sourceIndex) const
{
	if (! sourceIndex.isValid()) return QModelIndex();
	if (sourceIndex.model() != sourceModel())
	{
		qWarning("TreeProxyItemModel: index from wrong model passed to mapFromSource");
		return QModelIndex();
	}

	int sourceRow = sourceIndex.row();
	int id        = m_index2IdMapping.value(sourceRow, -1);
	Q_ASSERT(id >= 0);

	if (id == 0) return QModelIndex();

	Mapping * mapping = getMapping(id);
	int parentId = mapping->parentId;

	Mapping * parentMapping = getMapping(parentId);
	Q_ASSERT(parentMapping);

	int     proxyRow = parentMapping->childs.indexOf(id),
	                   proxyColumn = sourceIndex.column();

	return createIndex(proxyRow, proxyColumn, parentMapping);
}

/// For the given proxy index, this method return the corresponding source index (See QAbstractProxyModel)
QModelIndex TreeProxyItemModel::mapToSource(const QModelIndex & proxyIndex) const
{
	if (proxyIndex.model() != this)
	{
		qWarning("TreeProxyItemModel: index from wrong model passed to mapToSource");
		return QModelIndex();
	}

	Mapping * mapping;

	if (! proxyIndex.isValid())
	{
		int sourceRow = m_id2IndexMapping.value(0, -1);
		if (sourceRow >= 0)
			return sourceModel()->index(sourceRow, 0);
		else
			return QModelIndex();
	}

	mapping = static_cast<Mapping*>(proxyIndex.internalPointer());

	int sourceColumn = proxyIndex.column();
	if (sourceColumn == -1) return QModelIndex();

	int sourceId  = mapping->childs.at(proxyIndex.row());
	int sourceRow = m_id2IndexMapping.value(sourceId, -1);
	if (sourceRow == -1)    // Source Row deleted
	{
		return QModelIndex();
	}

	return sourceModel()->index(sourceRow, sourceColumn);
}

/// Return the \e QModelIndex for a given identifier \e id.
QModelIndex TreeProxyItemModel::index(int id) const
{
	if (id == 0) return QModelIndex();

	Mapping * mapping = getMapping(id);
	int parentId = mapping->parentId;

	Mapping * parentMapping = getMapping(parentId);
	int row = parentMapping->childs.indexOf(id);

	return createIndex(row, 0, parentMapping);
}

/// See QAbstractItemModel
QModelIndex TreeProxyItemModel::index(int row, int column, const QModelIndex & parent) const
{
	if ((row < 0) || (column < 0)) return QModelIndex();

	Mapping * parentMapping = getMapping(parent);
	Q_ASSERT(parentMapping);

	if (row >= parentMapping->childs.count()) return QModelIndex();
	if (column >= sourceModel()->columnCount()) return QModelIndex();

	return createIndex(row, column, parentMapping);
}

/// See QAbstractItemModel
QModelIndex TreeProxyItemModel::parent(const QModelIndex & index) const
{
	if (! index.isValid()) return QModelIndex();

	Mapping * parentMapping = static_cast<Mapping*>(index.internalPointer());

	int parentId = parentMapping->id;
	if (parentId == 0) return QModelIndex();

	int grandParentId = parentMapping->parentId;
	Mapping * grandParentMapping = getMapping(grandParentId);

	int parentRow = grandParentMapping->childs.indexOf(parentId);
	int parentCol = 0;

	return createIndex(parentRow, parentCol, grandParentMapping);
}

/// See QAbstractItemModel
int TreeProxyItemModel::rowCount(const QModelIndex & index) const
{
	if (index.column() > 0) return 0;

	Mapping * m = getMapping(index);
	int count   = m->childs.count();

	return count;
}

/// See QAbstractItemModel
int TreeProxyItemModel::columnCount(const QModelIndex & index) const
{
	return sourceModel()->columnCount(mapToSource(index));
}

/// See QAbstractItemModel
QVariant TreeProxyItemModel::data(const QModelIndex &proxyIndex, int role) const
{
	Mapping * m = getMapping(proxyIndex);
	if (-1 == m_id2IndexMapping.value(m->id, -1))
	{
		if (role == Qt::DisplayRole)
		{
			return tr("<deleted>");
		}
		else
		{
			return QVariant();
		}
	}
	return QAbstractProxyModel::data(proxyIndex, role);
}

/// See QAbstractItemModel
Qt::ItemFlags TreeProxyItemModel::flags(const QModelIndex &index) const
{
	Mapping * m = getMapping(index);
	int sourceRow = m_id2IndexMapping.value(m->id, -1);
	if (sourceRow == -1) return 0;
	return QAbstractProxyModel::flags(index);
}

