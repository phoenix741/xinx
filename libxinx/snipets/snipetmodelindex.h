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

#ifndef SNIPETMODELINDEX_H
#define SNIPETMODELINDEX_H
#pragma once

// Qt header
#include <QString>
#include <QAbstractItemModel>
#include <QHash>
#include <QApplication>
#include <QSqlDatabase>

/* SnipetItemModel */

class SnipetItemModel : public QAbstractItemModel {
	Q_OBJECT
public:
	virtual ~SnipetItemModel();

	virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent( const QModelIndex & index ) const;

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

	/*
	void loadSnipetList( const SnipetList & list );
	void clear();
	void addSnipet( const Snipet & snipet );
	void removeSnipet( const QModelIndexList & indexes );

	SnipetList getSnipetList() const;
	*/
private:
	struct indexInternalPointer {
		bool is_category;
		unsigned long parent_id;
	};

	SnipetItemModel( QSqlDatabase db, QObject * parent = 0 );
	friend class SnipetDatabaseManager;

	QSqlDatabase m_db;
	QList<indexInternalPointer> m_internalId;
	QSqlQuery m_query;
};

#endif // SNIPETMODELINDEX_H
