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
#include <contentview/contentviewparser.h>
#include <contentview/contentviewcache.h>
#include <contentview/contentviewnode.h>
#include <core/xinxproject.h>

// Qt header
#include <QStack>
#include <QFile>
#include <QBuffer>
#include <QFileInfo>
#include <QDir>
#include <QImage>

/* ContentViewException */

ContentViewException::ContentViewException( QString message, QString filename, int line, int column )
: XinxException( QString("Error : %1 at line %2:%3 of file %4").arg( message ).arg( line ).arg( column ).arg( QFileInfo(filename).fileName() ) ), m_line( line ), m_column( column ), m_filename( filename ) {

}

int ContentViewException::getLine() const {
	return m_line;
}

int ContentViewException::getColumn() const {
	return m_column;
}

/* ContentViewParser */

ContentViewParser::ContentViewParser( bool autoDelete ) : m_autoDelete( autoDelete ), m_alreadyRunning( false ), m_rootNode( 0 ), m_device( 0 ), m_id( 0 ), m_decaledLine( 0 ) {
}

ContentViewParser::~ContentViewParser() {
	delete m_device;
}

void ContentViewParser::setAutoDelete( bool value ) {
	m_autoDelete = value;
}

bool ContentViewParser::isAutoDelete() const {
	return m_autoDelete;
}

void ContentViewParser::setAttachId( unsigned long value ) {
	m_id = value;
}

unsigned long ContentViewParser::attachId() {
	return m_id;
}

void ContentViewParser::setDecalage( int line ) {
	m_decaledLine = line;
}

int ContentViewParser::decalage() const {
	return m_decaledLine;
}

void ContentViewParser::loadAttachedNode( ContentViewNode * rootNode ) {
	if( ! rootNode ) return;
	foreach( ContentViewNode * n, rootNode->childs() ) {
		m_attachedNode.append( qMakePair( rootNode, n ) );
	}
}

void ContentViewParser::detachAttachedNode() {
	QPair<ContentViewNode*,ContentViewNode*> p;
	foreach( p, m_attachedNode ) {
		p.second->detach( p.first );
	}
	m_attachedNode.clear();
}

void ContentViewParser::removeAttachedNode( ContentViewNode * n ) {
	QMutableListIterator< QPair<ContentViewNode*,ContentViewNode*> > i( m_attachedNode );
	while( i.hasNext() ) {
		QPair<ContentViewNode*,ContentViewNode*> p = i.next();
		if( p.second == n ) {
			i.remove();
		}
	}
}

QString ContentViewParser::locationOf( const QString & relativeFilename ) {
	QStringList searchList;
	const QString name = filename();

	if( ! name.isEmpty() )
		searchList << QFileInfo( name ).absolutePath();

	if( XINXProjectManager::self()->project() )
		searchList += XINXProjectManager::self()->project()->processedSearchPathList();

	QString absPath = QString();
	bool finded = false;
	foreach( const QString & path, searchList ) {
		absPath = QDir( path ).absoluteFilePath( relativeFilename );
		if( QFile::exists( absPath ) ) {
			finded = true;
			break;
		}
	}

	if( finded )
		return absPath;

	return relativeFilename;
}

void ContentViewParser::createContentViewNode( ContentViewNode * parent, const QString & filename ) {
	// Declaration
	QString name = QFileInfo( filename ).fileName(), absFilename = locationOf( filename );
	m_imports.append( absFilename );

	if( ! rootNode() ) return;

	ContentViewNode * node = 0;

	// If cache (so if we have project opened)
	ContentViewCache * cache = XINXProjectManager::self()->project() ? XINXProjectManager::self()->project()->preloadedFilesCache() : 0;
	if( cache ) {
		// Get the node from the cache
		node = cache->contentOfFileName( absFilename );
	}

	// If parent have this node as child, remove all child from attached node.
	if( node ) {
		if( parent->childs().contains( node ) ) {
			removeAttachedNode( node );
		} else {
			// Else if node in cache, we attach the node
			node->attach( parent, m_id );
		}
	} else {
		ContentViewNode * node = new ContentViewNode( name, -1 ), * attachedNodeToKeep;
		node->setData( ":/images/import.png", ContentViewNode::NODE_ICON );

		if( ! node->attach( parent, m_id ) ) {
			attachedNodeToKeep = parent->childs().at( parent->childs().indexOf( node ) );
			node->deleteInstance();
			removeAttachedNode( attachedNodeToKeep );
		}
	}
	if( parent->childs().contains( node ) ) {
		removeAttachedNode( node );
	} else if( node ) {
		// Else if node in cache, we attach the node
		node->attach( parent, m_id );
	} else {
		ContentViewNode * node = new ContentViewNode( name, -1 );
		node->setData( ":/images/import.png", ContentViewNode::NODE_ICON );
		if( ! node->attach( parent, m_id ) ) node->deleteInstance();
	}
}

