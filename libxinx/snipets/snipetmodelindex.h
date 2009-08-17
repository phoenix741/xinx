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
#include <QAbstractProxyModel>
#include <QSqlDatabase>
#include <QSqlQueryModel>

/* SnipetItemModel */

class SnipetItemModel : public QAbstractProxyModel {
	Q_OBJECT
public:
	virtual ~SnipetItemModel();

	virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent( const QModelIndex & index ) const;

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

	/*
	void loadSnipetList( const SnipetList & list );
	void clear();
	void addSnipet( const Snipet & snipet );
	void removeSnipet( const QModelIndexList & indexes );

	SnipetList getSnipetList() const;
	*/

	virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
private:
	SnipetItemModel( QSqlDatabase db, QObject * parent = 0 );
	friend class SnipetDatabaseManager;
	enum {
		list_id          = 0,
		list_parentid    = 1,
		list_icon        = 2,
		list_name        = 3,
		list_description = 4,
		list_shortcut    = 5,
		list_order       = 6,
		list_type        = 7
	};

	QSqlDatabase m_db;
	QSqlQueryModel * m_sourceModel;
};

#endif // SNIPETMODELINDEX_H
