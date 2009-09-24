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
#include "recursivesortfilterproxymodel.h"

// Qt header
#include <QDebug>

/* RecursiveSortFilterProxyModel */

RecursiveSortFilterProxyModel::RecursiveSortFilterProxyModel( QObject * parent ) : QSortFilterProxyModel( parent ), m_showAllChild( true ) {

}

void RecursiveSortFilterProxyModel::setHidePath( const QStringList & hidePath ) {
	if( m_hidePath != hidePath ) {
		m_indexCache.clear();
		m_hidePath = hidePath;
	}
}

bool RecursiveSortFilterProxyModel::canBeShow( const QModelIndex & index ) const {
	if( ! index.isValid() ) return true;
	if( m_indexCache.contains( index ) ) return m_indexCache.value( index );

	bool show = false;
	QString data = sourceModel()->data( index ).toString();
	if( m_hidePath.contains( data ) ) {
		m_indexCache.insert( QPersistentModelIndex( index ), false );
		return false;
	}

	show = data.contains( filterRegExp() );

	if( ! show ) {
		int r = sourceModel()->rowCount( index );
		for( int i = 0; i < r; i++ ) {
			QModelIndex child = sourceModel()->index( i, filterKeyColumn(), index );
			if( canBeShow( child ) ) {
				m_indexCache.insert( QPersistentModelIndex( index ), true );
				return true;
			}
		}

		show = m_showAllChild && mustBeShow( index );
		m_indexCache.insert( QPersistentModelIndex( index ), show );
		return show;
	} else {
		m_indexCache.insert( QPersistentModelIndex( index ), true );
		return true;
	}
}

bool RecursiveSortFilterProxyModel::mustBeShow( const QModelIndex & index ) const {
	QModelIndex parent = index.parent();
	if( ! parent.isValid() ) return false;
	bool show = parent.data().toString().contains( filterRegExp() );
	return show || mustBeShow( parent );
}

bool RecursiveSortFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const {
	Q_ASSERT( filterKeyColumn() != -1 );

	QModelIndex index = sourceModel()->index( source_row, filterKeyColumn(), source_parent );

	return canBeShow( index );
}

bool RecursiveSortFilterProxyModel::showAllChild() const {
	return m_showAllChild;
}

void RecursiveSortFilterProxyModel::setShowAllChild( bool value ) {
	if( m_showAllChild != value ) {
		m_indexCache.clear();
		m_showAllChild = value;
	}
}

void RecursiveSortFilterProxyModel::setFilterRegExp( const QString & regExp ) {
	m_indexCache.clear();
	foreach( QModelIndex index , m_indexes ) {
		if( index.isValid() )
			m_indexCache.insert( index, true );
	}
	QSortFilterProxyModel::setFilterRegExp( regExp );
}

void RecursiveSortFilterProxyModel::setIncludeIndex( const QModelIndexList & indexes ) {
	m_indexes.clear();
	foreach( QModelIndex index , indexes ) {
		m_indexes.append( index );
	}
}

