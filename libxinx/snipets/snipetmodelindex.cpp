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

/* SnipetItemModel */

SnipetItemModel::SnipetItemModel( QSqlDatabase db, QObject * parent ) : QAbstractItemModel( parent ), m_db( db ) {
	m_query = QSqlQuery( "SELECT id, '' as icon, name, '' as shortcut, description, name as snipet_order "
						 "FROM category "
						 "WHERE parent_id=:id "
						 "UNION ALL "
						 "SELECT id, icon, name, shortcut, description, 'ZZZ' || snipet_order "
						 "FROM snipet "
						 "WHERE category_id=:id "
						 "ORDER BY order", db );
}

SnipetItemModel::~SnipetItemModel() {
	qDeleteAll( m_internalId );
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

QModelIndex SnipetItemModel::index( int row, int column, const QModelIndex & parent ) const {
	if( ( column < 0 ) || ( column > 3 ) ) return QModelIndex(); // Test supplémentaire pour ModelTest

	if( parent.isValid() && ( parent.internalId() == -1 ) ) {
		if( ( row < 0 ) || ( row >= m_snipetList.values().at( parent.row() ).size() ) ) return QModelIndex();
		return createIndex( row, column, parent.row() );
	} else if( !parent.isValid() ) {
		if( ( row < 0 ) || ( row >= m_snipetList.keys().size() ) ) return QModelIndex();
		return createIndex( row, column, -1 );
	}
	return QModelIndex();
}

QModelIndex SnipetItemModel::parent( const QModelIndex & index ) const {
	if( index.isValid() && (index.internalId() >= 0) ) {
		return createIndex( index.internalId(), 0, -1 );
	}
	return QModelIndex();
}

int SnipetItemModel::rowCount( const QModelIndex & index ) const {
	if( index.column() != 0 ) return 0; // Column don't have children (Test supplémentaire pour ModelTest)
	if( ! index.isValid() ) {
		m_query.bindValue( ":id", 0 );
	} else {
		struct indexInternalPointer * p = static_cast<struct indexInternalPointer>( index.internalPointer() );
		if( p )
			m_query.bindValue( ":id", p->parent_id );
		else
			m_query.bindValue( ":id", 0 );
	}
	m_query.exec();

	return m_query.size();
}

int SnipetItemModel::columnCount( const QModelIndex & index ) const {
	if( index.isValid() && (index.internalId() == -1) ) return 1;
	return 3;
}

Qt::ItemFlags SnipetItemModel::flags( const QModelIndex & index ) const {
	if( index.isValid() && (index.internalId() == -1) ) {
		return Qt::ItemIsEnabled;
	} else
		return QAbstractItemModel::flags( index );
}

QVariant SnipetItemModel::headerData( int section, Qt::Orientation orientation, int role ) const {
	if( role != Qt::DisplayRole ) return QVariant();

	if( orientation == Qt::Horizontal ) {
		switch( section ) {
		case 0:
			return tr("Name");
		case 1:
			return tr("Key");
		case 2:
			return tr("Description");
		default:
			return QVariant();
		}
	} else
		return QVariant();
}

QVariant SnipetItemModel::data( const QModelIndex & index, int role ) const {
	if( ! index.isValid() ) return QVariant();

	if( index.internalId() == -1 ) {
		if( role == Qt::DisplayRole ) {
			switch( index.column() ) {
			case 0:
				return m_snipetList.keys().at( index.row() );
			case 1:
			case 2:
				return QString(""); // Needed for Model tests
			}
		} else if( role == Qt::DecorationRole ) {
			if( index.column() == 0 )
				return QIcon( ":/images/folder.png" );
		} else if( role == Qt::BackgroundRole ) {
			return QColor( 0xFF, 0xFF, 0xCC );
		} else if( role == Qt::FontRole ) {
			QFont currentFont;
			currentFont.setBold( true );
			return currentFont;
		}
	} else {
		int line = index.row();
		int category = index.internalId();
		if( ( line < 0 ) || ( line >= m_snipetList.values().at( category ).size() ) ) return QVariant();

		if( role == Qt::DisplayRole )
			switch( index.column() ) {
			case 0:
				return m_snipetList.values().at( category ).at( line ).name();
			case 1:
				return m_snipetList.values().at( category ).at( line ).key();
			case 2:
				return m_snipetList.values().at( category ).at( line ).description();
		} else if( role == Qt::DecorationRole ) {
			if( index.column() == 0 )
				return QIcon( m_snipetList.values().at( category ).at( line ).icon() );
		} else if( role == Qt::UserRole ) {
			if( index.column() == 0 )
				return QVariant::fromValue( m_snipetList.values().at( category ).at( line ) );
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
