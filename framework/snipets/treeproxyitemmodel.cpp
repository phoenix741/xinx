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
#include "treeproxyitemmodel.h"

// Qt header
#include <QVariant>
#include <QDebug>

/* TreeProxyItemModel */

TreeProxyItemModel::TreeProxyItemModel( QObject * parent ) : QAbstractProxyModel( parent ), m_sourceColumnCount( 0 ), m_resetModel( false ) {
}

TreeProxyItemModel::~TreeProxyItemModel() {
	qDeleteAll( m_idMapping );
}

void TreeProxyItemModel::setSourceModel( QAbstractItemModel * sourceModel ) {
	QAbstractProxyModel::setSourceModel( sourceModel );
	createMapping();
}

bool TreeProxyItemModel::resetModel() const {
	return m_resetModel;
}

void TreeProxyItemModel::setResetModel( bool value ) {
	m_resetModel = value;
}

TreeProxyItemModel::Mapping * TreeProxyItemModel::getMapping( int id ) const {
	Mapping * mapping = m_idMapping.value( id, 0 );
	if( ! mapping ) {
		mapping = new Mapping;
		mapping->id = id;
		mapping->parentId = -1;
		m_idMapping.insert( id, mapping );
	}
	return mapping;
}

TreeProxyItemModel::Mapping * TreeProxyItemModel::getMapping( const QModelIndex & index ) const {
	if( ! index.isValid() ) return getMapping( 0 );

	Mapping * parentMapping = static_cast<Mapping*>( index.internalPointer() );
	Q_ASSERT( parentMapping );

	int id = parentMapping->childs.at( index.row() );
	Mapping * childMapping = getMapping( id );
	Q_ASSERT( childMapping );

	return childMapping;
}

void TreeProxyItemModel::setParentId( int id, int parentId ) {
	Mapping * mapping = getMapping( id );
	Q_ASSERT( mapping );

	if( parentId != mapping->parentId ) {
		if( mapping->parentId >= 0 ) {
			Mapping * parentMapping = getMapping( mapping->parentId );
			int row = parentMapping->childs.indexOf( mapping->id );
			beginRemoveRows( index( mapping->parentId ), row, row );

			parentMapping->childs.remove( row );
			mapping->parentId = -1;

			endRemoveRows();
		}
		if( parentId != -1 ) {
			Mapping * parentMapping = getMapping( parentId );
			int row = parentMapping->childs.size();
			if( ( parentMapping->parentId >= 0 ) || ( parentMapping->id == 0 ) )
				beginInsertRows( index( parentMapping->id ), row, row);

			parentMapping->childs.insert( row, mapping->id );
			mapping->parentId = parentId;

			if( ( parentMapping->parentId >= 0 ) || ( parentMapping->id == 0 ) )
				endInsertRows();
		}
	} else {
		QModelIndex idx = index( id );
		emit dataChanged( idx, idx );
	}
}

/*! \internal
 * Create the mapping of all index in the table.
*/
void TreeProxyItemModel::createMapping() {
	if( m_resetModel || ( m_sourceColumnCount != sourceModel()->columnCount() ) ) {
		// The model completly change
		qDeleteAll( m_idMapping );
		m_idMapping.clear();
		m_sourceColumnCount = sourceModel()->columnCount();
		m_id2IndexMapping.clear();
		m_index2IdMapping.clear();

		reset();
	}

	QList<int> currentIds = m_idMapping.keys();
	QList< QPair<int,int> > idsList;
	currentIds.removeAll( 0 );

	m_id2IndexMapping.clear();
	m_index2IdMapping.clear();

	int source_rows = sourceModel()->rowCount();

	for( int i = 0; i < source_rows; ++i ) {
		QModelIndex index = sourceModel()->index( i, 0 );
		int id            = getUniqueIdentifier( index );
		int parentId      = getParentUniqueIdentifier( index );

		m_id2IndexMapping[ id ] = i;
		m_index2IdMapping[ i  ] = id;

		idsList.append( qMakePair( id, parentId ) );
		currentIds.removeAll( id );
	}

	foreach( int id, currentIds ) {
		setParentId( id, -1 );
		IndexMap::iterator it = m_idMapping.find( id );
		delete it.value();
		m_idMapping.erase( it );
	}

	for( int i = 0; i < source_rows; ++i ) {
		int id            = idsList.at( i ).first;
		int parentId      = idsList.at( i ).second;

		setParentId( id, parentId );
	}

	/*
	printMapping( 0 );
	qDebug() << m_id2IndexMapping.keys();
	qDebug() << m_index2IdMapping.keys();
	*/
}

