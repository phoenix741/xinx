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
#include "snipetitemmodel.h"
#include "snipetmanager.h"

// Qt header
#include <QIcon>
#include <QColor>
#include <QFont>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include <QMimeData>

/* SnipetItemModel */

SnipetItemModel::SnipetItemModel( QSqlDatabase db, QObject * parent ) : QAbstractProxyModel( parent ), m_db( db ) {
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel( this );
	setSourceModel( m_sourceModel );

	createMapping();
}

SnipetItemModel::~SnipetItemModel() {
	qDeleteAll( m_sourcesIndexMapping );
	delete m_sourceModel;
}

void SnipetItemModel::select( const QString & filter ) {
	m_filter = filter;

	QSqlQuery query( m_db );

	if( filter.isEmpty() ) {
		// Set the query used all snipet
		query.prepare(
			"SELECT id, parent_id, ':/images/folder.png' as icon, name, ifnull(description,''), '' as shortcut, 'CATEGORY' as type, ifnull(available_script,'') "
			"FROM categories "
			"UNION ALL "
			"SELECT id, category_id as parent_id, icon, name, ifnull(description,''), shortcut, 'SNIPET' as type, ifnull(available_script,'') "
			"FROM snipets "
			"ORDER BY type, name"
				);
	} else {
		query.prepare(
			"SELECT id, parent_id, ':/images/folder.png' as icon, name, ifnull(description,''), '' as shortcut, 'CATEGORY' as type, ifnull(available_script,'') "
			"FROM categories "
			"UNION ALL "
			"SELECT id, category_id as parent_id, icon, name, ifnull(description,''), shortcut, 'SNIPET' as type, ifnull(available_script,'') "
			"FROM snipets "
			"WHERE name||description||shortcut like '%'||:filter||'%'"
			"ORDER BY type, name"
				);
		query.bindValue( ":filter", filter );
	}
	Q_ASSERT( query.exec() );
	m_sourceModel->setQuery( query );


	// Define name for header column
	m_sourceModel->setHeaderData( list_id, Qt::Horizontal, tr("Id") );
	m_sourceModel->setHeaderData( list_parentid, Qt::Horizontal, tr("Parent") );
	m_sourceModel->setHeaderData( list_icon, Qt::Horizontal, tr("Icon") );
	m_sourceModel->setHeaderData( list_name, Qt::Horizontal, tr("Name") );
	m_sourceModel->setHeaderData( list_description, Qt::Horizontal, tr("Description") );
	m_sourceModel->setHeaderData( list_shortcut, Qt::Horizontal, tr("Shortcut") );
	m_sourceModel->setHeaderData( list_type, Qt::Horizontal, tr("Type") );
	m_sourceModel->setHeaderData( list_availablejs, Qt::Horizontal, tr("Available Script") );

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
	m_snipetIdMapping.clear();

	int source_rows = m_sourceModel->rowCount();
	for( int i = -1; i < source_rows; ++i ) {
		Mapping * m = new Mapping;
		m_sourcesIndexMapping.insert( i, m );
		m->is_category = false;
		m->parrentId = 0;
		m->index = i;
		m->parentIndex = 0;

		if( i >= 0 ) {
		QSqlRecord record = m_sourceModel->record( i );

		if( record.value( list_type ).toString() == "CATEGORY" ) {
			m->is_category = true;
			m_categoryIdMapping[ record.value( list_id ).toInt() ] = i;
		} else {
			m->is_category = false;
			m_snipetIdMapping[ record.value( list_id ).toInt() ] = i;
		}

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

QSqlQueryModel * SnipetItemModel::sourceModel() {
	return m_sourceModel;
}

QSqlQueryModel * SnipetItemModel::sourceModel() const {
	return m_sourceModel;
}

QSqlDatabase SnipetItemModel::database() {
	return m_db;
}

QSqlDatabase SnipetItemModel::database() const {
	return m_db;
}

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

	Mapping * m = static_cast<Mapping*>( proxyIndex.internalPointer() );

	int sourceColumn = proxyColumnToSource( proxyIndex.column() );
	if( sourceColumn == -1 ) return QModelIndex();

	int sourceRow = m->source_rows.at( proxyIndex.row() );

	return m_sourceModel->index( sourceRow, sourceColumn );
}

QModelIndex SnipetItemModel::index( bool isCategory, int id ) const {
	int sourceRow = -1;
	if( isCategory )
		sourceRow = m_categoryIdMapping.value( id, -1 );
	else
		sourceRow = m_snipetIdMapping.value( id, -1 );

	if( sourceRow >= 0 ) {
		Mapping * m = m_sourcesIndexMapping.value( sourceRow );

		int parentRow = m->parentIndex;
		IndexMap::const_iterator parentIt = m_sourcesIndexMapping.constFind( parentRow );
		Q_ASSERT( parentIt != m_sourcesIndexMapping.constEnd() );

		Mapping * parrentMapping = parentIt.value();

		int proxyRow = parrentMapping->source_rows.indexOf( m->index );

		return createIndex( proxyRow, 0, *parentIt );
	}
	return QModelIndex();
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

	Mapping * m = static_cast<Mapping*>( index.internalPointer() );

	int sourceRow = m->index;
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
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
		}
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
	}
	return 0;
}

