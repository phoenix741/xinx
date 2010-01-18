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

#ifndef BASESNIPETITEMMODEL_H
#define BASESNIPETITEMMODEL_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <utils/treeproxyitemmodel.h>
#include <snipets/snipetlist.h>

// Qt header
#include <QString>
#include <QSqlDatabase>
#include <QSqlQueryModel>

/* BaseSnipetItemModel */

class LIBEXPORT BaseSnipetItemModel : public TreeProxyItemModel {
	Q_OBJECT
public:
	enum SnipetItemRole {
		SnipetIdRole = Qt::UserRole,
		SnipetParentIdRole = Qt::UserRole + 2,
		SnipetTypeRole = Qt::UserRole + 1
	};

	virtual ~BaseSnipetItemModel();

	virtual QModelIndex index( bool isCategory, int id ) const;
	virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

	void indexesToIds( const QModelIndexList & indexes, QList<int> & snipetIds, QList<int> & categoryIds ) const;
	QList<int> indexesToIds( const QModelIndexList & indexes ) const;

	QSqlQueryModel * sourceModel();
	QSqlQueryModel * sourceModel() const;
public slots:
	void select();

protected:
	friend class SnipetManager;
	BaseSnipetItemModel( QSqlDatabase db, QObject * parent = 0 );

	int getTreeModelIdentifier( QString type, int id ) const;
	virtual int getUniqueIdentifier( const QModelIndex & sourceIndex ) const;
	virtual int getParentUniqueIdentifier( const QModelIndex & sourceIndex ) const;

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

	QSqlDatabase database();
	QSqlDatabase database() const;
private:
	void addIndexToList( QModelIndex index, QList<int> * ids ) const;

	QSqlDatabase m_db;
	QSqlQueryModel * m_sourceModel;
};

#endif // BASESNIPETMODELINDEX_H
