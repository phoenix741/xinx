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

/* TreeProxyItemModel */

TreeProxyItemModel::TreeProxyItemModel( QObject * parent ) : QAbstractProxyModel( parent ) {

}

TreeProxyItemModel::~TreeProxyItemModel() {
	qDeleteAll( m_idMapping );
}

void TreeProxyItemModel::setSourceModel( QAbstractItemModel * sourceModel ) {
	QAbstractProxyModel::setSourceModel( sourceModel );
	createMapping();
}

TreeProxyItemModel::Mapping * TreeProxyItemModel::getMapping( int id ) const {
	Mapping * mapping = m_idMapping.value( id, 0 );
	if( ! mapping ) {
		mapping = new Mapping;
		mapping->id = id;
		mapping->parentId = 0;
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


/*! \internal
 * Create the mapping of all index in the table.
*/
void TreeProxyItemModel::createMapping() {
	qDeleteAll( m_idMapping );
	m_id2IndexMapping.clear();
	m_index2IdMapping.clear();

	int source_rows = sourceModel()->rowCount();

	for( int i = 0; i < source_rows; ++i ) {
		QModelIndex index = sourceModel()->index( i, 0 );
		int id            = getUniqueIdentifier( index );
		int parentId      = getParentUniqueIdentifier( index );

		Mapping * m       = getMapping( id );
		m->parentId      = parentId;

		m_idMapping.insert( m->id, m );

		m_id2IndexMapping[ m->id ] = i;
		m_index2IdMapping[ i     ] = m->id;

		Mapping * parrentMapping = getMapping( parentId );
		parrentMapping->childs.append( id );
	}

	reset();
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
	Q_ASSERT( sourceRow >= 0 );

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
	if( index.column() > 0 ) return QModelIndex();

	Mapping * mapping = static_cast<Mapping*>( index.internalPointer() );

	int parentId = mapping->parentId;
	if( parentId == 0 ) return QModelIndex();

	Mapping * parentMapping = getMapping( parentId );

	int grandParentId = parentMapping->parentId;
	Mapping * grandParentMapping = getMapping( grandParentId );

	int parentRow = grandParentMapping->childs.indexOf( parentId );
	int parentCol = index.column();

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