QStringList SnipetItemModel::mimeTypes() const {
	QStringList types = QAbstractProxyModel::mimeTypes();
	types << "application/snipet.id.list";
	return types;
}

Qt::DropActions SnipetItemModel::supportedDropActions() const {
	return Qt::MoveAction;
}

QMimeData * SnipetItemModel::mimeData( const QModelIndexList &indexes ) const {
	QMimeData * mimeData = QAbstractProxyModel::mimeData( indexes );
	QByteArray encodedData;
	QDataStream stream( &encodedData, QIODevice::WriteOnly );

	foreach( QModelIndex index, indexes ) {
		if( index.isValid() ) {
			QString type = data( index, SnipetTypeRole ).toString();
			QString name = data( index, Qt::DisplayRole ).toString();
			int id = data( index, SnipetIdRole ).toInt();
			stream << id << type << name;
		}
	}

	mimeData->setData( "application/snipet.id.list", encodedData );
	return mimeData;
}

bool SnipetItemModel::dropMimeData( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent ) {
	if( ( action == Qt::MoveAction ) && data->hasFormat( "application/snipet.id.list" ) ) {
		int parentId = parent.data( SnipetIdRole ).toInt();

		QByteArray itemData = data->data("application/snipet.id.list");
		QDataStream stream(&itemData, QIODevice::ReadOnly);

		while( ! stream.atEnd() ) {
			int id;
			QString type, name;
			stream >> id >> type >> name;

			QSqlQuery updateQuery( m_db );
			if( type == "CATEGORY" ) {
				updateQuery.prepare( "UPDATE categories SET parent_id=:new_parent WHERE id=:id" );
			} else {
				updateQuery.prepare( "UPDATE snipets SET category_id=:new_parent WHERE id=:id" );
			}
			updateQuery.bindValue( ":new_parent", parentId );
			updateQuery.bindValue( ":id", id );

			bool result = updateQuery.exec();
			Q_ASSERT( result );


			select( m_filter );
		}

		return true;
	}
	return false;
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
	} else if( role == SnipetItemModel::SnipetTypeRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_type );
	} else if( role == SnipetItemModel::SnipetParentIdRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_parentid );
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

void SnipetItemModel::removeIndexes( const QModelIndexList & indexes, QWidget * parent ) {
	foreach( const QModelIndex & index, indexes ) {
		Mapping * parrentMapping = static_cast<Mapping*>( index.internalPointer() );
		int sourceRowIndex = parrentMapping->source_rows.at( index.row() );
		Mapping * rowMapping = m_sourcesIndexMapping.value( sourceRowIndex );

		if( rowMapping->is_category ) {
			SnipetManager::self()->removeCategory( rowMapping->id, parent );
		} else {
			SnipetManager::self()->removeSnipet( rowMapping->id, parent );
		}
	}

	select( m_filter );
}

void SnipetItemModel::importSnipetList( const SnipetList & list ) {
	SnipetManager::self()->importSnipetList( list );
	select( m_filter );
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
