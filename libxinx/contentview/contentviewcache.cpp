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

/* Declaration */

typedef ContentViewParser * pContentViewParser;

/* Static function */

void contentViewParserLoadFromMember( pContentViewParser & parser ) {
	Q_ASSERT( parser );

	qDebug( qPrintable( QString("x : %1").arg( (unsigned long)parser, 0, 16 ) ) );

	try {
		parser->loadFromMember();
	} catch( ContentViewException e ) {
		qWarning( qPrintable( e.getMessage() ) );
	}
	delete parser;
}

/* ContentViewCache */

ContentViewCache::ContentViewCache( XinxProject * project ) : m_project( project ) {
}

void ContentViewCache::initializeCache() {
	QVector<pContentViewParser> parsers;
	foreach( QString name, m_project->preloadedFiles() ) {
		QString filename = QDir( m_project->projectPath() ).absoluteFilePath( name );

		pContentViewParser parser;
		ContentViewNode * node = new ContentViewNode( filename, -1 );

		node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
		node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
		node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );

		IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
		if( fileType && ( parser = fileType->createParser() ) ) {
			try {
				parser->setRootNode( node );
				parser->setFilename( filename );

				node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );

				m_nodes.insert( QFileInfo( filename ).canonicalFilePath(), node );
				parsers.append( parser );
			} catch( ContentViewException e ) {
				delete parser;
				delete node;
				qWarning( qPrintable( e.getMessage() ) );
			}
		} else
			delete node;
	}
	qDebug( qPrintable( QString("Count : %1").arg( parsers.size() ) ) );
	qDebug( qPrintable( QString("1 : %1").arg( (unsigned long)parsers.at( 0 ), 0, 16 ) ) );
	qDebug( qPrintable( QString("2 : %1").arg( (unsigned long)parsers.at( 1 ), 0, 16 ) ) );
	QFuture<void> future = QtConcurrent::map( parsers, contentViewParserLoadFromMember );
}

ContentViewNode * ContentViewCache::contentOfFileName( const QString & filename ) {
	QString key = QFileInfo( filename ).canonicalFilePath();
	if( m_nodes.contains( key ) ) {
		return m_nodes.value( key );
	}

	ContentViewParser * parser;
	ContentViewNode * node = new ContentViewNode( filename, -1 );

	node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
	node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );

	IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
	if( fileType && ( parser = fileType->createParser() ) ) {
		try {
			parser->setAutoDelete( true );
			parser->setRootNode( node );
			parser->setFilename( filename );

			node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );

			m_nodes.insert( key, node );

			QFuture<void> future = QtConcurrent::run( parser, &ContentViewParser::loadFromMember );
		} catch( ContentViewException e ) {
		}
	} else
		delete node;

	return node;
}

QStringList ContentViewCache::contentsViewLoaded() const {
	return m_nodes.keys();
}

