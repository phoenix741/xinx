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
#include <QThread>
#include <QDebug>

/* Static */

QMultiHash<unsigned long, AbstractContentViewModel* > ContentViewNode::s_models;

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

ContentViewNode::ContentViewNode( const QString & name, int line, bool cachedNode ) : m_autoDelete( true ), m_cachedNode( cachedNode ), m_line( line ), m_testKeyData( ContentViewNode::NODE_ALL ) {
	m_datas.insert( ContentViewNode::NODE_NAME, name );
	m_datas.insert( ContentViewNode::NODE_DISPLAY_NAME, name );
}

ContentViewNode::~ContentViewNode() {
	if( m_referenceCounter > 0 )
		qWarning() << "Delete " << this << "(" << m_datas.values( ContentViewNode::NODE_NAME ) << ")" << " but reference counter is not null : " << m_referenceCounter;
}

void ContentViewNode::deleteInstance( unsigned long id ) {
	ContentViewNodeList childs = m_childs;
	foreach( ContentViewNode* node, childs ) {
		node->detach( this, id );
	}
	delete this;
}

bool ContentViewNode::attach( ContentViewNode * parent, unsigned long id ) {
	// In case where we are already attach to parent
	if( !parent || parent->m_childs.contains( this ) ) {
		return false;
	}

	// lock the model if possible
	QList<AbstractContentViewModel*> lockedModels = callModelsLock( id );

	// Search where inserting the node (sort by display name)
	ContentViewNodeList::iterator insertingRow = qLowerBound( parent->m_childs.begin(), parent->m_childs.end(), this, ContentViewNodeListSort );
	int rowForModel = insertingRow - parent->m_childs.begin();

	// If model, we alert it of inserting a row
	callModelBeginInsertRows( parent, rowForModel, lockedModels );

	// Set the filename
	if( m_filename.isEmpty() ) {
		m_filename = parent->m_filename;
	}

	// Insert this in line."insertingRow" of parent childs
	parent->m_childs.insert( insertingRow, this );

	// If model, we alert it of end
	callModelEndInsertRows( lockedModels );

	// Unlock models
	callModelsUnlock( lockedModels );

	// Incremente the counter
	m_referenceCounter.ref();

	return true;
}

void ContentViewNode::detach( ContentViewNode * parent, unsigned long id ) {
	Q_ASSERT( parent );

	// lock the model if possible
	QList<AbstractContentViewModel*> lockedModels = callModelsLock( id );

	// Search the line to remove
	int removingRow = parent->m_childs.indexOfObject( this );
	if( removingRow < 0 ) return;

	// If model, we alert it of removing row
	callModelBeginRemoveRows( parent, removingRow, removingRow, lockedModels );

	// Remove the row
	parent->m_childs.removeAt( removingRow );

	// If model, we alert it of end
	callModelEndRemoveRows( lockedModels );

	// Unlock models
	callModelsUnlock( lockedModels );

	// Decrement reference counter
	if( ( m_referenceCounter.deref() == 0 ) && m_autoDelete ) {
		deleteInstance( id );
	}
}

void ContentViewNode::addModel( AbstractContentViewModel * model, unsigned long id ) {
	Q_ASSERT( id );
	Q_ASSERT( model );

	if( ! s_models.contains( id, model ) )
		s_models.insert( id, model );
}

void ContentViewNode::removeModel( AbstractContentViewModel * model, unsigned long id ) {
	Q_ASSERT( id );
	Q_ASSERT( model );

	s_models.remove( id, model );
}

void ContentViewNode::clearModels( unsigned long id ) {
	Q_ASSERT( id );

	s_models.remove( id );
}

QList<AbstractContentViewModel*> ContentViewNode::models( unsigned long id ) {
	return s_models.values( id );
}

void ContentViewNode::removeAll( unsigned long id ) {
	if( m_childs.isEmpty() ) return;

	// lock the model if possible
	QList<AbstractContentViewModel*> lockedModels = callModelsLock( id );

	// If model, we alert it of removing row
	callModelBeginRemoveRows( this, 0, m_childs.count() - 1, lockedModels );

	// Remove all childs
	ContentViewNodeList childs = m_childs;
	m_childs.clear();
	foreach( ContentViewNode * node, childs ) {
		if( ( node->m_referenceCounter.deref() == 0 ) && node->m_autoDelete ) {
			node->deleteInstance( id );
		}
	}

	// If model, we alert it of end
	callModelEndRemoveRows( lockedModels );

	// Unlock models
	callModelsUnlock( lockedModels );
}

