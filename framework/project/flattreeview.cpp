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

// Xinx header
#include "flattreeview.h"
#include <core/exceptions.h>

// Qt header
#include <QHash>
#include <QDirModel>

/* FlatModel */

FlatModel::FlatModel( QDirModel * model, QModelIndex root ) : QAbstractItemModel( model ), m_model( model ) {
	m_model = model;
	m_root = QPersistentModelIndex( root );
	recalcPathList();
	connect( m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)) );
	connect( m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)) );

	connect( m_model, SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()) );
	connect( m_model, SIGNAL(layoutChanged()), this, SLOT(recalcPathList()) );
	connect( m_model, SIGNAL(modelAboutToBeReset()), this, SIGNAL(modelAboutToBeReset()) );
	connect( m_model, SIGNAL(modelReset()), this, SLOT(recalcPathList()) );
}

FlatModel::~FlatModel() {

}

void FlatModel::recalcPathList() {
	m_pathList.clear();
	insertIntoPathList( m_root );
	reset();
}

void FlatModel::insertIntoPathList( QModelIndex index ) {
	if( index.isValid() && m_model->isDir( index ) ) {
		int size = m_model->rowCount( index ), position = m_pathList.count();

		bool hasFile = false;
		for( int i = 0 ; i < size ; i++ ) {
			bool isDir = m_model->isDir( m_model->index( i, 0, index ) );
			if( !isDir )
				m_pathList << m_model->filePath( m_model->index( i, 0, index ) );
			hasFile |= !isDir;
		}
		if( hasFile )
			m_pathList.insert( position, m_model->filePath( index ) );


		for( int i = 0 ; i < size ; i++ ) {
			insertIntoPathList( m_model->index( i, 0, index ) );
		}
	}
}

void FlatModel::rowsInserted( const QModelIndex & parent, int start, int end ) {
	for( int i = start ; i <= end ; i++ ) {
		QString path = m_model->filePath( m_model->index( start, 0, parent ) );
		m_pathList << path;
	}
	m_pathList.sort();
	emit layoutChanged();
}

void FlatModel::rowsRemoved( const QModelIndex & parent, int start, int end ) {
	for( int i = start ; i <= end ; i++ ) {
		QString path = m_model->filePath( m_model->index( start, 0, parent ) );
		m_pathList.removeAll( path );
	}
	emit layoutChanged();
}

QModelIndex FlatModel::mappingToSource( const QModelIndex & index ) {
	QString path = m_pathList.at( index.row() );
	return m_model->index( path );
}

int FlatModel::columnCount ( const QModelIndex & parent ) const {
	if( parent.isValid() ) {
		QString path = m_pathList.at( parent.row() );
		QModelIndex converti = m_model->index( path );
		return m_model->columnCount( converti );
	} else
		return m_model->columnCount( m_root );
}

QVariant FlatModel::data ( const QModelIndex & index, int role ) const {
	if( index.isValid() ) {
		QString path = m_pathList.at( index.row() );
		QModelIndex converti = m_model->index( path, index.column() );
		if( converti.isValid() ) {
			if( ( role == Qt::DisplayRole ) && ( m_model->isDir( converti ) ) ) {
				QDir dir = m_model->fileInfo( m_root ).absoluteDir();
				return dir.relativeFilePath( path );
			} else {
				return m_model->data( converti, role );
			}
		} else
			return QVariant();
	} else
		return QVariant();
}

Qt::ItemFlags FlatModel::flags ( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QString path = m_pathList.at( index.row() );
		QModelIndex converti = m_model->index( path );
		if( converti.isValid() )
			return m_model->flags( converti );
		else
			return Qt::ItemIsSelectable;
	} else
		return Qt::ItemIsSelectable;
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
		int s = m_pathList.size();
		return s;
	}
}


