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
private:
	QHash<QModelIndex,int> m_count;

	FlatModel * m_parent;
};

PrivateFlatModel::PrivateFlatModel( FlatModel * parent ) {
	m_parent = parent;
}

int PrivateFlatModel::rowCount( QModelIndex index ) {
	if( m_count.count( index ) ) return m_count[ index ];	// Utiliser une table de hashage ?	

	int count = m_model->rowCount( index ), size = count;
	for( int i = 0; i < size ; i ++ ) {
		count += rowCount( m_model->index( i, index.column(), index ) );
	}

	if( ! m_count.count( index ) ) m_count[ index ] = count;	// Utiliser une table de hashage ?	
	return count;
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
	if( row == 0 ) return m_model->index( row, 0, index );
	int i = 0, rowDel = 0, size = m_model->rowCount( index );
	do {
		int cnt = rowCount( m_model->index( i, 0, index ) );
		if( ( row - rowDel ) < cnt )
			return flatToModel( m_model->index( i, 0, index ), row - rowDel );
		else
			rowDel += cnt + 1;
	} while( i < size );
	return QModelIndex();
}

QModelIndex PrivateFlatModel::flatToModel( QModelIndex index ) {
	QModelIndex newIndex = flatToModel( QModelIndex(), index.row() );
	if( ! newIndex.isValid() )
		return QModelIndex();
	else
		return m_model->index( newIndex.row(), index.column(), index.parent() );
}


PrivateFlatModel::~PrivateFlatModel() {
	
}

FlatModel::FlatModel( QAbstractItemModel * model ) {
	d = new PrivateFlatModel( this );
	d->m_model = model;
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



