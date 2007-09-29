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

// Xinx header
#include "flattreeview.h"
#include "exceptions.h"

// Qt header
#include <QHash>
#include <QDirModel>

class PrivateFlatModel : public QObject {
	Q_OBJECT
public:
	PrivateFlatModel( FlatModel * parent );
	virtual ~PrivateFlatModel();
	
	QDirModel * m_model;
	QPersistentModelIndex m_root;
	QStringList m_pathList;
		
public slots:
	void recalcPathList();
	void rowsInserted ( const QModelIndex & parent, int start, int end );
	void rowsRemoved ( const QModelIndex & parent, int start, int end );	
private:
	void insertIntoPathList( QModelIndex index );

	FlatModel * m_parent;
};

PrivateFlatModel::PrivateFlatModel( FlatModel * parent ) {
	m_parent = parent;
}

PrivateFlatModel::~PrivateFlatModel() {
	
}

void PrivateFlatModel::recalcPathList() {
	m_pathList.clear();
	insertIntoPathList( m_root );
	m_parent->reset();
}

void PrivateFlatModel::insertIntoPathList( QModelIndex index ) {
	if( m_model->isDir( index ) ) {
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

void PrivateFlatModel::rowsInserted( const QModelIndex & parent, int start, int end ) {
	for( int i = start ; i <= end ; i++ ) {
		QString path = m_model->filePath( m_model->index( start, 0, parent ) );
		m_pathList << path;
	}
	m_pathList.sort();
	emit m_parent->layoutChanged();
}

void PrivateFlatModel::rowsRemoved( const QModelIndex & parent, int start, int end ) {
	for( int i = start ; i <= end ; i++ ) {
		QString path = m_model->filePath( m_model->index( start, 0, parent ) );
		m_pathList.removeAll( path );
	}
	emit m_parent->layoutChanged();
}


/* FlatModel */

FlatModel::FlatModel( QDirModel * model, QModelIndex root ) {
	d = new PrivateFlatModel( this );
	d->m_model = model;
	d->m_root = QPersistentModelIndex( root );
	d->recalcPathList();
	connect( d->m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), d, SLOT(rowsInserted(QModelIndex,int,int)) );
	connect( d->m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), d, SLOT(rowsRemoved(QModelIndex,int,int)) );

	connect( d->m_model, SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()) );
	connect( d->m_model, SIGNAL(layoutChanged()), d, SLOT(recalcPathList()) );
	connect( d->m_model, SIGNAL(modelAboutToBeReset()), this, SIGNAL(modelAboutToBeReset()) );
	connect( d->m_model, SIGNAL(modelReset()), d, SLOT(recalcPathList()) );
}

FlatModel::~FlatModel() {
	delete d;
}
	
QModelIndex FlatModel::mappingToSource( const QModelIndex & index ) {
	QString path = d->m_pathList.at( index.row() );
	return d->m_model->index( path );
}

int FlatModel::columnCount ( const QModelIndex & parent ) const {
	if( parent.isValid() ) {
		QString path = d->m_pathList.at( parent.row() );
		QModelIndex converti = d->m_model->index( path );
		return d->m_model->columnCount( converti );
	} else
		return d->m_model->columnCount( d->m_root );
}

QVariant FlatModel::data ( const QModelIndex & index, int role ) const {
	if( index.isValid() ) {
		QString path = d->m_pathList.at( index.row() );
		QModelIndex converti = d->m_model->index( path, index.column() );
		if( ( role == Qt::DisplayRole ) && ( d->m_model->isDir( converti ) ) ) {
			QDir dir = d->m_model->fileInfo( d->m_root ).absoluteDir();
			return dir.relativeFilePath( path );
		} else {
			return d->m_model->data( converti, role );
		}
	} else
		return QVariant();
}

Qt::ItemFlags FlatModel::flags ( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QString path = d->m_pathList.at( index.row() );
		QModelIndex converti = d->m_model->index( path );
		return d->m_model->flags( converti );
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
		int s = d->m_pathList.size();
		return s;
	}
}

#include "flattreeview.moc"

