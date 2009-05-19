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

// Qt header
#include <QImage>
#include <QMutexLocker>
#include <QStack>

/* ContentViewModel */

ContentViewModel::ContentViewModel( ContentViewNode * root, QObject *parent ) : AbstractContentViewModel( root, parent ), m_lastId( 1 ) {
	QMutexLocker locker( mutex() );
	m_idOfNode.insert( rootNode(), 0 );
	m_nodeOfId.insert( 0, rootNode() );
	addChildsOf( 0 );
}

ContentViewModel::~ContentViewModel() {

}

QVariant ContentViewModel::data( const QModelIndex &index, int role ) const {
	QMutexLocker locker( mutex() );
	if( ! index.isValid() || index.column() )
		return QVariant();

	ContentViewNode * item = index.isValid() ? m_nodeOfId.value( index.internalId() ) : rootNode();

	if( item ) {
		if( role == Qt::UserRole ) {
			ContentViewModel::struct_file_content data;
			data.line = item->line();
			data.filename = item->fileName();
			return QVariant::fromValue( data );
		} else if( role == Qt::DecorationRole ) {
			QImage image = QImage( item->data( ContentViewNode::NODE_ICON ).toString() );
			return image.scaled( QSize(16,16) );
		} else {
			return item->data( role );
		}
	}
	return QVariant();
}

Qt::ItemFlags ContentViewModel::flags( const QModelIndex &index ) const {
	if ( !index.isValid() )
		return 0;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QModelIndex ContentViewModel::index( int row, int column, const QModelIndex &parent ) const {
	QMutexLocker locker( mutex()  );
	if( column ) return QModelIndex();

	quint32 parentId = 0;
	if( parent.isValid() )
		parentId = parent.internalId();
	int size = m_childs.value( parentId ).size();

	if( ( row < 0 ) || ( row >= size ) ) {
		return QModelIndex();
	}

	quint32 childId = m_childs.value( parentId ).at( row );
	return createIndex( row, column, childId );
}

QModelIndex ContentViewModel::index( quint32 id ) const {
	QMutexLocker locker( mutex()  );
	if( id == 0 ) return QModelIndex();

	quint32 parentId = m_parents.value( id );
	return createIndex( m_childs.value( parentId ).indexOf( id ), 0, id );
}

QModelIndex ContentViewModel::index( ContentViewNode * node ) const {
	QMutexLocker locker( mutex()  );
	if( node == rootNode() ) return QModelIndex();

	quint32 id = m_idOfNode.value( node );
	quint32 parentId = m_parents.value( id );
	return createIndex( m_childs.value( parentId ).indexOf( id ), 0, id );
}

quint32 ContentViewModel::createId() {
	return m_lastId++;
}

QModelIndex ContentViewModel::parent( const QModelIndex & i ) const {
	QMutexLocker locker( mutex()  );
	if( ! i.isValid() )
		return QModelIndex();

	quint32 parentId = m_parents.value( i.internalId() );
	return index( parentId );
}

bool ContentViewModel::hasChildren( const QModelIndex & parent ) const {
	return rowCount( parent );
}

int ContentViewModel::rowCount( const QModelIndex &parent ) const {
	if( ! parent.isValid() ) return m_childs.value( 0 ).size();
	return m_childs.value( parent.internalId() ).size();
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

void ContentViewModel::nodeChanged( ContentViewNode * node ) {
	QList<quint32> ids = m_idOfNode.values( node );
	foreach( quint32 id, ids ) {
		QModelIndex i = index( id );
		emit dataChanged( i, i );
	}
}

void ContentViewModel::beginInsertNode( ContentViewNode * node, int first, int last ) {
	// Si le noeud nous concerne pas, on a rien a faire
	if( ! m_idOfNode.keys().contains( node ) ) return;

	struct tupleParentFisrtLast t = { node, first, last };
	m_queue.enqueue( t );
}

void ContentViewModel::addChildsOf( quint32 id ) {
	QStack< quint32 > stack;

	stack.push( id );
	while ( stack.count() ) {
		quint32 currentId  = stack.pop();

		ContentViewNode * currentNode = m_nodeOfId.value( currentId );

		// Process children
		//QList<quint32> childs = m_childs.value( currentId );
		foreach ( ContentViewNode * child, currentNode->childs() ) {
			quint32 childId = createId();
			m_idOfNode.insert( child, childId );
			m_nodeOfId.insert( childId, child );
			m_childs[currentId].append( childId );
			m_parents.insert( childId, currentId );

			stack.push( childId );
		}
		//m_childs.insert( currentId, childs );
	}
}

void ContentViewModel::endInsertNode() {
	while( m_queue.size() ) {
		struct tupleParentFisrtLast t = m_queue.dequeue();
		QList<quint32> ids = m_idOfNode.values( t.parent );

		foreach( quint32 id, ids ) {
			beginInsertRows( index( id ), t.first, t.last );

			//QList<quint32> childs = m_childs.value( id );
			for( int index = t.first; index <= t.last; index++ ) {
				ContentViewNode * node = t.parent->childs().at( index );
				quint32 newId          = createId();

				m_idOfNode.insert( node, newId );
				m_nodeOfId.insert( newId, node );
				m_childs[id].insert( index, newId );
				m_parents.insert( newId, id );
				addChildsOf( newId );
			}
			//m_childs.insert( id, childs );

			endInsertRows();
		}
	}
}

void ContentViewModel::beginRemoveNode( ContentViewNode * node, int first, int last ) {
	// Si le noeud nous concerne pas, on a rien a faire
	if( ! m_idOfNode.keys().contains( node ) ) return;

	QList<quint32> ids = m_idOfNode.values( node );

	foreach( quint32 id, ids ) {
		beginRemoveRows( index( id ), first, last );

		//QList<quint32> childs = m_childs.value( id );
		for( int index = last; index >= first; index-- ) {
			ContentViewNode * child = node->childs().at( index );
			QList<quint32> list = m_childs.value(id);
			if( index < list.size() ) {
				quint32 oldId           = list.at( index );

				removeChildsOf( oldId );
				m_idOfNode.remove( child, oldId );
				m_nodeOfId.remove( oldId );
				m_parents.remove( oldId );
				m_childs[id].removeAt( index );
			}
		}
		//m_childs.insert( id, childs );

		endRemoveRows();
	}
}

void ContentViewModel::removeChildsOf( quint32 id ) {
	QStack< quint32 > stack;

	stack.push( id );
	while ( stack.count() ) {
		quint32 currentId  = stack.pop();

		// Process children
		foreach ( quint32 childId, m_childs.value( currentId ) ) {
			ContentViewNode * child = m_nodeOfId.value( childId );

			m_idOfNode.remove( child, childId );
			m_nodeOfId.remove( childId );
			m_parents.remove( childId );

			stack.push( childId );
		}
		m_childs.remove( currentId );
	}
}

void ContentViewModel::endRemoveNode() {
}