void TreeProxyItemModel::printMapping( int id, int niveau ) const {
	Mapping * m = getMapping( id );
	qDebug() << "Niveau : " << niveau << ", Id : " << m->id << ", Parent : " << m->parentId << ", childs : " << m->childs.size();

	foreach( int child, m->childs ) {
		printMapping( child, niveau + 1 );
	}
}

/// For the given source index, this method return the corresponding index in the proxy
QModelIndex TreeProxyItemModel::mapFromSource ( const QModelIndex & sourceIndex ) const {
	if( ! sourceIndex.isValid() ) return QModelIndex();
	if( sourceIndex.model() != sourceModel() ) {
		qWarning( "TreeProxyItemModel: index from wrong model passed to mapFromSource" );
		return QModelIndex();
	}

	int sourceRow = sourceIndex.row();
	int id        = m_index2IdMapping.value( sourceRow, -1 );
	Q_ASSERT( id >= 0 );

	Mapping * mapping = getMapping( id );
	int parentId = mapping->parentId;

	Mapping * parentMapping = getMapping( parentId );
	Q_ASSERT( parentMapping );

	int 	proxyRow = parentMapping->childs.indexOf( id ),
			proxyColumn = sourceIndex.column();

	return createIndex( proxyRow, proxyColumn, parentMapping );
}

QModelIndex TreeProxyItemModel::mapToSource ( const QModelIndex & proxyIndex ) const {
	if( ! proxyIndex.isValid() ) return QModelIndex();
	if( proxyIndex.model() != this ) {
		qWarning( "TreeProxyItemModel: index from wrong model passed to mapToSource" );
		return QModelIndex();
	}

	Mapping * mapping = static_cast<Mapping*>( proxyIndex.internalPointer() );

	int sourceColumn = proxyIndex.column();
	if( sourceColumn == -1 ) return QModelIndex();

	int sourceId  = mapping->childs.at( proxyIndex.row() );
	int sourceRow = m_id2IndexMapping.value( sourceId, -1 );
	if( sourceRow == -1 ) { // Source Row deleted
		return QModelIndex();
	}

	return sourceModel()->index( sourceRow, sourceColumn );
}

QModelIndex TreeProxyItemModel::index( int id ) const {
	if( id == 0 ) return QModelIndex();

	Mapping * mapping = getMapping( id );
	int parentId = mapping->parentId;

	Mapping * parentMapping = getMapping( parentId );
	int row = parentMapping->childs.indexOf( id );

	return createIndex( row, 0, parentMapping );
}

QModelIndex TreeProxyItemModel::index( int row, int column, const QModelIndex & parent ) const {
	if( ( row < 0 ) || ( column < 0 ) ) return QModelIndex();

	Mapping * parentMapping = getMapping( parent );
	Q_ASSERT( parentMapping );

	if( row >= parentMapping->childs.count() ) return QModelIndex();
	if( column >= sourceModel()->columnCount() ) return QModelIndex();

	return createIndex( row, column, parentMapping );
}

QModelIndex TreeProxyItemModel::parent( const QModelIndex & index ) const {
	if( ! index.isValid() ) return QModelIndex();

	Mapping * parentMapping = static_cast<Mapping*>( index.internalPointer() );

	int parentId = parentMapping->id;
	if( parentId == 0 ) return QModelIndex();

	int grandParentId = parentMapping->parentId;
	Mapping * grandParentMapping = getMapping( grandParentId );

	int parentRow = grandParentMapping->childs.indexOf( parentId );
	int parentCol = 0;

	return createIndex( parentRow, parentCol, grandParentMapping );
}

int TreeProxyItemModel::rowCount( const QModelIndex & index ) const {
	if( index.column() > 0 ) return 0;

	Mapping * m = getMapping( index );
	return m->childs.count();
}

int TreeProxyItemModel::columnCount( const QModelIndex & index ) const {
	return sourceModel()->columnCount( mapToSource( index ) );
}

QVariant TreeProxyItemModel::data( const QModelIndex &proxyIndex, int role ) const {
	Mapping * m = getMapping( proxyIndex );
	if( -1 == m_id2IndexMapping.value( m->id, -1 ) ) {
		if( role == Qt::DisplayRole ) {
			return tr("<deleted>");
		} else {
			return QVariant();
		}
	}
	return QAbstractProxyModel::data( proxyIndex, role );
}

Qt::ItemFlags TreeProxyItemModel::flags( const QModelIndex &index ) const {
	Mapping * m = getMapping( index );
	int sourceRow = m_id2IndexMapping.value( m->id, -1 );
	if( sourceRow == -1 ) return 0;
	return QAbstractProxyModel::flags( index );
}

