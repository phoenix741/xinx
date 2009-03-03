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

typedef ContentViewParser* pContentViewParser;

/* Static function */

void contentViewParserLoadFromMember( pContentViewParser & parser ) {
	Q_ASSERT( parser );
	pContentViewParser p = parser;

	p->loadFromMember();
	delete p;
}

/* ContentViewCache */

ContentViewCache::ContentViewCache( XinxProject * project ) : m_project( project ) {
}

void ContentViewCache::initializeCache() {
	QList<pContentViewParser> parsers;
	foreach( QString name, m_project->preloadedFiles() ) {
		QString filename = QDir( m_project->projectPath() ).absoluteFilePath( name );

		pContentViewParser parser;
		ContentViewNode * node = new ContentViewNode( filename, -1 );

		node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
		node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
		node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );

		IFileTypePlugin * fileType = XinxPluginsLoader::self()->matchedFileType( filename );
		if( fileType && ( parser = fileType->createParser() ) ) {
			m_nodes.insert( QFileInfo( filename ).canonicalFilePath(), node );

			parser->setRootNode( node );
			parser->setFilename( filename );
			node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );
			parsers.append( parser );
		} else
			delete node;
	}
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
		m_nodes.insert( key, node );

		parser->setAutoDelete( true );
		node->setData( QVariant::fromValue( fileType->icon() ), ContentViewNode::NODE_ICON );
		QFuture<void> future = QtConcurrent::run( parser, &ContentViewParser::loadFromFile, node, filename );
	} else
		delete node;

	return node;
}

QStringList ContentViewCache::contentsViewLoaded() const {
	return m_nodes.keys();
}

