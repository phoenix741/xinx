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

ContentViewParser * parserLoading( ContentViewParser * parser ) {
	try {
		parser->loadFromMember();
	} catch( ContentViewException e ) {
		// Tant que le parseur Javascript n'est pas au norme on ne n'affiche pas les erreurs des imports √  l'utilisateur
		qDebug( qPrintable( e.getMessage() ) );
	}
	return parser;
}

/* ContentViewCache */

ContentViewCache::ContentViewCache( XinxProject * project ) : QObject( project ), m_project( project ) {
	m_watcher = new QFutureWatcher<ContentViewParser*> ( this );
	connect( m_watcher, SIGNAL(resultReadyAt(int)), this, SLOT(resultReadyAt(int)) );
	m_timerId = startTimer( 200 );
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

	QList<ContentViewParser*> parsers;
	foreach( QString name, m_project->preloadedFiles() ) {
		QString filename = QDir::cleanPath( QDir( m_project->projectPath() ).absoluteFilePath( name ) );
		ContentViewParser * parser = createParser( filename );
		if( parser ) parsers << parser;
	}
	m_watcher->setFuture( QtConcurrent::mapped( parsers, parserLoading ) );
}

void ContentViewCache::resultReadyAt( int index ) {
	ContentViewParser * parser = m_watcher->resultAt( index );
	if( parser ) {
		delete parser;
	}
}

ContentViewParser * ContentViewCache::createParser( const QString & filename, ContentViewNode** refNode ) {
	if( ! QFileInfo( filename ).exists() ) return 0;
	if( m_nodes.contains( filename ) ) return 0;

	ContentViewParser * parser = 0;

	IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
	if( fileType && ( parser = fileType->createParser() ) ) {
		try {
			ContentViewNode * node = new ContentViewNode( filename, -1, true );

			node->setAutoDelete( false );
			node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
			node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
			node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );
			node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );

			parser->setRootNode( node );
			parser->setFilename( filename );

			m_nodes.insert( filename, node );
			if( refNode ) *refNode = node;
		} catch( ContentViewException e ) {
			// Certainement une erreur indiquant que le fichier n'existe pas (normalement d√©j√  control√©)
			qWarning( qPrintable( e.getMessage() ) );
		}
	}

	return parser;
}

ContentViewNode * ContentViewCache::contentOfFileName( const QString & filename ) {
	QString key = QFileInfo( filename ).canonicalFilePath();
	if( m_nodes.contains( key ) ) {
		return m_nodes.value( key );
	}

	ContentViewNode * node = 0;
	ContentViewParser * parser = createParser( key, &node );
	if( parser ) {
		m_parsers.enqueue( parser );
	}

	return node;
}

void ContentViewCache::timerEvent( QTimerEvent * event ) {
	Q_UNUSED( event );
	if( m_watcher->isRunning() ) {
		// Sera lanc√© au prochain tour
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

