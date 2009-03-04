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

ContentViewParser::ContentViewParser( bool autoDelete ) : m_autoDelete( autoDelete ), m_rootNode( 0 ), m_device( 0 ) {
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

QString ContentViewParser::locationOf( ContentViewNode * parent, const QString & filename ) {
	QStringList searchList;

	if( ! parent->fileName().isEmpty() )
		searchList << QFileInfo( parent->fileName() ).absolutePath();

	if( XINXProjectManager::self()->project() )
		searchList += XINXProjectManager::self()->project()->processedSearchPathList();

	QString absPath = QString();
	bool finded = false;
	foreach( const QString & path, searchList ) {
		absPath = QDir( path ).absoluteFilePath( filename );
		if( QFile::exists( absPath ) ) {
			finded = true;
			break;
		}
	}

	if( finded )
		return absPath;

	return filename;
}

void ContentViewParser::createContentViewNode( ContentViewNode * parent, const QString & filename ) {
	// Declaration
	QString name = QFileInfo( filename ).fileName(), absFilename;
	ContentViewNode * node = 0;
	ContentViewCache * cache = XINXProjectManager::self()->project() ? XINXProjectManager::self()->project()->preloadedFilesCache() : 0;

	// Search the location of the filename
	absFilename = locationOf( parent, filename );

	// If cache (so if we have project opened)
	if( cache ) {
		// Get the node from the cache
		node = cache->contentOfFileName( absFilename );
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

void ContentViewParser::loadFromContent( ContentViewNode * rootNode, const QString & content ) {
	QByteArray contentArray = content.toUtf8();
	QBuffer buffer( &contentArray );
	buffer.open( QIODevice::ReadOnly );

	m_rootNode = rootNode;
	m_device   = &buffer;

	loadFromDeviceImpl();

	m_rootNode = 0;
	m_device   = 0;

	if( m_autoDelete ) delete this;
}

void ContentViewParser::loadFromFile( ContentViewNode * rootNode, const QString & filename ) {
	QFile file( filename );

	// Open the file
	if (!file.open(QFile::ReadOnly))
		throw ContentViewException( QObject::tr("Cannot read file %1:%2.").arg(filename).arg(file.errorString()), 0, 0 );

	m_rootNode = rootNode;
	m_device   = &file;
	rootNode->setFileName( filename );

	loadFromDeviceImpl();

	m_rootNode = 0;
	m_device   = 0;

	if( m_autoDelete ) delete this;
}

void ContentViewParser::loadFromDevice( ContentViewNode * rootNode, QIODevice * device ) {
	m_rootNode = rootNode;
	m_device   = device;

	loadFromDeviceImpl();

	m_rootNode = 0;
	m_device   = 0;

	if( m_autoDelete ) delete this;
}

void ContentViewParser::loadFromMember() {
	loadFromDeviceImpl();

	if( m_autoDelete ) delete this;
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
		throw ContentViewException( QObject::tr("Cannot read file %1:%2.").arg( filename ).arg( errorString ), 0, 0 );
	}

	delete m_device;
	if( m_rootNode )
		m_rootNode->setFileName( filename );
	m_device = file;
	setInputDevice( file );
}

void ContentViewParser::setInputDevice( QIODevice * device ) {
	m_device = device;
}

QIODevice * ContentViewParser::inputDevice() const {
	return m_device;
}