int ContentViewNode::line() const {
	return m_line;
}

void ContentViewNode::setLine( int value ) {

	if( m_line != value ) {
		QList<AbstractContentViewModel*> lockedModels = callModelsLock( 0 );

		m_line = value;
		callModelsDataChanged( lockedModels );

		callModelsUnlock( lockedModels );
	}
}

const QString & ContentViewNode::fileName() const {
	return m_filename;
}

void ContentViewNode::setFileName( const QString & value ) {
	if( m_filename != value ) {
		m_filename = value;
	}
}

QVariant ContentViewNode::data( int index ) const {
	return m_datas.value( index );
}

void ContentViewNode::setData( const QVariant & value, int index ) {
	if( m_datas.value( index ) != value ) {
		QList<AbstractContentViewModel*> lockedModels = callModelsLock( 0 );

		m_datas.insert( index, value );
		callModelsDataChanged( lockedModels );

		callModelsUnlock( lockedModels );
	}
}

bool ContentViewNode::isAutoDelete() {
	return m_autoDelete;
}

void ContentViewNode::setAutoDelete( bool value ) {
	if( m_autoDelete != value ) {
		m_autoDelete = value;

		if( m_autoDelete && ( m_referenceCounter == 0 ) )
			deleteInstance();
	}
}

const ContentViewNodeList & ContentViewNode::childs() const {
	return m_childs;
}

bool ContentViewNode::operator==( const ContentViewNode & node ) const {
	if( m_testKeyData == ContentViewNode::NODE_ALL ) {
		//	if( node.m_filename     != m_filename     ) return false;
		if( node.m_line         != m_line         ) return false;
		if( node.m_datas.size() != m_datas.size() ) return false;

		foreach( int index, m_datas.keys() ) {
			if( m_datas.value( index ) != node.m_datas.value( index ) )
				return false;
		}

		return true;
	} else {
		return m_datas.value( m_testKeyData ) == node.m_datas.value( m_testKeyData );
	}
}

ContentViewNode & ContentViewNode::operator=( const ContentViewNode & node ) {
	m_line     = node.m_line;
	m_filename = node.m_filename;
	m_datas    = node.m_datas;

	return *this;
}

void ContentViewNode::setCompareData( int index ) {
	m_testKeyData = index;
}


/*
 * Pour empecher les deadlocks, il y a deux possibilit� :
 * - R�ordonner les thread (avec un qSort par exemple)
 * - Quand ordonner les thread n'est pas possible, il faut en cas de conflit (tryLock)
 * eviter le conflit et d�bloquer tous les thread avant de recommancer :
 *
 *  for( int i = 0 ; i < models.size(); i++ ) {
 *    AbstractContentViewModel * model = models.at( i );
 *    if( ! model ) continue;
 *
 *    // Le but ici est d'empecher les dead lock. Si l'un des modeles est d�j� utilis� alors on ne le lock pas.
 *    // Ce point n'est peut-�tre pas necessaire car nous ordonons les models.
 *    if( ! model->mutex().tryLock() ) {
 *      qDebug() << QThread::currentThread() << " unlock " << result;
 *      foreach( AbstractContentViewModel * lockedModel, result ) {
 *        lockedModel->mutex().unlock();
 *      }
 *      result.clear();
 *      i = 0;
 *      continue;
 *    }
 *    result += model;
 *  }
 *
 */
QList<AbstractContentViewModel*> ContentViewNode::callModelsLock( unsigned long id ) {
	QList<AbstractContentViewModel*> result, models;
	if( s_models.isEmpty() ) return result;

	models = id ? s_models.values( id ) : s_models.values();
	qSort( models );
	models.removeAll( 0 );

	foreach( AbstractContentViewModel * model, models ) {
		model->mutex()->lock();
		result += model;
	}

  return result;
}

void ContentViewNode::callModelsUnlock( QList<AbstractContentViewModel*> models ) {
	foreach( AbstractContentViewModel * model, models ) {
    	model->mutex()->unlock();
	}
}

void ContentViewNode::callModelsDataChanged( QList<AbstractContentViewModel*> models ) {
	foreach( AbstractContentViewModel * model, models ) {
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

