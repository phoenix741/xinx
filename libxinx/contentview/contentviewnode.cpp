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
#include "contentview/contentviewnode.h"
#include "contentview/abstractcontentviewmodel.h"

// Qt header
#include <QStack>

/* ContentViewNodeListSort */

bool ContentViewNodeListSort( ContentViewNode * d1, ContentViewNode * d2 ) {
	return d1->data( ContentViewNode::NODE_DISPLAY_NAME ).toString() < d2->data( ContentViewNode::NODE_DISPLAY_NAME ).toString();
}

/* ContentViewNode */

ContentViewNode::ContentViewNode( const QString & name, int line ) : m_canBeDeleted( true ), m_line( line ) {
	m_datas.insert( ContentViewNode::NODE_NAME, name );
}

ContentViewNode::~ContentViewNode() {
	detach();
}

void ContentViewNode::operator delete( void * ptr ) {
	ContentViewNode * node = static_cast<ContentViewNode*>( ptr );

	if( node && node->m_canBeDeleted )
		::operator delete( ptr );
}

void ContentViewNode::attach( ContentViewNode * parent, unsigned long id ) {
	// In case where we are already attach to parent
	if( !parent || parent->m_childs.contains( this ) ) {
		return;
	}

	// Search where inserting the node (sort by display name)
	QList<ContentViewNode*>::iterator insertingRow = qLowerBound( parent->m_childs.begin(), parent->m_childs.end(), this, ContentViewNodeListSort );
	int rowForModel = insertingRow - parent->m_childs.begin();

	// Set the model of the parent for this and all this child
	// If no id set, copy all models from parent to this and it childs.
	if( id ) {
		setModel( parent->model( id ), id );
	} else {
		setModels();
	}

	// If model, we alert it of inserting a row
	callModelBeginInsertRows( parent, rowForModel, id );

	// Insert this in line."insertingRow" of parent childs
	parent->m_childs.insert( insertingRow, this );

	// If model, we alert it of end
	callModelEndInsertRows( id );
}

void ContentViewNode::detach( unsigned long id ) {
	// Get the parent
	ContentViewNode * parent = m_parents.value( id );
	if( ! parent ) return;

	// Search the line to remove
	int removingRow = parent->m_childs.indexOf( this );
	if( removingRow < 0 ) return;

	// If model, we alert it of removing row
	callModelBeginRemoveRows( parent, removingRow, removingRow, id );

	// Remove the row
	parent->m_childs.removeAt( removingRow );
	m_parents.remove( id );

	// If model, we alert it of end
	callModelEndRemoveRows( id );

	// Remove the model for givent key
	if( id ) {
		setModel( 0, id );
	} else {
		clearModels();
	}
}

void ContentViewNode::detach() {
	// Detach all parents
	foreach( unsigned long id, m_parents.keys() ) {
		detach( id );
	}
}

void ContentViewNode::setModel( AbstractContentViewModel * model, unsigned long id ) {
	Q_ASSERT( id );

	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		if( node->m_models.value( id ) == model ) continue;

		// If model defined, we add it, else we remove it.
		if( model )
			node->m_models.insert( id, model );
		else
			node->m_models.remove( id );

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}

void ContentViewNode::setModels() {
	const QHash<unsigned long, AbstractContentViewModel* > & models = m_models;
	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		foreach( unsigned long id, models.keys() ) {
			node->m_models.insert( id, models.value( id ) );
		}

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}

void ContentViewNode::clearModels() {
	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		node->m_models.clear();

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}


AbstractContentViewModel * ContentViewNode::model( unsigned long id ) {
	return m_models.value( id );
}

void ContentViewNode::removeAll( unsigned long id ) {
	if( m_childs.isEmpty() ) return;

	// If model, we alert it of removing row
	callModelBeginRemoveRows( this, 0, m_childs.count() - 1, id );

	// Remove the row
	foreach( ContentViewNode * child, m_childs ) {
		child->m_parents.remove( id );
		child->m_models.remove( id );
	}
	m_childs.clear();

	// If model, we alert it of end
	callModelEndRemoveRows( id );
}

