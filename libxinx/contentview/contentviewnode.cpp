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
#include <QFileInfo>

/* Static */

/* ContentViewNodeListSort */

bool ContentViewNodeListSort( ContentViewNode * d1, ContentViewNode * d2 ) {
	return d1->data( ContentViewNode::NODE_DISPLAY_NAME ).toString().compare( d2->data( ContentViewNode::NODE_DISPLAY_NAME ).toString(), Qt::CaseInsensitive ) < 0;
}

/* ContentViewNodeList */

int ContentViewNodeList::indexOf( ContentViewNode* t, int from ) const {
	if(from < 0)
		from = qMax( from + size(), 0 );

	if( from < size() ) {
		for( int i = from; i < size(); i++ ) {
			ContentViewNode * node = at( i );
			if( t->operator==( *node ) ) {
				return i;
			}
		}
	}

	return -1;
}

int ContentViewNodeList::indexOfObject( ContentViewNode* t, int from ) const {
	return QList<ContentViewNode*>::indexOf( t, from );
}

int ContentViewNodeList::lastIndexOf( ContentViewNode* t, int from ) const {
	if( from < 0 )
		from += size();
	else if( from >= size() )
		from = size() - 1;

	if( from >= 0 ) {
		ContentViewNodeList::const_iterator i = end() + from + 1;

		for(; i != this->begin() ; --i ) {
			if( t->operator==( **i ) ) {
				return i - begin();
			}
		}
	}
	return -1;
}

bool ContentViewNodeList::contains( ContentViewNode* value ) const {
	ContentViewNodeList::const_iterator i = begin();

	for(; i != this->end() ; ++i ) {
		if( ( value && ( value->operator==( **i ) ) ) || ( ( 0 == value ) && ( 0 == *i ) ) ) {
			return true;
		}
	}
	return false;
}

int ContentViewNodeList::count( ContentViewNode* value ) const {
	int c = 0;
	ContentViewNodeList::const_iterator i = begin();

	for(; i != this->end() ; ++i ) {
		if( value->operator==( **i ) ) {
			++c;
		}
	}

	return c;
}

int ContentViewNodeList::count() const {
	return QList<ContentViewNode*>::count();
}


/* ContentViewNode */

ContentViewNode::ContentViewNode( const QString & name, int line, bool cachedNode ) : m_canBeDeleted( true ), m_cachedNode( cachedNode ), m_line( line ) {
	m_datas.insert( ContentViewNode::NODE_NAME, name );
	m_datas.insert( ContentViewNode::NODE_DISPLAY_NAME, name );
}

ContentViewNode::~ContentViewNode() {
	detach();
}

void ContentViewNode::deleteInstance( unsigned long id ) {
	if( m_canBeDeleted ) {
		ContentViewNodeList childs = m_childs;
		foreach( ContentViewNode* node, childs ) {
			node->detach( id );
			node->deleteInstance( id );
		}
		delete this;
	}
}

bool ContentViewNode::attach( ContentViewNode * parent, unsigned long id ) {
	// In case where we are already attach to parent
	if( !parent || parent->m_childs.contains( this ) ) {
		return false;
	}
	Q_ASSERT( ! m_parents.value( id ) );

	// Set the model of the parent for this and all this child
	// If no id set, copy all models from parent to this and it childs.
	propagateModels( parent, id );

	// lock the model if possible
	QList<AbstractContentViewModel*> lockedModels = callModelsLock( id );

	// Search where inserting the node (sort by display name)
	ContentViewNodeList::iterator insertingRow = qLowerBound( parent->m_childs.begin(), parent->m_childs.end(), this, ContentViewNodeListSort );
	int rowForModel = insertingRow - parent->m_childs.begin();

	// Set the parent or quit if already defined.
	if( m_cachedNode )
		m_parents.insert( id, parent );
	else
		m_parents.insert( 0, parent );

	// If model, we alert it of inserting a row
	callModelBeginInsertRows( parent, rowForModel, lockedModels );

	// Insert this in line."insertingRow" of parent childs
	parent->m_childs.insert( insertingRow, this );

	// If model, we alert it of end
	callModelEndInsertRows( lockedModels );

	// Unlock models
	callModelsUnlock( lockedModels );

	return true;
}

void ContentViewNode::detach( unsigned long id ) {
	// Get the parent
	ContentViewNode * parent = m_cachedNode ? m_parents.value( id ) : m_parents.value( 0 );
	if( ! parent ) return;

	// lock the model if possible
	QList<AbstractContentViewModel*> lockedModels = callModelsLock( id );

	// Search the line to remove
	int removingRow = parent->m_childs.indexOf( this );
	if( removingRow < 0 ) return;

	// If model, we alert it of removing row
	callModelBeginRemoveRows( parent, removingRow, removingRow, lockedModels );

	// Remove the row
	parent->m_childs.removeAt( removingRow );
	if( m_cachedNode )
		m_parents.remove( id );
	else
		m_parents.remove( 0 );

	// If model, we alert it of end
	callModelEndRemoveRows( lockedModels );

	// Unlock models
	callModelsUnlock( lockedModels );

	// Remove the model for givent key
	clearModels( id );
}