ContentViewNode * ContentViewParser::attachNode( ContentViewNode * parent, ContentViewNode * child ) {
	Q_ASSERT( parent );
	Q_ASSERT( child );

	ContentViewNode * result = child;
	child->setLine( child->line() + m_decaledLine );

	if( ! child->attach( parent, m_id ) ) {
		int index = parent->childs().indexOf( child );
		child->deleteInstance();
		result = index >= 0 ? parent->childs().at( index ) : 0;
	}
	removeAttachedNode( result );

	return result;
}

const QStringList & ContentViewParser::imports() const {
	return m_imports;
}

void ContentViewParser::loadFromContent( ContentViewNode * rootNode, const QByteArray & content ) {
	if( m_alreadyRunning )
		throw ContentViewException( tr("Parser already running"), filename(), 0, 0 );

	try {
		m_alreadyRunning = true;

		QByteArray contentArray = content;
		QBuffer buffer( &contentArray );
		buffer.open( QIODevice::ReadOnly );

		m_imports.clear();
		m_rootNode = rootNode;
		m_device   = &buffer;
		removeAttachedNodes();

		loadFromDeviceImpl();

		m_rootNode = 0;
		m_device   = 0;

		m_alreadyRunning = false;
		if( m_autoDelete ) delete this;
	} catch( ContentViewException e ) {
		m_rootNode = 0;
		m_device   = 0;

		m_alreadyRunning = false;

		if( m_autoDelete ) delete this;
		throw e;
	}
}

void ContentViewParser::loadFromFile( ContentViewNode * rootNode, const QString & filename ) {
	if( m_alreadyRunning )
		throw ContentViewException( tr("Parser already running"), filename, 0, 0 );

	try {
		m_alreadyRunning = true;
		QFile file( filename );

		// Open the file
		if (!file.open(QFile::ReadOnly))
			throw ContentViewException( tr("Parser already running"), filename, 0, 0 );

		m_imports.clear();
		m_rootNode = rootNode;
		m_device   = &file;
		m_filename = filename;
		if( rootNode )
			rootNode->setFileName( filename );
		removeAttachedNodes();

		loadFromDeviceImpl();

		m_rootNode = 0;
		m_device   = 0;
		m_filename = QString();

		m_alreadyRunning = false;
		if( m_autoDelete ) delete this;
	} catch( ContentViewException e ) {
		m_rootNode = 0;
		m_device   = 0;
		m_filename = QString();

		m_alreadyRunning = false;
		if( m_autoDelete ) delete this;

		throw e;
	}
}

void ContentViewParser::loadFromDevice( ContentViewNode * rootNode, QIODevice * device ) {
	if( m_alreadyRunning )
		throw ContentViewException( tr("Parser already running"), filename(), 0, 0 );

	try {
		m_alreadyRunning = true;
		m_rootNode = rootNode;
		m_device   = device;
		removeAttachedNodes();

		loadFromDeviceImpl();

		m_imports.clear();
		m_rootNode = 0;
		m_device   = 0;

		m_alreadyRunning = false;
		if( m_autoDelete ) delete this;
	} catch( ContentViewException e ) {
		m_rootNode = 0;
		m_device   = 0;

		m_alreadyRunning = false;
		if( m_autoDelete ) delete this;

		throw e;
	}
}

void ContentViewParser::loadFromMember() {
	if( m_alreadyRunning )
		throw ContentViewException( tr("Parser already running"), filename(), 0, 0 );

	try {
		m_imports.clear();
		m_alreadyRunning = true;
		removeAttachedNodes();

		loadFromDeviceImpl();

		m_alreadyRunning = false;
		if( m_autoDelete ) delete this;
	} catch( ContentViewException e ) {
		m_alreadyRunning = false;
		if( m_autoDelete ) delete this;

		throw e;
	}
}

void ContentViewParser::setRootNode( ContentViewNode * node ) {
	m_rootNode = node;
}

ContentViewNode * ContentViewParser::rootNode() const {
	return m_rootNode;
}

void ContentViewParser::setFilename( const QString & filename ) {
	QFile * file = new QFile( filename );

	// Open the file
	if( ! file->open(QFile::ReadOnly) ) {
		QString errorString = file->errorString();
		delete file;
		throw ContentViewException( tr("Cannot read file %1:%2.").arg( filename ).arg( errorString ), filename, 0, 0 );
	}

	delete m_device;
	if( m_rootNode )
		m_rootNode->setFileName( filename );
	m_filename = filename;
	m_device = file;
}

const QString & ContentViewParser::filename() const {
	if( rootNode() )
		return rootNode()->fileName();
	return m_filename;
}

void ContentViewParser::setInputDevice( QIODevice * device ) {
	m_device = device;
}

QIODevice * ContentViewParser::inputDevice() const {
	return m_device;
}
