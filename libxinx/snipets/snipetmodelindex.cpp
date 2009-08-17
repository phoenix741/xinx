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

	m_sourceModel->setQuery(
			"SELECT id, parent_id, ':/images/folder.png' as icon, name, ifnull(description,''), '' as shortcut, 'C' || ifnull(category_order,0) as list_order, 'CATEGORY' as type "
			"FROM category "
			"UNION ALL "
			"SELECT id, category_id as parent_id, icon, name, ifnull(description,''), shortcut, 'S' || ifnull(snipet_order,0) as list_order, 'SNIPET' as type "
			"FROM snipets "
			"ORDER BY list_order", db
			);

	m_sourceModel->setHeaderData( list_id, Qt::Horizontal, tr("Id") );
	m_sourceModel->setHeaderData( list_parentid, Qt::Horizontal, tr("Parent") );
	m_sourceModel->setHeaderData( list_icon, Qt::Horizontal, tr("Icon") );
	m_sourceModel->setHeaderData( list_name, Qt::Horizontal, tr("Name") );
	m_sourceModel->setHeaderData( list_description, Qt::Horizontal, tr("Description") );
	m_sourceModel->setHeaderData( list_shortcut, Qt::Horizontal, tr("Shortcut") );
	m_sourceModel->setHeaderData( list_order, Qt::Horizontal, tr("Order") );
	m_sourceModel->setHeaderData( list_type, Qt::Horizontal, tr("Type") );
}

SnipetItemModel::~SnipetItemModel() {

}

/*
void SnipetItemModel::loadSnipetList( const SnipetList & list ) {
	foreach( const Snipet & s, list )
		m_snipetList[ s.category() ].append( s );
	reset();
}

void SnipetItemModel::clear() {
	m_snipetList.clear();
	reset();
}

SnipetList SnipetItemModel::getSnipetList() const {
	SnipetList result;
	foreach( const SnipetList & list, m_snipetList )
		result += list;
	return result;
}
*/

QModelIndex SnipetItemModel::mapFromSource ( const QModelIndex & sourceIndex ) const {
	int proxyColumn = -1;
	switch( sourceIndex.column() ) {
	case list_name :
		proxyColumn = 0;
		break;
	case list_shortcut :
		proxyColumn = 1;
		break;
	case list_description :
		proxyColumn = 2;
		break;
	default:
		return QModelIndex();
	}
	return createIndex( sourceIndex.row(), proxyColumn, sourceIndex.internalPointer() );
}

QModelIndex SnipetItemModel::mapToSource ( const QModelIndex & proxyIndex ) const {
	int sourceColumn = -1;
	switch( proxyIndex.column() ) {
	case 0 :
		sourceColumn = list_name;
		break;
	case 1 :
		sourceColumn = list_shortcut;
		break;
	case 2 :
		sourceColumn = list_description;
		break;
	}
	return createIndex( proxyIndex.row(), sourceColumn, proxyIndex.internalPointer() );
}

QModelIndex SnipetItemModel::index( int row, int column, const QModelIndex & parent ) const {
	if( ( row < 0 ) || ( column < 0 ) ) return QModelIndex();

	int sourceColumn = -1;
	switch( column ) {
	case 0 :
		sourceColumn = list_name;
		break;
	case 1 :
		sourceColumn = list_shortcut;
		break;
	case 2 :
		sourceColumn = list_description;
		break;
	default:
		return QModelIndex();
	}
	QModelIndex sourceIndex = m_sourceModel->index( row, sourceColumn, parent );

	return createIndex( row, column, sourceIndex.internalPointer() );
}

QModelIndex SnipetItemModel::parent( const QModelIndex & index ) const {
	return QModelIndex();
}

int SnipetItemModel::rowCount( const QModelIndex & index ) const {
	return m_sourceModel->rowCount( index );
}

int SnipetItemModel::columnCount( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QSqlRecord record = m_sourceModel->record( index.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return 1;
		}
	}
	return 3;
}

Qt::ItemFlags SnipetItemModel::flags( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QSqlRecord record = m_sourceModel->record( index.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return Qt::ItemIsEnabled;
		}
	}
	return QAbstractItemModel::flags( index );
}

QVariant SnipetItemModel::data( const QModelIndex & index, int role ) const {
	if( ! index.isValid() ) return QVariant();

	if( role == Qt::DisplayRole ) {
		return QAbstractProxyModel::data( index, role );
	} else if( role == Qt::DecorationRole ) {
		if( index.column() == 0 ) {
			QSqlRecord record = m_sourceModel->record( index.row() );
			return QIcon( record.value( list_icon ).toString() );
		}
	} else if( role == Qt::BackgroundRole ) {
		QSqlRecord record = m_sourceModel->record( index.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return QColor( 0xFF, 0xFF, 0xCC );
		}
	} else if( role == Qt::FontRole ) {
		QSqlRecord record = m_sourceModel->record( index.row() );
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			QFont currentFont;
			currentFont.setBold( true );
			return currentFont;
		}
	}

	return QVariant();
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
