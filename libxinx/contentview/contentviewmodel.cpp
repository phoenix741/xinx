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
#include "contentview/contentviewmodel.h"
#include "contentview/contentviewnode.h"

/* ContentViewModel */

ContentViewModel::ContentViewModel( ContentViewNode * root, QObject *parent ) : AbstractContentViewModel( root, parent ) {

}

ContentViewModel::~ContentViewModel() {

}

QVariant ContentViewModel::data( const QModelIndex &index, int role ) const {
	if( ! index.isValid() || index.column() )
		return QVariant();

	ContentViewNode * item = static_cast<ContentViewNode*>( index.internalPointer() );

	if( item ) {
		if( role == Qt::UserRole ) {
			ContentViewModel::struct_file_content data;
			data.line = item->line();
			data.filename = item->fileName();
			return QVariant::fromValue( data );
		} else
			return item->data( (ContentViewNode::RoleIndex)role );
	}
	return QVariant();
}

Qt::ItemFlags ContentViewModel::flags( const QModelIndex &index ) const {
	if ( !index.isValid() )
		return Qt::ItemIsEnabled;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QModelIndex ContentViewModel::index( int row, int column, const QModelIndex &parent ) const {
	if( column ) return QModelIndex();

	ContentViewNode * item = 0;
	if( parent.isValid() )
		item = static_cast<ContentViewNode*>( parent.internalPointer() );
	else
		item = rootNode();

	if( ( row < 0 ) || ( row >= item->childs().size() ) ) {
		return QModelIndex();
	}

	ContentViewNode * child = item->childs().at( row );
	return createIndex( row, column, child );
}

QModelIndex ContentViewModel::index( ContentViewNode * node ) const {
	if( node ) {
		ContentViewNode * parent = node->parent( (unsigned long)rootNode() );
		if( parent )
			return createIndex( parent->childs().indexOf( node ), 0, node );
	}
	return QModelIndex();
}

QModelIndex ContentViewModel::parent( const QModelIndex &index ) const {
	if( ! index.isValid() )
		return QModelIndex();

	ContentViewNode * item = static_cast<ContentViewNode*>( index.internalPointer() );
	if( !item )
		return QModelIndex();

	ContentViewNode * parent = item->parent( (unsigned long)rootNode() );
	if( !parent )
		return QModelIndex();

	ContentViewNode * parent2 = parent->parent( (unsigned long)rootNode() );
	if( !parent2 ) // In this case parent is rootNode()
		return QModelIndex();

	return createIndex( parent2->childs().indexOf( parent ), 0, parent );
}

bool ContentViewModel::hasChildren( const QModelIndex & parent ) const {
	return rowCount( parent );
}

int ContentViewModel::rowCount( const QModelIndex &parent ) const {
	if( ! parent.isValid() ) return rootNode()->childs().size();

	ContentViewNode * item = static_cast<ContentViewNode*>( parent.internalPointer() );

	return item ? item->childs().size() : 0;
}

QVariant ContentViewModel::headerData( int section, Qt::Orientation orientation, int role ) const {
	if( ( orientation == Qt::Horizontal ) && ( role == Qt::DisplayRole ) && ( section == 0 ) )
		return tr("Name");

	return QVariant();
}

int ContentViewModel::columnCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	return 1;
}

