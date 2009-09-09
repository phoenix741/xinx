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
#include "snipetmodelindex.h"
#include "snipetmanager.h"

// Qt header
#include <QIcon>
#include <QColor>
#include <QFont>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlError>

/* SnipetItemModel */

SnipetItemModel::SnipetItemModel( QSqlDatabase db, QObject * parent ) : QAbstractProxyModel( parent ), m_db( db ) {
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel( this );
	setSourceModel( m_sourceModel );

	createMapping();
}

SnipetItemModel::~SnipetItemModel() {
	qDeleteAll( m_sourcesIndexMapping );
}

void SnipetItemModel::select() {
	// Set the query used all snipet
	m_sourceModel->setQuery(
			"SELECT id, parent_id, ':/images/folder.png' as icon, name, ifnull(description,''), '' as shortcut, 'C' || ifnull(category_order,0) as list_order, 'CATEGORY' as type "
			"FROM categories "
			"UNION ALL "
			"SELECT id, category_id as parent_id, icon, name, ifnull(description,''), shortcut, 'S' || ifnull(snipet_order,0) as list_order, 'SNIPET' as type "
			"FROM snipets "
			"ORDER BY list_order", m_db
			);

	// Define name for header column
	m_sourceModel->setHeaderData( list_id, Qt::Horizontal, tr("Id") );
	m_sourceModel->setHeaderData( list_parentid, Qt::Horizontal, tr("Parent") );
	m_sourceModel->setHeaderData( list_icon, Qt::Horizontal, tr("Icon") );
	m_sourceModel->setHeaderData( list_name, Qt::Horizontal, tr("Name") );
	m_sourceModel->setHeaderData( list_description, Qt::Horizontal, tr("Description") );
	m_sourceModel->setHeaderData( list_shortcut, Qt::Horizontal, tr("Shortcut") );
	m_sourceModel->setHeaderData( list_order, Qt::Horizontal, tr("Order") );
	m_sourceModel->setHeaderData( list_type, Qt::Horizontal, tr("Type") );

	// Initialize the mapping
	createMapping();

	// Reset the layout
	reset();
}

/*! \internal
	Create the mapping of all index in the table.
*/
void SnipetItemModel::createMapping() {
	qDeleteAll( m_sourcesIndexMapping );
	m_sourcesIndexMapping.clear();
	m_categoryIdMapping.clear();

	int source_rows = m_sourceModel->rowCount();
	for( int i = -1; i < source_rows; ++i ) {
		Mapping * m = new Mapping;
		IndexMap::const_iterator it = IndexMap::const_iterator( m_sourcesIndexMapping.insert( i, m ) );
		m->map_iter = it;

		if( i >= 0 ) {
			QSqlRecord record = m_sourceModel->record( i );

			if( record.value( list_type ).toString() == "CATEGORY" ) {
				m->is_category = true;
				m_categoryIdMapping[ record.value( list_id ).toInt() ] = i;
			} else
				m->is_category = false;

			m->id = record.value( list_id ).toInt();
		} else { // Create the root Item
			m->is_category = true;
			m->id = 0;
			m_categoryIdMapping[ 0 ] = -1;
		}
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

/*
void SnipetItemModel::loadSnipetList( const SnipetList & list ) {
	foreach( const Snipet & s, list )
		m_snipetList[ s.category() ].append( s );
	reset();
}


SnipetList SnipetItemModel::getSnipetList() const {
	SnipetList result;
	foreach( const SnipetList & list, m_snipetList )
		result += list;
	return result;
}
*/

int SnipetItemModel::proxyColumnToSource( int proxyColumn ) const {
	switch( proxyColumn ) {
	case 0 :
		return list_name;
	case 1 :
		return list_shortcut;
	case 2 :
		return list_description;
	default:
		return -1;
	}
}

int SnipetItemModel::sourceColumnToProxy( int sourceColumn ) const {
	switch( sourceColumn ) {
	case list_name :
		return 0;
	case list_shortcut :
		return 1;
	case list_description :
		return 2;
	default:
		return -1;
	}
}

/// For the given source index, this method return the corresponding index in the proxy
QModelIndex SnipetItemModel::mapFromSource ( const QModelIndex & sourceIndex ) const {
	if( ! sourceIndex.isValid() ) return QModelIndex();
	if( sourceIndex.model() != m_sourceModel ) {
		qWarning( "SnipetItemModel: index from wrong model passed to mapFromSource" );
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

QModelIndex SnipetItemModel::mapToSource ( const QModelIndex & proxyIndex ) const {
	if( ! proxyIndex.isValid() ) return QModelIndex();
	if( proxyIndex.model() != this ) {
		qWarning( "SnipetItemModel: index from wrong model passed to mapToSource" );
		return QModelIndex();
	}

	IndexMap::const_iterator it = static_cast<Mapping*>( proxyIndex.internalPointer() )->map_iter;

	int sourceColumn = proxyColumnToSource( proxyIndex.column() );
	if( sourceColumn == -1 ) return QModelIndex();

	Mapping * m = it.value();
	int sourceRow = m->source_rows.at( proxyIndex.row() );

	return m_sourceModel->index( sourceRow, sourceColumn );
}

QModelIndex SnipetItemModel::index( int row, int column, const QModelIndex & parent ) const {
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

QModelIndex SnipetItemModel::parent( const QModelIndex & index ) const {
	if( ! index.isValid() ) return QModelIndex();

	IndexMap::const_iterator it = static_cast<Mapping*>( index.internalPointer() )->map_iter;
	Q_ASSERT( it != m_sourcesIndexMapping.constEnd() );

	int sourceRow = it.key();
	if( sourceRow == -1 ) return QModelIndex();

	QModelIndex sourceParent = m_sourceModel->index( sourceRow, proxyColumnToSource( 0 ) );
	QModelIndex proxyParent = mapFromSource( sourceParent );

	return proxyParent;
}

int SnipetItemModel::rowCount( const QModelIndex & index ) const {
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

int SnipetItemModel::columnCount( const QModelIndex & index ) const {
	Q_UNUSED( index );

	return 3;
}

Qt::ItemFlags SnipetItemModel::flags( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return Qt::ItemIsEnabled;
		}
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	}
	return 0;
}

QVariant SnipetItemModel::data( const QModelIndex & index, int role ) const {
	if( ! index.isValid() ) return QVariant();


	if( role == Qt::DecorationRole ) {
		if( index.column() == 0 ) {
			QModelIndex sourceIndex = mapToSource( index );
			QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
			return QIcon( record.value( list_icon ).toString() );
		}
	} else if( role == Qt::BackgroundRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return QColor( 0xFF, 0xFF, 0xCC );
		}
	} else if( role == Qt::FontRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			QFont currentFont;
			currentFont.setBold( true );
			return currentFont;
		}
	} else if( role == Qt::StatusTipRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_description );
	} else if( role == SnipetItemModel::SnipetIdRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_id );
	}

	return QAbstractProxyModel::data( index, role );
}

