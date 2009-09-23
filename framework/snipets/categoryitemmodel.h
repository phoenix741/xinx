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

// Xinx header
#include "snipets/treeproxyitemmodel.h"

// Qt header
#include <QString>
#include <QSqlDatabase>
#include <QSqlQueryModel>

/* CategoryItemModel */

class CategoryItemModel : public TreeProxyItemModel {
	Q_OBJECT
public:
	enum CategoryItemRole {
		CategoryIdRole = Qt::UserRole
	};

	virtual ~CategoryItemModel();

	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

	void select();

	virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
protected:
	friend class SnipetManager;
	CategoryItemModel( QSqlDatabase db, QObject * parent = 0 );

	virtual int getUniqueIdentifier( const QModelIndex & sourceIndex ) const;
	virtual int getParentUniqueIdentifier( const QModelIndex & sourceIndex ) const;
private:
	enum {
		list_id          = 0,
		list_parentid    = 1,
		list_name        = 2
	};

	int proxyColumnToSource( int proxyColumn ) const;
	int sourceColumnToProxy( int sourceColumn ) const;

	QSqlDatabase m_db;
	QSqlQueryModel * m_sourceModel;
};

#endif // CATEGORYMODELINDEX_H
