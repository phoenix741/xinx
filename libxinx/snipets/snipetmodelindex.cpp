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
#include <QSqlError>

/* SnipetItemModel */

SnipetItemModel::SnipetItemModel( QSqlDatabase db, QObject * parent ) : QAbstractItemModel( parent ), m_db( db ) {
	m_listQuery = QSqlQuery( "SELECT id, name as snipet_order, 'C' as cat "
						 "FROM category "
						 "WHERE parent_id=:id "
						 "UNION ALL "
						 "SELECT id, 'ZZZ' || snipet_order, 'S' as cat "
						 "FROM snipets "
						 "WHERE category_id=:id "
						 "ORDER BY snipet_order", db );
	m_parentQuery = QSqlQuery( "SELECT parent_id as id "
						 "FROM category "
						 "WHERE id=:id "
						 "UNION ALL "
						 "SELECT category_id as id "
						 "FROM snipets "
						 "WHERE id=:id ", db );
	m_categoryQuery = QSqlQuery( "SELECT id, name, description, parent_id "
						 "FROM category "
						 "WHERE id=:id ", db );
	m_snipetQuery = QSqlQuery( "SELECT id, icon, name, shortcut, description, category_id "
						 "FROM snipets "
						 "WHERE id=:id ", db );
}

SnipetItemModel::~SnipetItemModel() {
	qDeleteAll( m_internalPointers.values() );
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

SnipetItemModel::indexInternalPointer * SnipetItemModel::getInternalPointer( QString type, unsigned long id ) const {
	QString key = QString("%1%2").arg( type ).arg( (ulong)id, (int)10, (int)10, QChar('0') );
	if( ! m_internalPointers.contains( key ) ) {
		struct indexInternalPointer * p = new struct indexInternalPointer;
		m_internalPointers.insert( key, p );

		p->id = id;
		p->is_category = ( type == "C" );
	}
	return m_internalPointers.value( key );
}

QModelIndex SnipetItemModel::index( int row, int column, const QModelIndex & parent ) const {
	if( ( column < 0 ) || ( column > 3 ) ) return QModelIndex(); // Test supplementaire pour ModelTest
	if( row < 0 ) return QModelIndex();

	struct indexInternalPointer * p = 0;
	if( parent.isValid() && (p = static_cast<struct indexInternalPointer*>( parent.internalPointer() )) ) {
		// Si on est sur un snipet et non sur une catÈgorie, pas la peine d'aller plus loin : pas d'enfant.
		if( ! p->is_category ) return QModelIndex();

		m_listQuery.bindValue( ":id", QVariant::fromValue( p->id ) );
	} else {
		m_listQuery.bindValue( ":id", QVariant::fromValue( 0 ) );
	}
	if( ! m_listQuery.exec() ) {
		qWarning( qPrintable( m_listQuery.lastError().text() ) );
		return QModelIndex();
	}

	if( ! m_listQuery.seek( row ) ) {
		qWarning( qPrintable( m_listQuery.lastError().text() ) );
		return QModelIndex();
	}

	return createIndex( row, column,
						getInternalPointer(
								m_listQuery.value( list_category ).toString(),
								m_listQuery.value( list_id ).toInt()
						) );
}

QModelIndex SnipetItemModel::parent( const QModelIndex & index ) const {
	if( index.isValid() ) {
		struct indexInternalPointer * p = static_cast<struct indexInternalPointer*>( index.internalPointer() );
		if( p ) {
			unsigned int parentId;
			if( p->is_category ) {
				m_categoryQuery.bindValue( ":id", QVariant::fromValue( p->id ) );
				if( ! m_categoryQuery.exec() ) {
					qWarning( qPrintable( m_categoryQuery.lastError().text() ) );
					return QModelIndex();
				}
				m_categoryQuery.first();
				parentId = m_categoryQuery.value( category_parent ).toUInt();
			} else {
				m_snipetQuery.bindValue( ":id", QVariant::fromValue( p->id ) );
				if( ! m_snipetQuery.exec() ) {
					qWarning( qPrintable( m_snipetQuery.lastError().text() ) );
					return QModelIndex();
				}
				m_snipetQuery.first();
				parentId = m_snipetQuery.value( snipet_category ).toUInt();
			}

			if( parentId == 0 ) return QModelIndex();

			m_parentQuery.bindValue( ":id", QVariant::fromValue( parentId ) );
			m_parentQuery.exec();
			if( ! m_parentQuery.exec() ) {
				qWarning( qPrintable( m_parentQuery.lastError().text() ) );
				return QModelIndex();
			}

			m_parentQuery.first();

			unsigned long gparentId = m_parentQuery.value( parent_id ).toUInt();

			m_listQuery.bindValue( ":id", QVariant::fromValue( gparentId ) );
			m_listQuery.exec();
			if( ! m_listQuery.exec() ) {
				qWarning( qPrintable( m_listQuery.lastError().text() ) );
				return QModelIndex();
			}

			int row = 0;
			while( m_listQuery.next() ) {
				if( m_listQuery.value( list_id ).toUInt() == parentId ) break;
				row++;
			}

			return createIndex( row, 0, getInternalPointer( "C", parentId ) );
		}
	}
	return QModelIndex();
}

int SnipetItemModel::rowCount( const QModelIndex & index ) const {
	if( index.isValid() && ( index.column() != 0 ) ) return 0; // Column don't have children (Test suppl√©mentaire pour ModelTest)

	struct indexInternalPointer * p = 0;
	if( index.isValid() && (p = static_cast<struct indexInternalPointer*>( index.internalPointer() )) ) {
		// Si on est sur un snipet, pas d'enfant
		if( ! p->is_category ) return 0;

		m_listQuery.bindValue( ":id", QVariant::fromValue( p->id ) );
	} else {
		m_listQuery.bindValue( ":id", 0 );
	}
	if( ! m_listQuery.exec() ) {
		qWarning( qPrintable( m_listQuery.lastError().text() ) );
		return 0;
	}

	int sizeOfList = 0;
	while( m_listQuery.next() ) sizeOfList++;

	return sizeOfList;
}

int SnipetItemModel::columnCount( const QModelIndex & index ) const {
	if( index.isValid() && (index.internalId() == -1) ) return 1;
	return 3;
}

Qt::ItemFlags SnipetItemModel::flags( const QModelIndex & index ) const {
	struct indexInternalPointer * p = 0;
	if( index.isValid() && (p = static_cast<struct indexInternalPointer*>( index.internalPointer() )) && (!p->is_category) ) {
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

	struct indexInternalPointer * p = static_cast<struct indexInternalPointer*>( index.internalPointer() );
	if( p->is_category ) {
		/* Category */
		m_categoryQuery.bindValue( ":id", QVariant::fromValue( p->id ) );
		m_categoryQuery.exec();
		m_categoryQuery.first();

		if( role == Qt::DisplayRole ) {
			switch( index.column() ) {
			case 0:
				return m_categoryQuery.value( category_name );
			case 1:
				return QString(""); // Needed for Model tests
			case 2:
				return m_categoryQuery.value( category_description ); // Needed for Model tests
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
		/* Snipet */
		m_snipetQuery.bindValue( ":id", QVariant::fromValue( p->id ) );
		m_snipetQuery.exec();
		m_snipetQuery.first();

		if( role == Qt::DisplayRole )
			switch( index.column() ) {
			case 0:
				return m_snipetQuery.value( snipet_name );
			case 1:
				return m_snipetQuery.value( snipet_shortcut );
			case 2:
				return m_snipetQuery.value( snipet_description );
		} else if( role == Qt::DecorationRole ) {
			if( index.column() == 0 )
				return QIcon( m_snipetQuery.value( snipet_icon ).toString() );
		} else if( role == Qt::UserRole ) {
			if( index.column() == 0 )
				return QVariant::fromValue( m_snipetQuery.value( snipet_id ) );
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
