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
#include "contentview/completionnodemodel.h"
#include "contentview/contentviewnode.h"

// Qt header
#include <QTimerEvent>
#include <QStack>
#include <QImage>

/* CompletionNodeModel */

CompletionNodeModel::CompletionNodeModel( ContentViewNode * root, QObject *parent ) : AbstractContentViewModel( root, parent ) {
	startTimer(0);
}

CompletionNodeModel::~CompletionNodeModel() {
}

void CompletionNodeModel::timerEvent( QTimerEvent * event ) {
	killTimer( event->timerId() );
	//	addAllNodes( rootNode() );
}

QVariant CompletionNodeModel::data( const QModelIndex &index, int role ) const {
	if( ! index.isValid() || index.column() )
		return QVariant();

	ContentViewNode * item = static_cast<ContentViewNode*>( index.internalPointer() );
	QImage image;

	if( item ) {
		switch( role ) {
		case Qt::DecorationRole:
			image = QImage( item->data( ContentViewNode::NODE_ICON ).toString() );
			return image.scaled( QSize(16,16) );
		case Qt::DisplayRole:
			return item->data( ContentViewNode::NODE_DISPLAY_NAME );
		case CompletionNodeModel::CompletionNodeName:
			return item->data( ContentViewNode::NODE_NAME );
		default:
			return item->data( (ContentViewNode::RoleIndex)role );
		}
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

bool CompletionNodeModel::mustElementBeShowed( ContentViewNode * n ) {
	Q_UNUSED( n );
	return true;
}

const QList<ContentViewNode*> & CompletionNodeModel::nodes() const {
	return m_nodes;
}

const QList<ContentViewNode*> & CompletionNodeModel::showedNodes() const {
	return m_showedNodes;
}

bool CompletionNodeModel::isElementShowed( ContentViewNode * n ) {
	return m_showedNodes.contains( n );
}

void CompletionNodeModel::showNode( ContentViewNode * n ) {
	if( m_showedNodes.contains( n ) ) return;

	QList<ContentViewNode*>::iterator insertingRow = qLowerBound( m_showedNodes.begin(), m_showedNodes.end(), n, ContentViewNodeListSort );
	int rowForModel = insertingRow - m_showedNodes.begin();

	beginInsertRows( QModelIndex(), rowForModel, rowForModel );
	m_showedNodes.insert( insertingRow, n );
	endInsertRows();
}

void CompletionNodeModel::hideNode( ContentViewNode * n ) {
	if( ! m_showedNodes.contains( n ) ) return;

	int removingRow = m_showedNodes.indexOf( n );
	if( removingRow < 0 ) return;

	beginRemoveRows( QModelIndex(), removingRow, removingRow );
	m_showedNodes.removeAt( removingRow );
	endRemoveRows();
}

void CompletionNodeModel::addAllNodes( ContentViewNode * n ) {
	Q_ASSERT( n );

	QStack< ContentViewNode* > stack;

	stack.push( n );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		addNode( node );

		// Process children
		foreach ( ContentViewNode * child, node->childs() )
			stack.push( child );
	}
}

void CompletionNodeModel::addNode( ContentViewNode * n ) {
	Q_ASSERT( n );
	if( m_nodes.contains( n ) ) {
		qDebug( qPrintable( QString("The node %1(%2) is already in the list").arg( (unsigned long)n, 0, 16 ).arg( n->data().toString() ) ) );
		return;
	}

	// Add the node to list
	QList<ContentViewNode*>::iterator insertingRow = qLowerBound( m_nodes.begin(), m_nodes.end(), n, ContentViewNodeListSort );
	m_nodes.insert( insertingRow, n );

	if( mustElementBeShowed( n ) ) {
		showNode( n );
	}
}

void CompletionNodeModel::removeAllNodes( ContentViewNode * n ) {
	Q_ASSERT( n );

	QStack< ContentViewNode* > stack;

	stack.push( n );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		removeNode( node );

		// Process children
		foreach ( ContentViewNode * child, node->childs() )
			stack.push( child );
	}
}

void CompletionNodeModel::removeNode( ContentViewNode * n ) {
	Q_ASSERT( n );

	// Remove the node to list
	hideNode( n );
	int removingRow = m_nodes.indexOf( n );
	m_nodes.removeAt( removingRow );
}

void CompletionNodeModel::beginInsertNode( ContentViewNode * n, int first, int last ) {
	m_parent = n;
	m_first  = first;
	m_last   = last;
}

void CompletionNodeModel::endInsertNode() {
	for( int i = m_first; i <= m_last; i++ ) {
		ContentViewNode * node = m_parent->childs().at( i );

		addAllNodes( node );
	}
}

void CompletionNodeModel::beginRemoveNode( ContentViewNode * n, int first, int last ) {
	m_parent = n;
	m_first  = first;
	m_last   = last;

	for( int i = m_first ; i <= m_last ; i++ ) {
		ContentViewNode * child = m_parent->childs().at( i );
		removeAllNodes( child );
	}
}

void CompletionNodeModel::endRemoveNode() {
}

void CompletionNodeModel::reset() {
	m_showedNodes.clear();
	AbstractContentViewModel::reset();
	foreach( ContentViewNode* node, m_nodes ) {
		if( mustElementBeShowed( node ) ) {
			m_showedNodes.append( node );
		}
	}
	AbstractContentViewModel::reset();
}

