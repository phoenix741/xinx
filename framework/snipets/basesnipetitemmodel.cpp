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

/* BaseSnipetItemModel */

/*!
 * \class BaseSnipetItemModel
 * \brief The class select datas in the snipet Sqlite base and provide information in tree.
 *
 * This class select in the base all categories and all snipets in a QSqlQueryTable and transform
 * this source model to a tree model with the help of TreeProxyItemModel.
 *
 * This class can be subclassing to hide some column, and add other feature as
 * Drag & Drop.
 */

/*!
 * \brief Create a BaseSnipetItemModel
 *
 * Create a BaseSnipetItemModel, and load categories and snipets from the \e db base. For
 * this purpose, BaseSnipetItemModel create a QSqlQueryModel used as source model of a TreeProxyItemModel.
 * \param db Base where BaseSnipetItemModel must load the content of it's structure.
 * \param parent Parent object of the new instance of BaseSnipetItemModel.
 */
BaseSnipetItemModel::BaseSnipetItemModel( QSqlDatabase db, QObject * parent ) : TreeProxyItemModel( parent ), m_db( db ) {
	// This will be automatically deleted.
	m_sourceModel = new QSqlQueryModel( this );
	setSourceModel( m_sourceModel );
}

/// Destroy the BaseSnipetItemModel
BaseSnipetItemModel::~BaseSnipetItemModel() {

}

/*!
 * \brief Select all categories and snipets in the source model
 *
 * This method select all the categories and snipets from the source model
 * and re-create the internal structure of the TreeProxyItemModel by calling
 * createMapping()
 */
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

/// Source model (methode provide for convenience)
QSqlQueryModel * BaseSnipetItemModel::sourceModel() {
	return m_sourceModel;
}

/// Source model (methode provide for convenience)
QSqlQueryModel * BaseSnipetItemModel::sourceModel() const {
	return m_sourceModel;
}

/// Database where the model is connected (methode provide for convenience)
QSqlDatabase BaseSnipetItemModel::database() {
	return m_db;
}

/// Database where the model is connected (methode provide for convenience)
QSqlDatabase BaseSnipetItemModel::database() const {
	return m_db;
}

/*!
 * \brief Calculate the unique identifier for a type and an id.
 *
 * Calculate the unique identifier used in methode getUniqueIdentifier() and
 * getParentUniqueIdentifier().
 * For this, the method used the qHash methode on a string generated from the
 * type and the id.
 * \param type Type of line in the source model. The value can be \e CATEGORY  or \e SNIPET.
 * \param id the unique identifier in the snipet list or in the category list on the source model.
 * \return a unique identifier for the tree model
 * \sa getUniqueIdentifier(), getParentUniqueIdentifier()
 */
int BaseSnipetItemModel::getTreeModelIdentifier( QString type, int id ) const {
	if( type == "CATEGORY" ) {
		if( id == 0 ) return 0;
		return qHash( QString("C%1").arg( id ) );
	} else {
		return qHash( QString("S%1").arg( id ) );
	}
}

/*!
 * \brief Calculate the unique identifier for a given source index.
 *
 * To calculate the unique identifier, this method read the type and the
 * id columns for the selected row (ignoring sourceIndex.column()) and
 * calulate the unique identifier with getTreeModelIdentifier().
 * \param sourceIndex the index in the source model.
 * \return a unique identifier for the tree model
 * \sa getTreeModelIdentifier(), getParentUniqueIdentifier()
 */
int BaseSnipetItemModel::getUniqueIdentifier( const QModelIndex & sourceIndex ) const {
	QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
	QString type = record.value( list_type ).toString();
	int id       = record.value( list_id ).toInt();
	return getTreeModelIdentifier( type, id );
}

/*!
 * \brief Calculate the unique identifier for the parent of a given source index.
 *
 * To calculate the unique identifier, this method read the parent id of the selected row
 * (ignoring sourceIndex.column()) and calulate the unique identifier with getTreeModelIdentifier().
 * The type column is not used. For a category or a snipet, the parent is always a category.
 * \param sourceIndex the index in the source model.
 * \return a unique identifier for the parent of sourceIndex in the tree model
 * \sa getTreeModelIdentifier(), getUniqueIdentifier()
 */
int BaseSnipetItemModel::getParentUniqueIdentifier( const QModelIndex & sourceIndex ) const {
	QSqlRecord record = m_sourceModel->record( sourceIndex.row() );
	int parentId = record.value( list_parentid ).toInt();
	if( parentId == 0 ) return 0;

	return getTreeModelIdentifier( "CATEGORY", parentId );
}

/*!
 * \brief Calculate the tree QModelIndex for a given type and id
 *
 * This methode use TreeProxyItemModel::index(int) to calculate the tree
 * QModelIndex for a unique identifier. As getUniqueIdentifier(), this methode
 * use isCategory and id to calculate the tree unique identifier.
 * \param isCategory must be true, if the line search is a category, false for a snipet
 * \return An index in the tree model.
 * \sa getTreeModelIdentifier()
 */
QModelIndex BaseSnipetItemModel::index( bool isCategory, int id ) const {
	if( isCategory && ( id == 0 ) ) return QModelIndex();

	QString type = isCategory ? "CATEGORY" : "SNIPET";
	return TreeProxyItemModel::index( getTreeModelIdentifier( type, id ) );
}

/// Method provide for convenience (See QAbstractItemModel)
QModelIndex BaseSnipetItemModel::index( int row, int column, const QModelIndex & parent ) const {
	return TreeProxyItemModel::index( row, column, parent );
}

/*!
 * \brief Returns the data stored under the given role for the item referred to by the index.
 *
 * This method return more information that the default implementation in QAbstractItemModel.
 * \param role If the role have BaseSnipetItemModel::SnipetIdRole, data() returns the id of the snipet
			   or id of the category.
			   If the role have BaseSnipetItemModel::SnipetTypeRole, data() returns \e CATEGORY or
			   \e SNIPET to tell if the index is a Category or a Snipet.(String form)
			   If the role have BaseSnipetItemModel::SnipetParentIdRole, data() returns the parent
			   identifier of the index.
 * \param index The index where data must search information.
 * \return Information stored in a QVariant.
 */
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

/*!
 * \brief Return for a list of index, all the snipet and categories.
 *
 * For each index in the indexes list, this method store the id in he snipetIds list
 * or in the categoryIds list.
 * \param indexes The list of index to convert
 * \param snipetIds list of id passed by reference
 * \param categoryIds list of id passed by reference
 */
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

/*!
 * \internal
 * This method add in the ids list, all snipets of a category, and for the category
 * call it recursively to add all snipet of it's child category.
 */
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

/*!
 * \brief For a given list of index, return a list of snipets.
 *
 * This method return for a given list of index, all the snipet in the
 * category and in the child category. If the index refere to a snipet,
 * the snipet is directly added.
 * \param indexes List of index to convert
 * \return list of snipet identifier.
 */
QList<int> BaseSnipetItemModel::indexesToIds( const QModelIndexList & indexes ) const {
	QList<int> ids;
	foreach( const QModelIndex & index, indexes ) {
		addIndexToList( index, &ids );
	}

	qSort( ids );
	return ids;
}

