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

BaseSnipetItemModel::BaseSnipetItemModel( QSqlDatabase db, QObject * parent ) : TreeProxyItemModel( parent ), m_db( db ) {
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel( this );
	setSourceModel( m_sourceModel );
}

BaseSnipetItemModel::~BaseSnipetItemModel() {

}

void BaseSnipetItemModel::select() {
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

QSqlQueryModel * BaseSnipetItemModel::sourceModel() {
	return m_sourceModel;
}

QSqlQueryModel * BaseSnipetItemModel::sourceModel() const {
	return m_sourceModel;
}

QSqlDatabase BaseSnipetItemModel::database() {
	return m_db;
}

QSqlDatabase BaseSnipetItemModel::database() const {
	return m_db;
}

int BaseSnipetItemModel::getTreeModelIdentifier( QString type, int id ) const {
	if( type == "CATEGORY" ) {
		if( id == 0 ) return 0;
		return qHash( QString("C%1").arg( id ) );
	} else {
		return qHash( QString("S%1").arg( id ) );
	}
}

int BaseSnipetItemModel::getUniqueIdentifier( const QModelIndex & sourceIndex ) const {
	QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
	QString type = record.value( list_type ).toString();
	int id       = record.value( list_id ).toInt();
	return getTreeModelIdentifier( type, id );
}

int BaseSnipetItemModel::getParentUniqueIdentifier( const QModelIndex & sourceIndex ) const {
	QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
	int parentId = record.value( list_parentid ).toInt();
	if( parentId == 0 ) return 0;

	return getTreeModelIdentifier( "CATEGORY", parentId );
}

QModelIndex BaseSnipetItemModel::index( bool isCategory, int id ) const {
	if( isCategory && ( id == 0 ) ) return QModelIndex();

	QString type = isCategory ? "CATEGORY" : "SNIPET";
	return TreeProxyItemModel::index( getTreeModelIdentifier( type, id ) );
}

QModelIndex BaseSnipetItemModel::index( int row, int column, const QModelIndex & parent ) const {
	return TreeProxyItemModel::index( row, column, parent );
}

QVariant BaseSnipetItemModel::data( const QModelIndex & index, int role ) const {
	if( ! index.isValid() ) return QVariant();


	if( role == BaseSnipetItemModel::SnipetIdRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_id );
	} else if( role == BaseSnipetItemModel::SnipetTypeRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_type );
	} else if( role == BaseSnipetItemModel::SnipetParentIdRole ) {
		QModelIndex sourceIndex = mapToSource( index );
		if( ! sourceIndex.isValid() ) return QVariant();

		QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
		return record.value( list_parentid );
	}

	return TreeProxyItemModel::data( index, role );
}

void BaseSnipetItemModel::indexesToIds( const QModelIndexList & indexes, QList<int> & snipetIds, QList<int> & categoryIds ) const {
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

void BaseSnipetItemModel::addIndexToList( QModelIndex index, QList<int> * ids ) const {
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

QList<int> BaseSnipetItemModel::indexesToIds( const QModelIndexList & indexes ) const {
	QList<int> ids;
	foreach( const QModelIndex & index, indexes ) {
		addIndexToList( index, &ids );
	}

	qSort( ids );
	return ids;
}

