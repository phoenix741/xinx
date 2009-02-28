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

/* ContentViewException */

ContentViewException::ContentViewException( QString message, int line, int column )
: XinxException( QString("Error : %1 at line %2:%3").arg( message ).arg( line ).arg( column ) ), m_line( line ), m_column( column ) {

}

int ContentViewException::getLine() const {
	return m_line;
}

int ContentViewException::getColumn() const {
	return m_column;
}

/* ContentViewParser */

ContentViewParser::ContentViewParser( bool autoDelete ) : m_autoDelete( autoDelete ) {
}

ContentViewParser::~ContentViewParser() {
}

void ContentViewParser::setAutoDelete( bool value ) {
	m_autoDelete = value;
}

bool ContentViewParser::isAutoDelete() const {
	return m_autoDelete;
}

void ContentViewParser::loadAttachedNode( ContentViewNode * rootNode ) {
	foreach( ContentViewNode * n, rootNode->childs() ) {
		m_attachedNode.append( n );
	}
}

void ContentViewParser::detachAttachedNode() {
	foreach( ContentViewNode * n, m_attachedNode ) {
		n->detach();
		delete n;
	}
}

void ContentViewParser::createContentViewNode( ContentViewNode * parent, const QString & filename ) {
	// Declaration
	QString name = QFileInfo( filename ).fileName();
	ContentViewNode * node = 0;
	ContentViewCache * cache = XINXProjectManager::self()->project() ? XINXProjectManager::self()->project()->preloadedFilesCache() : 0;

	// If cache (so if we have project opened)
	if( cache ) {
		// Get the node from the cache
		node = cache->contentOfFileName( filename );
	}

	// If parent have this node as child, remove all child from attached node.
	if( parent->childs().contains( node ) ) {
		m_attachedNode.removeAll( node );
	} else if( node ) {
		// Else if node in cache, we attach the node
		node->attach( parent );
	} else {
		ContentViewNode * node = new ContentViewNode( name, -1 );
		node->attach( parent );
	}
}

bool ContentViewParser::loadFromContent( ContentViewNode * rootNode, const QString & content ) {
	bool result;
	QByteArray contentArray = content.toUtf8();
	QBuffer buffer( &contentArray );
	buffer.open( QIODevice::ReadOnly );

	result = loadFromDeviceImpl( rootNode, &buffer );

	if( m_autoDelete ) delete this;
	return result;
}

bool ContentViewParser::loadFromFile( ContentViewNode * rootNode, const QString & filename ) {
	bool result;
	QFile file( filename );

	// Open the file
	if (!file.open(QFile::ReadOnly))
		throw ContentViewException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	result = loadFromDeviceImpl( rootNode, & file );

	if( m_autoDelete ) delete this;
	return result;
}

bool ContentViewParser::loadFromDevice( ContentViewNode * rootNode, QIODevice * device ) {
	bool result = loadFromDeviceImpl( rootNode, device );

	if( m_autoDelete ) delete this;
	return result;
}
