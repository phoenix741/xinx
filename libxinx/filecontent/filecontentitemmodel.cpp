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

#include "filecontent/filecontentitemmodel.h"

/* PrivateFileContentModel */

class PrivateFileContentModel {
public:
	FileContentElement * m_root;
};

/* FileContentItemModel */

FileContentItemModel::FileContentItemModel( FileContentElement * root, QObject *parent ) : QAbstractItemModel( parent ) {
	d = new PrivateFileContentModel();
	d->m_root = root;

	connect( d->m_root, SIGNAL(updated(FileContentElement*)), this, SLOT(update(FileContentElement*)) );
	connect( d->m_root, SIGNAL(aboutToRemove(FileContentElement*)), this, SLOT(beginRemoveRow(FileContentElement*)) );
	connect( d->m_root, SIGNAL(removed()), this, SLOT(endRemoveRow()) );
	connect( d->m_root, SIGNAL(aboutToAdd(FileContentElement*,int)), this, SLOT(beginInsertRow(FileContentElement*,int)) );
	connect( d->m_root, SIGNAL(added()), this, SLOT(endInsertRow()) );
}

FileContentItemModel::~FileContentItemModel() {
	d->m_root->disconnect( this );
	delete d;
}

QVariant FileContentItemModel::data( const QModelIndex &index, int role ) const {
	if( !index.isValid() )
		return QVariant();

	FileContentItemModel::struct_file_content data;
	FileContentElement * element = static_cast<FileContentElement*>( index.internalPointer() );

	switch( role ) {
	case Qt::DisplayRole:
		return element->displayName();
	case Qt::DecorationRole:
		return element->icon();
	case Qt::ToolTipRole:
		return element->displayTips();
	case Qt::UserRole:
		data.line = element->line();
		data.filename = element->filename();
		return QVariant::fromValue( data );
	default:
		return QVariant();
	}
}

Qt::ItemFlags FileContentItemModel::flags( const QModelIndex &index ) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex FileContentItemModel::index( int row, int column, const QModelIndex &parent ) const {
	FileContentElement * parentElement = d->m_root, * currentElement = NULL;
	if( parent.isValid() ) {
		parentElement = static_cast<FileContentElement*>( parent.internalPointer() );
	}

	currentElement = parentElement->element( row );

	if( currentElement )
		return createIndex( row, column, currentElement );
	else
		return QModelIndex();
}

QModelIndex FileContentItemModel::index( FileContentElement * element ) const {
	if( element == d->m_root )
		return QModelIndex();
	else
		return createIndex( element->row(), 0, element );
}

QModelIndex FileContentItemModel::parent( const QModelIndex &index ) const {
	if( !index.isValid() )
		return QModelIndex();

	FileContentElement * element = static_cast<FileContentElement*>( index.internalPointer() ),
					   * parent  = element->parent();

	if( ( parent == d->m_root ) || ( parent == NULL ) )
		return QModelIndex();

	return createIndex( parent->row(), 0, parent );
}

bool FileContentItemModel::hasChildren( const QModelIndex & parent ) const {
	FileContentElement * element;
	if( parent.isValid() )
		element = static_cast<FileContentElement*>( parent.internalPointer() );
	else
		element = d->m_root;
	FileContentParser * parser = dynamic_cast<FileContentParser*>( element );

	if( parser && (!parser->isLoaded()) )
		return true;
	else
		return QAbstractItemModel::hasChildren( parent );
}

int FileContentItemModel::rowCount( const QModelIndex &parent ) const {
	FileContentElement * element;
	if( parent.isValid() )
		element = static_cast<FileContentElement*>( parent.internalPointer() );
	else
		element = d->m_root;

	return element->rowCount();
}

QVariant FileContentItemModel::headerData( int section, Qt::Orientation orientation, int role ) const {
	if( ( orientation == Qt::Horizontal ) && ( role == Qt::DisplayRole ) && ( section == 0 ) )
		return tr("Name");

	return QVariant();
}

int FileContentItemModel::columnCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );


	return 1;
}

void FileContentItemModel::beginInsertRow( FileContentElement * element, int row ) {
	QModelIndex idx = index( element->parent() );
	beginInsertRows( idx, row, row );
}

void FileContentItemModel::endInsertRow() {
	endInsertRows();
}

void FileContentItemModel::beginRemoveRow( FileContentElement * element ) {
	QModelIndex idx = index( element->parent() );
	beginRemoveRows( idx, element->row(), element->row() );
}

void FileContentItemModel::endRemoveRow() {
	endRemoveRows();
}

void FileContentItemModel::update( FileContentElement * element ) {
	emit dataChanged( index( element ), index( element ) );
}
