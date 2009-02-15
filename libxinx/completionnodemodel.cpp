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
#include "completionnodemodel.h"
#include "contentviewnode.h"

/* CompletionNodeModel */

CompletionNodeModel::CompletionNodeModel( ContentViewNode * root, QObject *parent ) : AbstractContentViewModel( root, parent ) {
}

CompletionNodeModel::~CompletionNodeModel() {
}

QVariant CompletionNodeModel::data( const QModelIndex &index, int role ) const {
	if( ! index.isValid() || index.column() )
		return QVariant();

	ContentViewNode * item = static_cast<ContentViewNode*>( index.internalPointer() );

	if( item ) {
		return item->data( (ContentViewNode::RoleIndex)role );
	}

	return QVariant();
}

Qt::ItemFlags CompletionNodeModel::flags( const QModelIndex &index ) const {
	if ( !index.isValid() )
		return Qt::ItemIsEnabled;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QModelIndex CompletionNodeModel::index( int row, int column, const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	if( column ) return QModelIndex();
	if( ( row < 0 ) || ( row >= m_showedNodes.size() ) ) return QModelIndex();

	ContentViewNode * item = m_showedNodes.at( row );

	return createIndex( row, column, item );
}

QModelIndex CompletionNodeModel::index( ContentViewNode * node ) const {
	if( ! m_showedNodes.contains( node ) ) return QModelIndex();

	return createIndex( m_showedNodes.indexOf( node ), 0, node );
}

QModelIndex CompletionNodeModel::parent( const QModelIndex &index ) const {
	Q_UNUSED( index );
	return QModelIndex();
}

bool CompletionNodeModel::hasChildren( const QModelIndex & parent ) const {
	Q_UNUSED( parent );
	return false;
}

int CompletionNodeModel::rowCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );

	return m_showedNodes.size();
}

QVariant CompletionNodeModel::headerData( int section, Qt::Orientation orientation, int role ) const {
	if( ( orientation == Qt::Horizontal ) && ( role == Qt::DisplayRole ) && ( section == 0 ) )
		return tr("Name");

	return QVariant();
}

int CompletionNodeModel::columnCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	return 1;
}

bool CompletionNodeModel::mustElementBeShowed( ContentViewNode * node ) {
	Q_UNUSED( node );
	return true;
}

QList<ContentViewNode*> & CompletionNodeModel::nodes() {
	return m_nodes;
}

QList<ContentViewNode*> & CompletionNodeModel::showedNodes() {
	return m_showedNodes;
}

bool CompletionNodeModel::isElementShowed( ContentViewNode * node ) {
	return m_showedNodes.contains( node );
}

void CompletionNodeModel::showNode( ContentViewNode * node ) {
	if( m_showedNodes.contains( node ) ) return;

	QList<ContentViewNode*>::iterator insertingRow = qLowerBound( m_showedNodes.begin(), m_showedNodes.end(), node, ContentViewNodeListSort );
	int rowForModel = insertingRow - m_showedNodes.begin();

	beginInsertRows( QModelIndex(), rowForModel, rowForModel );
	m_showedNodes.insert( insertingRow, node );
	endInsertRows();
}

void CompletionNodeModel::hideNode( ContentViewNode * node ) {
	if( ! m_showedNodes.contains( node ) ) return;

	int removingRow = m_showedNodes.indexOf( node );
	if( removingRow < 0 ) return;

	beginRemoveRows( QModelIndex(), removingRow, removingRow );
	m_showedNodes.removeAt( removingRow );
	endRemoveRows();
}

void CompletionNodeModel::beginInsertNode( ContentViewNode * node, int first, int last ) {
	m_parent = node;
	m_first  = first;
	m_last   = last;
}

void CompletionNodeModel::endInsertNode() {
	for( int i = m_first; i <= m_last; i++ ) {
		ContentViewNode * node = m_parent->childs().at( i );

		QList<ContentViewNode*>::iterator insertingRow = qLowerBound( m_nodes.begin(), m_nodes.end(), node, ContentViewNodeListSort );
		m_nodes.insert( insertingRow, node );

		if( mustElementBeShowed( node ) ) {
			showNode( node );
		}
	}
}

void CompletionNodeModel::beginRemoveNode( ContentViewNode * node, int first, int last ) {
	m_parent = node;
	m_first  = first;
	m_last   = last;

	for( int i = m_first ; i <= m_last ; i++ ) {
		ContentViewNode * child = m_parent->childs().at( i );

		hideNode( child );
		int removingRow = m_nodes.indexOf( child );
		m_nodes.removeAt( removingRow );
	}
}

void CompletionNodeModel::endRemoveNode() {
}
