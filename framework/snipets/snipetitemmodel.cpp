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

SnipetItemModel::SnipetItemModel( QSqlDatabase db, QObject * parent ) : TreeProxyItemModel( parent ), m_db( db ) {
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel( this );
	setSourceModel( m_sourceModel );
}

SnipetItemModel::~SnipetItemModel() {

}

void SnipetItemModel::select() {
	QSqlQuery query( m_db );

	// Set the query used all snipet
	query.prepare(
		"SELECT id, parent_id, ':/images/folder.png' as icon, name, ifnull(description,''), '' as shortcut, 'CATEGORY' as type, ifnull(available_script,'') "
		"FROM categories "
		"UNION ALL "
		"SELECT id, category_id as parent_id, icon, name, ifnull(description,''), shortcut, 'SNIPET' as type, ifnull(available_script,'') "
		"FROM snipets "
		"ORDER BY type, name"
			);
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
}

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

int SnipetItemModel::getTreeModelIdentifier( QString type, int id ) const {
	if( type == "CATEGORY" ) {
		if( id == 0 ) return 0;
		return qHash( QString("C%1").arg( id ) );
	} else {
		return qHash( QString("S%1").arg( id ) );
	}
}

int SnipetItemModel::getUniqueIdentifier( const QModelIndex & sourceIndex ) const {
	QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
	QString type = record.value( list_type ).toString();
	int id       = record.value( list_id ).toInt();
	return getTreeModelIdentifier( type, id );
}

int SnipetItemModel::getParentUniqueIdentifier( const QModelIndex & sourceIndex ) const {
	QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
	int parentId = record.value( list_parentid ).toInt();
	if( parentId == 0 ) return 0;

	return getTreeModelIdentifier( "CATEGORY", parentId );
}

/// For the given source index, this method return the corresponding index in the proxy
QModelIndex SnipetItemModel::mapFromSource ( const QModelIndex & sourceIndex ) const {
	QModelIndex index = mapFromSource( sourceIndex );
	int column = sourceColumnToProxy( index.column() );
	if( column == -1 ) return QModelIndex();

	return TreeProxyItemModel::createIndex( index.row(), column, index.internalPointer() );
}

QModelIndex SnipetItemModel::mapToSource ( const QModelIndex & proxyIndex ) const {
	QModelIndex index = proxyIndex;
	int column = proxyColumnToSource( index.column() );
	if( column == -1 ) return QModelIndex();

	return TreeProxyItemModel::mapToSource( createIndex( index.row(), column, index.internalPointer() ) );
}

QModelIndex SnipetItemModel::index( bool isCategory, int id ) const {
	if( isCategory && ( id == 0 ) ) return QModelIndex();

	QString type = isCategory ? "CATEGORY" : "SNIPET";
	return TreeProxyItemModel::index( getTreeModelIdentifier( type, id ) );
}

QModelIndex SnipetItemModel::index( int row, int column, const QModelIndex & parent ) const {
	return TreeProxyItemModel::index( row, column, parent );
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
	QStringList types = TreeProxyItemModel::mimeTypes();
	types << "application/snipet.id.list";
	return types;
}

Qt::DropActions SnipetItemModel::supportedDropActions() const {
	return Qt::MoveAction;
}

QMimeData * SnipetItemModel::mimeData( const QModelIndexList &indexes ) const {
	QMimeData * mimeData = TreeProxyItemModel::mimeData( indexes );
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
	Q_UNUSED( row );
	Q_UNUSED( column );

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

			int parentIdentifier = getTreeModelIdentifier( "CATEGORY", parentId );
			int identifier = getTreeModelIdentifier( type, id );

			setParentId( identifier, parentIdentifier );
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
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return QColor( 0xFF, 0xFF, 0xCC );
		}
	} else if( role == Qt::FontRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			QFont currentFont;
			currentFont.setBold( true );
			return currentFont;
		}
	} else if( role == Qt::StatusTipRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_description );
	} else if( role == SnipetItemModel::SnipetIdRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_id );
	} else if( role == SnipetItemModel::SnipetTypeRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_type );
	} else if( role == SnipetItemModel::SnipetParentIdRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_parentid );
	}

	return TreeProxyItemModel::data( index, role );
}

void SnipetItemModel::indexesToIds( const QModelIndexList & indexes, QList<int> & snipetIds, QList<int> & categoryIds ) const {
	foreach( const QModelIndex & index, indexes ) {
		QString type = index.data( SnipetTypeRole ).toString();
		bool isCategory = type == "CATEGORY";
		int id = index.data( SnipetIdRole ).toInt();

		if( isCategory ) {
			categoryIds.append( id );
		} else {
			snipetIds.append( id );
		}

	}
}

void SnipetItemModel::addIndexToList( QModelIndex index, QList<int> * ids ) const {
	QString type = index.data( SnipetTypeRole ).toString();
	bool isCategory = type == "CATEGORY";
	int id = index.data( SnipetIdRole ).toInt();

	if( isCategory ) {
		int row = index.model()->rowCount( index );
		for( int i = 0 ; i < row ; i++ ) {
			QModelIndex child = index.child( i, 0 );
			addIndexToList( child, ids );
		}
	} else {
		if( ! ids->contains( id ) )
			ids->append( id );
	}
}

QList<int> SnipetItemModel::indexesToIds( const QModelIndexList & indexes ) const {
	QList<int> ids;
	foreach( const QModelIndex & index, indexes ) {
		addIndexToList( index, &ids );
	}

	qSort( ids );
	return ids;
}