void ContentViewNode::clear() {
	QList<ContentViewNode*> list = m_childs;
	removeAll();
	qDeleteAll( list );
}

ContentViewNode * ContentViewNode::parent( unsigned long id ) const {
	if( m_parents.contains( id ) )
		return m_parents.value( id );
	else
		return m_parents.value( 0 );
}

int ContentViewNode::line() const {
	return m_line;
}

void ContentViewNode::setLine( int value ) {
	if( m_line != value ) {
		m_line = value;
		callModelsDataChanged();
	}
}

const QString & ContentViewNode::fileName() const {
	return m_filename;
}

void ContentViewNode::setFileName( const QString & value ) {
	if( m_filename != value ) {
		m_filename = value;
		callModelsDataChanged();
	}
}

QVariant ContentViewNode::data( enum RoleIndex index ) const {
	return m_datas.value( index );
}

void ContentViewNode::setData( const QVariant & value, enum RoleIndex index ) {
	if( m_datas.value( index ) != value ) {
		m_datas.insert( index, value );
		callModelsDataChanged();
	}
}

bool ContentViewNode::canBeDeleted() {
	return m_canBeDeleted;
}

void ContentViewNode::setCanBeDeleted( bool value ) {
	m_canBeDeleted = value;
}

const QList<ContentViewNode*> & ContentViewNode::childs() const {
	return m_childs;
}

bool ContentViewNode::operator==( const ContentViewNode & node ) const {
	if( node.m_datas.size() != m_datas.size() ) return false;

	foreach( enum ContentViewNode::RoleIndex index, m_datas.keys() ) {
		if( m_datas.value( index ) != node.m_datas.value( index ) )
			return false;
	}

	return true;
}

ContentViewNode & ContentViewNode::operator=( const ContentViewNode & node ) {
	m_line     = node.m_line;
	m_filename = node.m_filename;
	m_datas    = node.m_datas;

	return *this;
}

void ContentViewNode::callModelsDataChanged() {
	foreach( AbstractContentViewModel * model, m_models ) {
		if( ! model ) continue;
		model->nodeChanged( this );
	}
}

void ContentViewNode::callModelBeginInsertRows( ContentViewNode * node, int line, unsigned long id ) {
	AbstractContentViewModel * model = m_models.value( id );
	if( model ) {
		model->beginInsertNode( node, line, line );
	} else {
		foreach( AbstractContentViewModel * model, m_models ) {
			if( ! model ) continue;
			model->beginInsertNode( node, line, line );
		}
	}
}

void ContentViewNode::callModelEndInsertRows( unsigned long id ) {
	AbstractContentViewModel * model = m_models.value( id );
	if( model ) {
		model->endInsertNode();
	} else {
		foreach( AbstractContentViewModel * model, m_models ) {
			if( ! model ) continue;
			model->endInsertNode();
		}
	}
}

void ContentViewNode::callModelBeginRemoveRows( ContentViewNode * node, int firstLine, int lastLine, unsigned long id ) {
	AbstractContentViewModel * model = m_models.value( id );
	if( model ) {
		model->beginRemoveNode( node, firstLine, lastLine );
	} else {
		foreach( AbstractContentViewModel * model, m_models ) {
			if( ! model ) continue;
			model->beginRemoveNode( node, firstLine, lastLine );
		}
	}
}

void ContentViewNode::callModelEndRemoveRows( unsigned long id ) {
	AbstractContentViewModel * model = m_models.value( id );
	if( model ) {
		model->endRemoveNode();
	} else {
		foreach( AbstractContentViewModel * model, m_models ) {
			if( ! model ) continue;
			model->endRemoveNode();
		}
	}
}
