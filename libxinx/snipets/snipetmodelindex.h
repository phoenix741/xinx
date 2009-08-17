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
#include <QSqlQuery>

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

	void select();
private:
	SnipetItemModel( QSqlDatabase db, QObject * parent = 0 );
	friend class SnipetDatabaseManager;

	mutable QHash<QString,indexInternalPointer*> m_internalPointers;
	struct indexInternalPointer * getInternalPointer( QString type, unsigned long id ) const;

	enum {
		list_id = 0,
		list_name = 1,
		list_category = 2,

		parent_id = 0,

		snipet_id = 0,
		snipet_icon = 1,
		snipet_name = 2,
		snipet_shortcut = 3,
		snipet_description = 4,
		snipet_category = 5,

		category_id = 0,
		category_name = 1,
		category_description = 2,
		category_parent = 3
	};

	QSqlDatabase m_db;
	mutable QSqlQuery m_listQuery, m_parentQuery, m_snipetQuery, m_categoryQuery;
};

#endif // SNIPETMODELINDEX_H
