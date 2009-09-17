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

#ifndef SNIPETITEMMODEL_H
#define SNIPETITEMMODEL_H
#pragma once

// Qt header
#include <QString>
#include <QAbstractProxyModel>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QMap>
#include <QHash>
#include <QVector>

// Xinx header
#include "snipets/snipetlist.h"

/* SnipetItemModel */

class SnipetItemModel : public QAbstractProxyModel {
	Q_OBJECT
public:
	enum SnipetItemRole {
		SnipetIdRole = Qt::UserRole,
		SnipetParentIdRole = Qt::UserRole + 2,
		SnipetTypeRole = Qt::UserRole + 1
	};

	virtual ~SnipetItemModel();

	virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent( const QModelIndex & index ) const;

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
	virtual Qt::DropActions supportedDropActions() const;
	virtual bool dropMimeData( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );

	/*
	void loadSnipetList( const SnipetList & list );
	void addSnipet( const Snipet & snipet );

	SnipetList getSnipetList() const;
	*/

	/*!
	 * Import a list of snipet into the base.
	 */
	void importSnipetList( const SnipetList & list );
	/*!
	 * Remove snipets from the database where the indexes is indicate.
	 */
	void removeIndexes( const QModelIndexList & indexes, QWidget * parent = 0 );
	/*!
	 * Clear all data of the snipet (you must call \e select after)
	 * \sa select()
	 */
	void clear();
	void select( const QString & filter = QString() );

	virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
protected:
	friend class SnipetManager;
	SnipetItemModel( QSqlDatabase db, QObject * parent = 0 );

	enum {
		list_id          = 0,
		list_parentid    = 1,
		list_icon        = 2,
		list_name        = 3,
		list_description = 4,
		list_shortcut    = 5,
		list_type        = 6,
		list_availablejs = 7
	};

	QSqlQueryModel * sourceModel();
	QSqlQueryModel * sourceModel() const;

	QSqlDatabase database();
	QSqlDatabase database() const;
private:
	struct Mapping {
		bool is_category;
		int id, parrentId;
		int parentIndex;
		QVector<int> source_rows;
		QMap<int,Mapping*>::const_iterator map_iter;
	};
	typedef QMap<int,Mapping*> IndexMap;

	IndexMap m_sourcesIndexMapping;
	QHash<int,int> m_categoryIdMapping;
	QHash<int,int> m_snipetIdMapping;

	int proxyColumnToSource( int proxyColumn ) const;
	int sourceColumnToProxy( int sourceColumn ) const;
	void createMapping();

	QSqlDatabase m_db;
	QSqlQueryModel * m_sourceModel;
	QString m_filter;
};

#endif // SNIPETMODELINDEX_H