void SnipetItemModel::clear() {
	emit layoutAboutToBeChanged();

	qDeleteAll( m_sourcesIndexMapping );
	m_sourcesIndexMapping.clear();
	m_categoryIdMapping.clear();

	emit layoutChanged();
}

void SnipetItemModel::removeSnipet( const QModelIndexList & indexes ) {
	foreach( const QModelIndex & index, indexes ) {
		Mapping * parrentMapping = static_cast<Mapping*>( index.internalPointer() );
		int sourceRowIndex = parrentMapping->source_rows.at( index.row() );
		Mapping * rowMapping = m_sourcesIndexMapping.value( sourceRowIndex );

		SnipetDatabaseManager::self()->removeSnipet( rowMapping->id );
	}

	select();
}

void SnipetItemModel::importSnipetList( const SnipetList & list ) {
	SnipetDatabaseManager::self()->importSnipetList( list );
	select();
}

/*
void SnipetItemModel::removeSnipet( const QModelIndexList & indexes ) {
	QMultiMap<QString,int> deletedValue;
	foreach( const QModelIndex & index, indexes ) {
		deletedValue.insert( index.parent().data().toString(), index.row() );
	}

	foreach( const QString & category, deletedValue.uniqueKeys() ) {
		QList<int> lines = deletedValue.values( category );
		qSort( lines.begin(), lines.end(), qGreater<int>() );
		foreach( int line, lines ) {
			beginRemoveRows( index( m_snipetList.keys().indexOf( category ), 0 ), line, line );
			m_snipetList[ category ].removeAt( line );
			endRemoveRows();
		}
	}
}

void SnipetItemModel::addSnipet( const Snipet & snipet ) {
	QString category = snipet.category();
	int indexOfCategory = m_snipetList.keys().indexOf( category );

	if( indexOfCategory < 0 ) {
		m_snipetList[ category ] = SnipetList();
		reset();
		indexOfCategory = m_snipetList.keys().indexOf( category );
	}

	SnipetList list = m_snipetList.value( category );

	SnipetList::iterator i = qLowerBound( list.begin(), list.end(), snipet );
	beginInsertRows( index( indexOfCategory, 0 ), i - list.begin(), i - list.begin() );
	list.insert( i, snipet );
	m_snipetList[ category ] = list;
	endInsertRows();
}
*/
