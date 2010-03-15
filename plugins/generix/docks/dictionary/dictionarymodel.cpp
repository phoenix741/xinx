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
#include "dictionarymodel.h"
#include <contentview2/contentview2manager.h>

// Qt header
#include <QPixmap>

#define SQL_LIMIT 255

/* DictionaryModel */

DictionaryModel::DictionaryModel(QString uuid, QObject * parent) : QAbstractItemModel(parent), m_uuid( uuid )
{
}

DictionaryModel::~DictionaryModel()
{
}

int DictionaryModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

QVariant DictionaryModel::data(const QModelIndex & item, int role) const
{
	if (!m_query.isValid())
		return QVariant();
	if (!item.isValid())
		return QVariant();
	if (item.column() >= 1)
		return false;

	bool result = m_query.seek(item.row());
	Q_ASSERT_X (result, "DictionaryModel::data", qPrintable(m_query.lastError().text()));

	if (Qt::DecorationRole == role)
	{
		QString pixmap = m_query.value(1).toString();
		return QPixmap(pixmap);
	}
	else if (Qt::DisplayRole == role)
	{
		QString name = m_query.value(0).toString();
		return name;
	}
	return QVariant();
}

QModelIndex DictionaryModel::index(int row, int column, const QModelIndex & parent) const
{
	if( parent.isValid() ) return QModelIndex();

	return createIndex( row, column );
}

QModelIndex DictionaryModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

int DictionaryModel::rowCount(const QModelIndex & parent) const
{
	return parent.isValid() ? 0 : m_size;
}

void DictionaryModel::select( QSqlDatabase db, const QString & filter, const QString & order ) {
	QSqlQuery rootIdQuery("SELECT root_id FROM cv_file WHERE path=:path", db);
	rootIdQuery.bindValue(":path", m_uuid);
	bool result = rootIdQuery.exec();
	Q_ASSERT_X(result, "TreeModel::select", qPrintable(rootIdQuery.lastError().text()));
	Q_ASSERT_X(rootIdQuery.first(), "TreeModel::select", "Can't find the root ID");
	uint rootId = rootIdQuery.value(0).toUInt();

	QString filterWhereClause =
			" AND (lower(name) like '%'||lower(:filter1)||'%' "
			"  OR EXISTS (SELECT 1 FROM cv_node child, cv_link child_link WHERE "
			"             child.id = child_link.child_id AND child_link.parent_id=cv_node.id "
			"             AND lower(child.name) like '%'||lower(:filter2)||'%'))";

	QString queryStr =
			"SELECT name, icon "
			"FROM cv_node, cv_link "
			"WHERE cv_link.parent_id=:parent "
			"  AND cv_link.child_id=cv_node.id "
			"  AND cv_node.path=:path " + (filter.isEmpty() ? "" : filterWhereClause) + " "
			"ORDER BY " + (order.isEmpty() ? "cv_node.name ASC" : order) + " "
			"LIMIT " + QString("%1").arg(SQL_LIMIT);

	QSqlQuery query( queryStr, db );
	query.bindValue( ":parent", rootId );
	query.bindValue( ":path", m_uuid );
	if (!filter.isEmpty())
	{
		query.bindValue(":filter1", filter);
		query.bindValue(":filter2", filter);
	}
	result = query.exec();
	Q_ASSERT_X( result, "DictionaryModel::select", qPrintable( query.lastError().text() ) );

	bool mustClearModel = m_size >= 0;
	if (mustClearModel) {
		beginRemoveRows(QModelIndex(), 0, qMax(m_size, 0));
		m_size = -1;
	}

	m_query.clear();

	if (mustClearModel)
		endRemoveRows();

	m_query = query;
	int i = 0;
	while (m_query.next())
		++i;

	beginInsertRows(QModelIndex(), 0, i - 1);
	m_size = i ;
	endInsertRows();

}
