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
#include "snipetitemmodel.h"
#include "snipetmanager.h"

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

/* SnipetItemModel */

SnipetItemModel::SnipetItemModel(QSqlDatabase db, QObject * parent) : BaseSnipetItemModel(db, parent)
{

}

SnipetItemModel::~SnipetItemModel()
{

}

int SnipetItemModel::proxyColumnToSource(int proxyColumn) const
{
	switch (proxyColumn)
	{
	case 0 :
		return list_name;
	case 1 :
		return list_shortcut;
	case 2 :
		return list_description;
	default:
		return -1;
	}
}

int SnipetItemModel::sourceColumnToProxy(int sourceColumn) const
{
	switch (sourceColumn)
	{
	case list_name :
		return 0;
	case list_shortcut :
		return 1;
	case list_description :
		return 2;
	default:
		return -1;
	}
}

/// For the given source index, this method return the corresponding index in the proxy
QModelIndex SnipetItemModel::mapFromSource(const QModelIndex & sourceIndex) const
{
	QModelIndex index = mapFromSource(sourceIndex);
	int column = sourceColumnToProxy(index.column());
	if (column == -1) return QModelIndex();

	return BaseSnipetItemModel::createIndex(index.row(), column, index.internalPointer());
}

QModelIndex SnipetItemModel::mapToSource(const QModelIndex & proxyIndex) const
{
	QModelIndex index = proxyIndex;
	int column = proxyColumnToSource(index.column());
	if (column == -1) return QModelIndex();

	return BaseSnipetItemModel::mapToSource(createIndex(index.row(), column, index.internalPointer()));
}

int SnipetItemModel::columnCount(const QModelIndex & index) const
{
	Q_UNUSED(index);

	return 3;
}

Qt::ItemFlags SnipetItemModel::flags(const QModelIndex & index) const
{
	if (index.isValid())
	{
		QModelIndex sourceIndex = mapToSource(index);
		QSqlRecord record = sourceModel()->record(sourceIndex.row());
		if (record.value(list_type).toString() == "CATEGORY")
		{
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
		}
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
	}
	return 0;
}

QStringList SnipetItemModel::mimeTypes() const
{
	QStringList types = BaseSnipetItemModel::mimeTypes();
	types << "application/snipet.id.list";
	return types;
}

Qt::DropActions SnipetItemModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QMimeData * SnipetItemModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData * mimeData = BaseSnipetItemModel::mimeData(indexes);
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach(QModelIndex index, indexes)
	{
		if (index.isValid())
		{
			QString type = data(index, SnipetTypeRole).toString();
			QString name = data(index, Qt::DisplayRole).toString();
			int id = data(index, SnipetIdRole).toInt();
			stream << id << type << name;
		}
	}

	mimeData->setData("application/snipet.id.list", encodedData);
	return mimeData;
}

bool SnipetItemModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	Q_UNUSED(row);
	Q_UNUSED(column);

	if ((action == Qt::MoveAction) && data->hasFormat("application/snipet.id.list"))
	{
		int parentId = parent.data(SnipetIdRole).toInt();

		QByteArray itemData = data->data("application/snipet.id.list");
		QDataStream stream(&itemData, QIODevice::ReadOnly);

		while (! stream.atEnd())
		{
			int id;
			QString type, name;
			stream >> id >> type >> name;

			QSqlQuery updateQuery(database());
			if (type == "CATEGORY")
			{
				updateQuery.prepare("UPDATE categories SET parent_id=:new_parent WHERE id=:id");
			}
			else
			{
				updateQuery.prepare("UPDATE snipets SET category_id=:new_parent WHERE id=:id");
			}
			updateQuery.bindValue(":new_parent", parentId);
			updateQuery.bindValue(":id", id);

			bool result = updateQuery.exec();
			Q_ASSERT(result);

			int parentIdentifier = getTreeModelIdentifier("CATEGORY", parentId);
			int identifier = getTreeModelIdentifier(type, id);

			setParentId(identifier, parentIdentifier);
		}

		return true;
	}
	return false;
}

QVariant SnipetItemModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid()) return QVariant();


	if (role == Qt::DecorationRole)
	{
		if (index.column() == 0)
		{
			QModelIndex sourceIndex = mapToSource(index);
			QSqlRecord record = sourceModel()->record(sourceIndex.row());
			return QIcon(record.value(list_icon).toString());
		}
	}
	else if (role == Qt::BackgroundRole)
	{
		QModelIndex sourceIndex = mapToSource(index);
		if (! sourceIndex.isValid()) return QVariant();

		QSqlRecord record = sourceModel()->record(sourceIndex.row());
		if (record.value(list_type).toString() == "CATEGORY")
		{
			return QColor(0xFF, 0xFF, 0xCC);
		}
	}
	else if (role == Qt::FontRole)
	{
		QModelIndex sourceIndex = mapToSource(index);
		if (! sourceIndex.isValid()) return QVariant();

		QSqlRecord record = sourceModel()->record(sourceIndex.row());
		if (record.value(list_type).toString() == "CATEGORY")
		{
			QFont currentFont;
			currentFont.setBold(true);
			return currentFont;
		}
	}
	else if (role == Qt::StatusTipRole)
	{
		QModelIndex sourceIndex = mapToSource(index);
		if (! sourceIndex.isValid()) return QVariant();

		QSqlRecord record = sourceModel()->record(sourceIndex.row());
		return record.value(list_description);
	}

	return BaseSnipetItemModel::data(index, role);
}
