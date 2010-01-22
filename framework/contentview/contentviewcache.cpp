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
#include "project/xinxproject.h"

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
#include <QProgressDialog>

/* Static function */

ContentViewParser * parserLoading( ContentViewParser * parser ) {
	try {
		parser->loadFromMember();
	} catch( ContentViewException e ) {
		// Tant que le parseur Javascript n'est pas au norme on ne n'affiche pas les erreurs des imports �  l'utilisateur
		qDebug( qPrintable( e.getMessage() ) );
	}
	return parser;
}

/* ContentViewCache */

ContentViewCache::ContentViewCache( XinxProject * project ) : QObject( project ), m_project( project ) {
	//QThreadPool::globalInstance()->setMaxThreadCount( 1 );

	m_watcher = new QFutureWatcher<ContentViewParser*> ( this );
	connect( m_watcher, SIGNAL(resultReadyAt(int)), this, SLOT(resultReadyAt(int)) );
	connect( m_watcher, SIGNAL(finished()), this, SIGNAL(cacheLoaded()) );
	connect( m_watcher, SIGNAL(progressRangeChanged(int,int)), this, SIGNAL(progressRangeChanged(int,int)) );
	connect( m_watcher, SIGNAL(progressValueChanged(int)), this, SIGNAL(progressValueChanged(int)) );
}

ContentViewCache::~ContentViewCache() {
	killTimer( m_timerId );
	m_parsers.clear();
	if( m_watcher->isRunning() ) {
		m_watcher->waitForFinished();
	}
	delete m_watcher;

	QList<ContentViewNode*> nodes = m_nodes.values();
	foreach( ContentViewNode * node, nodes ) {
		node->setAutoDelete( true );
	}
}

void ContentViewCache::initializeCache() {
	if( m_watcher->isRunning() ) {
		m_watcher->waitForFinished();
	}

	// Initialise the first list
	QStringList preloadedFiles;
	foreach( QString name, m_project->preloadedFiles() ) {
		QString filename = QDir::cleanPath( QDir( m_project->projectPath() ).absoluteFilePath( name ) );
		if( QFile::exists( filename ) && ( ! preloadedFiles.contains( filename ) ) ) {
			preloadedFiles << filename;
		}
	}

	loadCache( preloadedFiles );

	m_timerId = startTimer( 1000 );
}

void ContentViewCache::loadCache( QStringList filenames ) {
	const QStringList & keys = m_nodes.keys();

	QQueue<QString> imports;
	
	// Remove file already in cache
	foreach( const QString & import, filenames ) {
		if( ! keys.contains( import ) ) {
			imports += import;
		}
	}

	if( imports.isEmpty() )
		return;

	// Search import
	while( imports.size() ) {
		QString filename = imports.dequeue();
		ContentViewParser * parser = createParser( filename );
		if( parser ) {
			parser->loadFromMember();
			foreach( QString file, parser->imports() ) {
				QString cleanedFile = QDir::cleanPath( file );
				if( QFile::exists( cleanedFile ) && ( ! filenames.contains( cleanedFile ) ) && ( ! keys.contains( cleanedFile ) ) ) {
					filenames << cleanedFile;
					imports.enqueue( cleanedFile );
				}
			}
			delete parser;
		}
	}

	foreach( QString filename, filenames ) {
		ContentViewParser * parser = createParserAndNode( filename );
		if( parser ) m_parsers << parser;
	}

}

void ContentViewCache::addToCache( ContentViewParser * parser ) {
	m_parsers.append( parser );
}

void ContentViewCache::resultReadyAt( int index ) {
	ContentViewParser * parser = m_watcher->resultAt( index );
	if( parser && ! parser->isPersistent() ) {
		delete parser;
	}
}

ContentViewParser * ContentViewCache::createParser( const QString & filename, ContentViewNode* node ) {
	if( ! QFileInfo( filename ).exists() ) return 0;
	if( m_nodes.contains( filename ) ) return 0;

	ContentViewParser * parser = 0;

	IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
	if( fileType && ( parser = fileType->createParser() ) ) {
		try {
			if( node ) {
				parser->setRootNode( node );
				node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );
			}
			parser->setFilename( filename );
		} catch( ContentViewException e ) {
			// Certainement une erreur indiquant que le fichier n'existe pas (normalement déj�  controlé)
			qWarning( qPrintable( e.getMessage() ) );
			delete parser;
			parser = 0;
		}
	}
	return parser;
}

ContentViewParser * ContentViewCache::createParserAndNode( const QString & filename, ContentViewNode** refNode ) {
	ContentViewNode * node = new ContentViewNode( filename, -1, true );
	ContentViewParser * parser = createParser( filename, node );

	if( parser ) {
		node->setAutoDelete( false );
		node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
		node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
		node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );

		m_nodes.insert( filename, node );
		if( refNode ) *refNode = node;
	} else
		node->deleteInstance();

	return parser;
}

ContentViewNode * ContentViewCache::contentOfFileName( const QString & filename ) {
	QString key = QFileInfo( filename ).canonicalFilePath();
	if( m_nodes.contains( key ) ) {
		return m_nodes.value( key );
	}

	ContentViewNode * node = 0;
	ContentViewParser * parser = createParserAndNode( key, &node );
	if( parser ) {
		m_parsers.enqueue( parser );
	}

	return node;
}

void ContentViewCache::timerEvent( QTimerEvent * event ) {
	Q_UNUSED( event );
	if( m_watcher->isRunning() ) {
		// Sera lancé au prochain tour
		return;
	}

	QList<ContentViewParser*> parsers;
	while( m_parsers.size() ) {
		parsers << m_parsers.dequeue();
	}

	m_watcher->setFuture( QtConcurrent::mapped( parsers, parserLoading ) );
}

QStringList ContentViewCache::contentsViewLoaded() const {
	return m_nodes.keys();
}

