/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "flattreeview.h"

#include <QHash>

class PrivateFlatModel {
public:
	PrivateFlatModel( FlatModel * parent );
	~PrivateFlatModel();
	
	QModelIndex flatToModel( QModelIndex index, int row );
	QModelIndex flatToModel( QModelIndex index );
	int rowCount( QModelIndex index = QModelIndex() );
	
	QAbstractItemModel * m_model;
	QPersistentModelIndex m_root;
private:
	QHash<QModelIndex,int> m_count;

	FlatModel * m_parent;
};

PrivateFlatModel::PrivateFlatModel( FlatModel * parent ) {
	m_parent = parent;
}

#include <assert.h>

int PrivateFlatModel::rowCount( QModelIndex index ) {
	assert( m_root.isValid() );
	QModelIndex idx = index.isValid() ? index : QModelIndex(m_root);
	
	if( m_count.count( idx ) ) return m_count[ idx ] + 1;	// Utiliser une table de hashage ?	

	int count = 0, size = m_model->rowCount( idx );
	for( int i = 0; i < size ; i ++ ) {
		count += rowCount( m_model->index( i, idx.column(), idx ) );
	}

	if( ! m_count.count( idx ) ) m_count[ idx ] = count;	// Utiliser une table de hashage ?	
	return count + 1;
}

/*
|- (0) bin
 |- (1) toto1
  |- (2) titi1
  |- (3) titi2
  |- (4) titi3
 |- (5) toto2
 |- (6) toto3
  |- (7) titi1
  |- (8) titi2
|- (9) usr
|- (10) var
|- (11) tmp
*/

QModelIndex PrivateFlatModel::flatToModel( QModelIndex index, int row ) {
	assert( m_root.isValid() );
	if( row == 0 ) return index;
	int i = 0, rowDel = 0, size = m_model->rowCount( index );
	do {
		int cnt = rowCount( m_model->index( i, 0, index ) );
		if( ( row - rowDel - 1 ) < cnt )
			return flatToModel( m_model->index( i, 0, index ), row - rowDel - 1 );
		else
			rowDel += cnt;
		i++;
	} while( ( i < size) && ( row - rowDel >= 0 ) );
	return QModelIndex();
}

QModelIndex PrivateFlatModel::flatToModel( QModelIndex index ) {
	if( index.isValid() ) {
		QModelIndex newIndex = flatToModel( m_root, index.row() );
		if( ! newIndex.isValid() )
			return m_root;
		else
			return m_model->index( newIndex.row(), index.column(), newIndex.parent() );
	} else {
		return m_root;
	}
}


PrivateFlatModel::~PrivateFlatModel() {
	
}

FlatModel::FlatModel( QAbstractItemModel * model, QModelIndex root ) {
	d = new PrivateFlatModel( this );
	d->m_model = model;
	d->m_root = QPersistentModelIndex( root );
}

FlatModel::~FlatModel() {
	delete d;
}
	
int FlatModel::columnCount ( const QModelIndex & parent ) const {
	QModelIndex converti = d->flatToModel( parent );
	return d->m_model->columnCount( converti );
}

QVariant FlatModel::data ( const QModelIndex & index, int role ) const {
	QModelIndex converti = d->flatToModel( index );
	return d->m_model->data( converti, role );	
}

Qt::ItemFlags FlatModel::flags ( const QModelIndex & index ) const {
	QModelIndex converti = d->flatToModel( index );
	return d->m_model->flags( converti );	
}

QModelIndex FlatModel::index ( int row, int column, const QModelIndex & parent ) const {
	if( ! parent.isValid() )
		return createIndex( row, column );
	else
		return QModelIndex();
}

QModelIndex FlatModel::parent ( const QModelIndex & index ) const {
	Q_UNUSED( index );
	return QModelIndex();
}

int FlatModel::rowCount ( const QModelIndex & parent ) const {
	if( parent.isValid() )
		return 0;
	else {
		return d->rowCount();
	}
}



