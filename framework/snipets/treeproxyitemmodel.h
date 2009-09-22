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

#ifndef TREEPROXYITEMMODEL_H
#define TREEPROXYITEMMODEL_H
#pragma once

// Qt header
#include <QString>
#include <QAbstractProxyModel>
#include <QHash>
#include <QVector>

/* TreeProxyItemModel */

class TreeProxyItemModel : public QAbstractProxyModel {
	Q_OBJECT
public:
	TreeProxyItemModel( QObject * parent = 0 );
	virtual ~TreeProxyItemModel();

	virtual QModelIndex index( int id ) const;
	virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent( const QModelIndex & index ) const;

	virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const;
	virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const;

	virtual QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
	virtual QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
protected:
	virtual int getUniqueIdentifier( const QModelIndex & sourceIndex ) const = 0;
	virtual int getParentUniqueIdentifier( const QModelIndex & sourceIndex ) const = 0;

	virtual void setSourceModel( QAbstractItemModel * sourceModel );
	virtual void createMapping();
private:
	struct Mapping {
		int id; //!< Define the id of the line. This id have nothing to do with the line in the source model
		int parentId; //! Define the parent id of this structure
		QVector<int> childs; //!< Define a list of id, child of this element
	};
	/*!
	 * Map of id/structure.
	 * This map associate id with a stucture contains id, parentId, and rows.
	 */
	typedef QHash<int,Mapping*> IndexMap;

	/// Map of id/structure
	mutable IndexMap m_idMapping;
	QHash<int,int> m_id2IndexMapping;
	QHash<int,int> m_index2IdMapping;

	Mapping * getMapping( int id ) const;
	Mapping * getMapping( const QModelIndex & index ) const;
};

#endif // TREEPROXYITEMMODEL_H