void ContentViewNode::detach() {
	// Detach all parents
	foreach( unsigned long id, m_parents.keys() ) {
		detach( id );
	}
}

void ContentViewNode::addModel( AbstractContentViewModel * model, unsigned long id ) {
	Q_ASSERT( id );
	Q_ASSERT( model );

	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		if( ! node->m_models.contains( id, model ) )
			node->m_models.insert( id, model );

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}

void ContentViewNode::removeModel( AbstractContentViewModel * model, unsigned long id ) {
	Q_ASSERT( id );
	Q_ASSERT( model );

	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		node->m_models.remove( id, model );

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}

void ContentViewNode::clearModels( unsigned long id ) {
	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		if( id == 0 ) {
			node->m_models.clear();
		} else {
			node->m_models.remove( id );
		}

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}

void ContentViewNode::propagateModels( ContentViewNode * n, unsigned long id ) {
	QStack< ContentViewNode* > stack;

	stack.push( this );
	while ( stack.count() ) {
		ContentViewNode * node  = stack.pop();

		if( id == 0 ) {
			QHashIterator<unsigned long, AbstractContentViewModel*> i( n->m_models );
			while( i.hasNext() ) {
				i.next();

				if( ! node->m_models.contains( i.key(), i.value() ) )
					node->m_models.insert( i.key(), i.value() );
			}
		} else {
			QListIterator<AbstractContentViewModel*> i( n->m_models.values( id ) );
			while( i.hasNext() ) {
				AbstractContentViewModel * m = i.next();

				if( ! node->m_models.contains( id, m ) )
					node->m_models.insert( id, m );
			}
		}

		foreach ( ContentViewNode * child, node->m_childs )
			stack.push( child );
	}
}

QList<AbstractContentViewModel*> ContentViewNode::models( unsigned long id ) {
	return m_models.values( id );
}

void ContentViewNode::removeAll( unsigned long id ) {
	if( m_childs.isEmpty() ) return;

	// lock the model if possible
	QList<AbstractContentViewModel*> lockedModels = callModelsLock( id );

	// If model, we alert it of removing row
	callModelBeginRemoveRows( this, 0, m_childs.count() - 1, lockedModels );

	// Remove the row
	foreach( ContentViewNode * child, m_childs ) {
		child->m_parents.remove( id );
		child->m_models.remove( id );
	}
	m_childs.clear();

	// If model, we alert it of end
	callModelEndRemoveRows( lockedModels );

	// Unlock models
	callModelsUnlock( lockedModels );
}

void ContentViewNode::clear() {
	ContentViewNodeList list = m_childs;
	removeAll();
	foreach( ContentViewNode * c, m_childs )
		c->deleteInstance();
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
	if( m_filename.isEmpty() ) {
		ContentViewNode * parent = m_parents.value( 0 ); // Parent pour l'id 0 : pas besoin de se compliquer plus la vie
		if( parent )
			return parent->fileName();
	}
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

const ContentViewNodeList & ContentViewNode::childs() const {
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

QList<AbstractContentViewModel*> ContentViewNode::callModelsLock( unsigned long id ) {
	QList<AbstractContentViewModel*> result;
	if( m_models.isEmpty() ) return result;

	if( id ) {
		QList<AbstractContentViewModel *> models = m_models.values( id );
		foreach( AbstractContentViewModel * model, models ) {
			if( model ) {
        qDebug() << QThread::currentThread() << " lock model " << model;
				model->mutex().lock();
				result += model;
			}
		}
	} else {
		foreach( AbstractContentViewModel * model, m_models.values() ) {
			if( ! model ) continue;
      qDebug() << QThread::currentThread() << " lock model " << model;
      model->mutex().lock();
			result += model;
		}
	}
	return result;
}

void ContentViewNode::callModelsUnlock( QList<AbstractContentViewModel*> models ) {
	foreach( AbstractContentViewModel * model, models ) {
    qDebug() << QThread::currentThread() << " unlock model " << model;
    model->mutex().unlock();
	}
}

void ContentViewNode::callModelsDataChanged() {
	foreach( AbstractContentViewModel * model, m_models ) {
		if( ! model ) continue;
		model->nodeChanged( this );
	}
}

void ContentViewNode::callModelBeginInsertRows( ContentViewNode * node, int line, QList<AbstractContentViewModel*> models ) {
	foreach( AbstractContentViewModel * model, models ) {
		if( ! model ) continue;
		model->beginInsertNode( node, line, line );
	}
}

void ContentViewNode::callModelEndInsertRows( QList<AbstractContentViewModel*> models ) {
	foreach( AbstractContentViewModel * model, models ) {
		if( ! model ) continue;
		model->endInsertNode();
	}
}

void ContentViewNode::callModelBeginRemoveRows( ContentViewNode * node, int firstLine, int lastLine, QList<AbstractContentViewModel*> models ) {
	foreach( AbstractContentViewModel * model, models ) {
		if( ! model ) continue;
		model->beginRemoveNode( node, firstLine, lastLine );
	}
}

void ContentViewNode::callModelEndRemoveRows( QList<AbstractContentViewModel*> models ) {
	foreach( AbstractContentViewModel * model, models ) {
		if( ! model ) continue;
		model->endRemoveNode();
	}
}

