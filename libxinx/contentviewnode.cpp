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
#include "contentviewnode.h"
#include "contentviewmodel.h"

/* ContentViewNodeListSort */

bool ContentViewNodeListSort( ContentViewNode * d1, ContentViewNode * d2 ) {
	return d1->data( ContentViewNode::NODE_DISPLAY_NAME ).toString() < d2->data( ContentViewNode::NODE_DISPLAY_NAME ).toString();
}

/* ContentViewNode */

ContentViewNode::ContentViewNode( const QString & name, int line ) : m_oldFlag( false ), m_line( line ) {
	m_datas.insert( ContentViewNode::NODE_NAME, name );
}

ContentViewNode::~ContentViewNode() {
	detach();
}

void ContentViewNode::attach( ContentViewNode * parent, unsigned long id ) {
	// In case where we are already attach to parent
	if( !parent || parent->m_childs.contains( this ) ) {
		int finded = parent->m_childs.indexOf( this );
		parent->m_childs.at( finded )->markAsRecent();
		return;
	}

	// Search where inserting the node (sort by display name)
	QList<ContentViewNode*>::iterator insertingRow = qLowerBound( parent->m_childs.begin(), parent->m_childs.end(), this, ContentViewNodeListSort );
	int rowForModel = insertingRow - parent->m_childs.begin();

	// If no model given we copy models from parent
	setModel( parent->model( id ), id );

	// If model, we alert it of inserting a row
	callModelBeginInsertRows( parent, rowForModel, id );

	parent->m_childs.insert( insertingRow, this );

	// If model, we alert it of end
	callModelEndInsertRows( id );
}

void ContentViewNode::detach( unsigned long id ) {
	// Get the parent
	ContentViewNode * parent = m_parents.value( id );
	if( ! parent ) return;

	// Get the model
	ContentViewModel * model = m_models.value( id );
	if( ! model ) return;

	// Search the line to remove
	int removingRow = parent->m_childs.indexOf( this );
	if( removingRow < 0 ) return;

}

void ContentViewNode::detach() {
	foreach( unsigned long id, m_parents.keys() ) {
		detach( id );
	}
}

void ContentViewNode::setModel( ContentViewModel * model, unsigned long id ) {
	Q_ASSERT( id );

	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		if( node->m_models.value( id ) == model ) continue;
		node->m_models.insert( id, model );

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}

ContentViewModel * ContentViewNode::model( unsigned long id ) {
	return m_models.value( id );
}

void ContentViewNode::removeAll() {

}

void ContentViewNode::removeAllOld() {

}

void ContentViewNode::clear() {

}

void ContentViewNode::clearOld() {

}

void ContentViewNode::markAllAsOld() {
	m_oldFlag = true;
}

void ContentViewNode::markAsRecent() {
	m_oldFlag = false;
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
	foreach( ContentViewModel * model, m_models ) {
		if( ! model ) continue;
		model->callDataChanged( model->index( this ), model->index( this ) );
	}
}

void ContentViewNode::callModelBeginInsertRows( ContentViewNode * node, int line, unsigned long id ) {
	ContentViewModel * model = m_models.value( id );
	if( model ) {
		model->beginInsertRows( model->index( node ), line, line );
	} else {
		foreach( ContentViewModel * model, m_models ) {
			if( ! model ) continue;
			model->beginInsertRows( model->index( node ), line, line );
		}
	}
}

void ContentViewNode::callModelEndInsertRows( unsigned long id ) {
	ContentViewModel * model = m_models.value( id );
	if( model ) {
		model->endInsertRows();
	} else {
		foreach( ContentViewModel * model, m_models ) {
			if( ! model ) continue;
			model->endInsertRows();
		}
	}
}
