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
#include "core/xinxproject.h"

#include "contentview/contentviewcache.h"
#include "contentview/contentviewparser.h"
#include "contentview/contentviewnode.h"

#include "plugins/xinxpluginsloader.h"

// Qt header
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QFileInfo>
#include <QIcon>
#include <QVariant>

/* Static function */

void parserLoading( ContentViewParser * parser ) {
	try {
		parser->loadFromMember();
	} catch( ContentViewException e ) {
		qWarning( qPrintable( e.getMessage() ) );
	}
}

ContentViewNode* fileLoading( const QString & filename ) {
	ContentViewParser * parser = 0;
	ContentViewNode * node = new ContentViewNode( filename, -1, true );

	node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
	node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );

	IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
	if( fileType && ( parser = fileType->createParser() ) ) {
		try {
			node->setCanBeDeleted( false );

			parser->setRootNode( node );
			parser->setFilename( filename );

			node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );

			parser->loadFromMember();

			delete parser;
		} catch( ContentViewException e ) {
			delete parser; parser = 0;
			node->deleteInstance(); node = 0;
			qWarning( qPrintable( e.getMessage() ) );
		}
	} else {
		node->deleteInstance(); node = 0;
	}

	return node;
}

/* ContentViewCache */

ContentViewCache::ContentViewCache( XinxProject * project ) : QObject( project ), m_project( project ) {
	m_watcher = new QFutureWatcher<ContentViewNode*> ( this );
	connect( m_watcher, SIGNAL(resultReadyAt(int)), this, SLOT(resultReadyAt(int)) );
	startTimer( 500 );
}

ContentViewCache::~ContentViewCache() {
	if( m_watcher->isRunning() ) {
		m_watcher->waitForFinished();
	}
	delete m_watcher;

	QList<ContentViewNode*> nodes = m_nodes.values();
	foreach( ContentViewNode * node, nodes ) {
		node->setCanBeDeleted( true );
		node->deleteInstance();
	}
}

void ContentViewCache::initializeCache() {
	if( m_watcher->isRunning() ) {
		m_watcher->waitForFinished();
	}

	QVector<QString> filenames;
	foreach( QString name, m_project->preloadedFiles() ) {
		QString filename = QDir( m_project->projectPath() ).absoluteFilePath( name );
		filenames << filename;
	}
	m_watcher->setFuture( QtConcurrent::mapped( filenames, fileLoading ) );
}

void ContentViewCache::resultReadyAt( int index ) {
	ContentViewNode * node = m_watcher->resultAt( index );
	if( node ) {
		QString filename = QFileInfo( node->fileName() ).canonicalFilePath();

		if( ! m_nodes.contains( filename ) )
			m_nodes.insert( filename, node );
		else {
			node->setCanBeDeleted( true );
			node->deleteInstance();
		}
	}
}


ContentViewNode * ContentViewCache::contentOfFileName( const QString & filename ) {
	QString key = QFileInfo( filename ).canonicalFilePath();
	if( m_nodes.contains( key ) ) {
		return m_nodes.value( key );
	}

	ContentViewParser * parser;

	IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
	if( fileType && ( parser = fileType->createParser() ) ) {
		try {
			ContentViewNode * node = new ContentViewNode( filename, -1, true );

			node->setCanBeDeleted( false );
			node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
			node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
			node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );
			node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );

			parser->setAutoDelete( true );
			parser->setRootNode( node );
			parser->setFilename( filename );

			m_nodes.insert( key, node );
			m_parsers.enqueue( parser );

			return node;
		} catch( ContentViewException e ) {
		}
	}

	return 0;
}

void ContentViewCache::timerEvent( QTimerEvent * event ) {
	Q_UNUSED( event );
	while( m_parsers.size() ) {
		ContentViewParser * parser = m_parsers.dequeue();
		QFuture<void> future = QtConcurrent::run( &parserLoading, parser );
		//parser->loadFromMember();
	}
}

QStringList ContentViewCache::contentsViewLoaded() const {
	return m_nodes.keys();
}

