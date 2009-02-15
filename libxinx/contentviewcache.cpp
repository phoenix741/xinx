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
#include "contentviewcache.h"
#include "contentviewparser.h"
#include "contentviewnode.h"

// Qt header
#include <QtConcurrentRun>
#include <QFileInfo>
#include <QIcon>
#include <QVariant>

/* ContentViewCache */

ContentViewCache::ContentViewCache( XinxProject * project ) {
}

ContentViewNode * ContentViewCache::contentOfFileName( const QString & filename ) {
	ContentViewNode * node = new ContentViewNode( filename, -1 );
	node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_NAME );
	node->setData( QVariant( QFileInfo( filename ).fileName() ), ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( QVariant( "include"                        ), ContentViewNode::NODE_TYPE );
	node->setData( QVariant( QIcon(":/images/import.png")     ), ContentViewNode::NODE_ICON );

	ContentViewParser * parser = 0; // QCache pour gérer la suppression
	parser->setAutoDelete( true );

	QFuture<void> future = QtConcurrent::run( parser, &ContentViewParser::loadFromFile, node, filename );

	return node;
}

