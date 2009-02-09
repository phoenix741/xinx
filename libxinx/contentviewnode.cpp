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

ContentViewNode::ContentViewNode( const QString & name, int line ) : m_line( line ), m_oldFlag( false ) {
	m_datas.insert( ContentViewNode::NODE_NAME, name );
}

ContentViewNode::~ContentViewNode() {
	detach();
}

void ContentViewNode::attach( ContentViewNode * parent, ContentViewModel * model ) {
	if( !parent || parent->m_childs.contains( this ) ) {
		int finded = parent->m_childs.indexOf( this );
		parent->m_childs.at( finded )->markAsRecent();
		return;
	}

	// In case where we are already attach to parent
	detach( parent );

	// Search where inserting the node (sort by display name)
	QList<ContentViewNode*>::iterator insertingRow = qLowerBound( parent->m_childs.begin(), parent->m_childs.end(), this, ContentViewNodeListSort );
	int rowForModel = insertingRow - parent->m_childs.begin();

	// If no model given we copy models from parent


	// If model, we alert it of inserting a row
	callModelBeginInsertRows( parent, rowForModel, model );

	// Update the parent in the node and all it's child
	m_parent.insert( model, parent );
	if ( m_parent.keys().contains( model ) ) {
		QStack< QPair< ContentViewNode*,ContentViewNode*> > stack;

		stack.push( qMakePair( parent, this ) );
		while ( stack.count() ) {
			QPair<ContentViewNode *, ContentViewNode *> node  = stack.pop();

			node.second->m_parent.insert( model, node.first );

			foreach ( ContentViewNode * child, node.second->m_childs )
				stack.push( qMakePair( node.second, child ) );
		}
	}

	parent->m_childs.insert( insertingRow, this );

	// If model, we alert it of end
	callModelEndInsertRows( model );
}

void ContentViewNode::detach( ContentViewNode * parent ) {
	if( ! m_parent.contains( parent ) ) return;

	int removingRow = parent->m_childs.indexOf( this );
	if( removingRow < 0 ) return;

	ContentViewModel * model = m_parent.key( parent );
	if( ! model ) return;
}

void ContentViewNode::detach() {
	foreach( ContentViewNode * parent, m_parent ) {
		detach( parent );
	}
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

ContentViewNode * ContentViewNode::parent( ContentViewModel * model ) const {
	if( m_parent.contains( model ) )
		return m_parent.value( model );
	else
		return m_parent.value( 0, 0 );
}

int ContentViewNode::line() const {
	return m_line;
}

void ContentViewNode::setLine( int line ) {
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
}

void ContentViewNode::callModelsDataChanged() {
	foreach( ContentViewModel * model, m_parent.keys() ) {
		if( ! model ) continue;
		model->callDataChanged( index( this ), index( this ) );
	}
}

void ContentViewNode::callModelBeginInsertRows( ContentViewNode * node, int line, ContentViewModel * model ) {
	if( model ) {
		model->beginInsertRows( model->index( node ), line, line );
	} else {
		foreach( ContentViewModel * model, m_parent.keys() ) {
			if( ! model ) continue;
			model->beginInsertRows( index( node ), line, line );
		}
	}
}

void ContentViewNode::callModelEndInsertRows( ContentViewModel * model ) {
	if( model ) {
		model->endInsertRows();
	} else {
		foreach( ContentViewModel * model, m_parent.keys() ) {
			if( ! model ) continue;
			model->endInsertRows();
		}
	}
}
