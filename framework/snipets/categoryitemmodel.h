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

#ifndef CATEGORYITEMMODEL_H
#define CATEGORYITEMMODEL_H
#pragma once

// Qt header
#include <QString>
#include <QAbstractProxyModel>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QMap>
#include <QHash>
#include <QVector>

/* CategoryItemModel */

class CategoryItemModel : public QAbstractProxyModel {
	Q_OBJECT
public:
	enum CategoryItemRole {
		CategoryIdRole = Qt::UserRole
	};

	virtual ~CategoryItemModel();

	QModelIndex index( int categoryId );
	virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent( const QModelIndex & index ) const;

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

	/*!
	 * Clear all data of the snipet (you must call \e select after)
	 * \sa select()
	 */
	void clear();
	void select();

	virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
protected:
	friend class SnipetDatabaseManager;
	CategoryItemModel( QSqlDatabase db, QObject * parent = 0 );

	enum {
		list_id          = 0,
		list_parentid    = 1,
		list_name        = 2,
		list_order       = 3
	};

	QSqlQueryModel * sourceModel();
	QSqlQueryModel * sourceModel() const;

	QSqlDatabase database();
	QSqlDatabase database() const;
private:
	struct Mapping {
		int id, parrentId; // Has debug information only.
		int parentIndex;
		QVector<int> source_rows;
		QMap<int,Mapping*>::const_iterator map_iter;
	};
	typedef QMap<int,Mapping*> IndexMap;

	IndexMap m_sourcesIndexMapping;
	QHash<int,int> m_categoryIdMapping;

	int proxyColumnToSource( int proxyColumn ) const;
	int sourceColumnToProxy( int sourceColumn ) const;
	void createMapping();

	QSqlDatabase m_db;
	QSqlQueryModel * m_sourceModel;
};

#endif // CATEGORYMODELINDEX_H
