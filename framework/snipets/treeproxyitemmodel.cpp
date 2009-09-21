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

/* SnipetItemModel */

TreeProxyItemModel::TreeProxyItemModel( QObject * parent ) : QAbstractProxyModel( parent ) {

}

TreeProxyItemModel::~TreeProxyItemModel() {
	qDeleteAll( m_idMapping );
}

void TreeProxyItemModel::setSourceModel( QAbstractItemModel * sourceModel ) {
	QAbstractProxyModel::setSourceModel( sourceModel );
	createMapping();
}

Mapping * TreeProxyItemModel::getMapping( int id ) {
	Mapping * mapping = m_id.value( id, 0 );
	if( ! mapping ) {
		mapping = new Mapping;
		mapping->id = id;
		mapping->parrentId = 0;
	}
	return mapping;
}


/*! \internal
	Create the mapping of all index in the table.
*/
void TreeProxyItemModel::createMapping() {
	qDeleteAll( m_idMapping );
	m_id2IndexMapping.clear();
	m_index2IdMapping.clear();

	int source_rows = sourceModel()->rowCount();

	for( int i = 0; i < source_rows; ++i ) {
		Mapping * m = new Mapping;
		m->id        = getUniqueIdentifier( sourceModel()->index( i, 0 ) );
		m->parrentId = getParentUniqueIdentifier( sourceModel()->index( i, 0 ) );

		m_idMapping.insert( m->id, m );

		m_id2IndexMapping[ m->id ] = i;
		m_index2IdMapping[ i     ] = m->id;
	}

	for( int i = 0; i < source_rows; ++i ) {
		QSqlRecord record = m_sourceModel->record( i );

		int parentCategoryId = record.value( list_parentid ).toInt();
		int parentCategoryIndex = m_categoryIdMapping.value( parentCategoryId, -2 );
		Q_ASSERT( parentCategoryIndex > -2 );
		Mapping * mapping = m_sourcesIndexMapping.value( i );
		mapping->parentIndex = parentCategoryIndex;
		mapping->parrentId   = parentCategoryId;

		Mapping * categoryMapping = m_sourcesIndexMapping.value( parentCategoryIndex );
		categoryMapping->source_rows.append( i );
	}
}

Mapping * TreeProxyItemModel::getMapping( int index ) {

}

/// For the given source index, this method return the corresponding index in the proxy
QModelIndex TreeProxyItemModel::mapFromSource ( const QModelIndex & sourceIndex ) const {
	if( ! sourceIndex.isValid() ) return QModelIndex();
	if( sourceIndex.model() != m_sourceModel ) {
		qWarning( "TreeProxyItemModel: index from wrong model passed to mapFromSource" );
		return QModelIndex();
	}

	int row = sourceIndex.row();
	IndexMap::const_iterator it = m_sourcesIndexMapping.constFind( row );
	Q_ASSERT( it != m_sourcesIndexMapping.constEnd() );

	int parentRow = it.value()->parentIndex;
	IndexMap::const_iterator parentIt = m_sourcesIndexMapping.constFind( parentRow );
	Q_ASSERT( parentIt != m_sourcesIndexMapping.constEnd() );

	Mapping * m = parentIt.value();

	int proxyRow = m->source_rows.indexOf( row ), proxyColumn = sourceColumnToProxy( sourceIndex.column() );
	if( proxyColumn == -1 ) return QModelIndex();

	return createIndex( proxyRow, proxyColumn, *parentIt );
}

QModelIndex TreeProxyItemModel::mapToSource ( const QModelIndex & proxyIndex ) const {
	if( ! proxyIndex.isValid() ) return QModelIndex();
	if( proxyIndex.model() != this ) {
		qWarning( "TreeProxyItemModel: index from wrong model passed to mapToSource" );
		return QModelIndex();
	}

	Mapping * m = static_cast<Mapping*>( proxyIndex.internalPointer() );

	int sourceColumn = proxyColumnToSource( proxyIndex.column() );
	if( sourceColumn == -1 ) return QModelIndex();

	int sourceRow = m->source_rows.at( proxyIndex.row() );

	return m_sourceModel->index( sourceRow, sourceColumn );
}

QModelIndex TreeProxyItemModel::index( int row, int column, const QModelIndex & parent ) const {
	if( ( row < 0 ) || ( column < 0 ) ) return QModelIndex();

	IndexMap::const_iterator it = m_sourcesIndexMapping.constFind( -1 );

	QModelIndex sourceParent = mapToSource( parent );
	if( sourceParent.isValid() ) {
		it = m_sourcesIndexMapping.constFind( sourceParent.row() );
	}

	Q_ASSERT( it != m_sourcesIndexMapping.constEnd() );
	if( it.value()->source_rows.count() <= row )
		return QModelIndex();

	return createIndex( row, column, *it );
}

QModelIndex TreeProxyItemModel::parent( const QModelIndex & index ) const {
	if( ! index.isValid() ) return QModelIndex();

	Mapping * m = static_cast<Mapping*>( index.internalPointer() );

	int sourceRow = m->index;
	if( sourceRow == -1 ) return QModelIndex();

	QModelIndex sourceParent = m_sourceModel->index( sourceRow, proxyColumnToSource( 0 ) );
	QModelIndex proxyParent = mapFromSource( sourceParent );

	return proxyParent;
}

int TreeProxyItemModel::rowCount( const QModelIndex & index ) const {
	if( index.column() > 0 ) return 0;
	if( ! index.isValid() ) {
		Mapping * rootMapping = m_sourcesIndexMapping.value( -1 );
		return rootMapping->source_rows.count();
	} else {
		Mapping * parrentMapping = static_cast<Mapping*>( index.internalPointer() );
		int sourceRowIndex = parrentMapping->source_rows.at( index.row() );
		Mapping * rowMapping = m_sourcesIndexMapping.value( sourceRowIndex );

		return rowMapping->source_rows.count();
	}
}

int TreeProxyItemModel::columnCount( const QModelIndex & index ) const {
	return sourceModel()->columnCount( mapToSource( index ) );
}

