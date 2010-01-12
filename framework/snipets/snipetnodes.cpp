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
#include "snipetnodes.h"
#include "snipetmanager.h"
#include "contentview/contentviewnode.h"

// Qt header
#include <QSqlQuery>
#include <QtConcurrentRun>
#include <QSqlError>
#include <QVariant>

/* Static member */

SnipetCompletionParser * SnipetCompletionParser::s_self = 0;

/* SnipetCompletionParser */

SnipetCompletionParser::SnipetCompletionParser() {
}

SnipetCompletionParser::~SnipetCompletionParser() {

}

SnipetCompletionParser * SnipetCompletionParser::self() {
	if( ! s_self ) {
		s_self = new SnipetCompletionParser();
		try {
			ContentViewNode * node = new ContentViewNode( "root", -1 );
			node->setAutoDelete( false );
			s_self->setRootNode( node );
			QFuture<void> future = QtConcurrent::run( (ContentViewParser*)s_self, &ContentViewParser::loadFromMember );
		} catch( ContentViewException e ) {
			qWarning( qPrintable( e.getMessage() ) );

		}
		XINXStaticDeleter::self()->add( s_self );
	}
	return s_self;
}

void SnipetCompletionParser::loadFromDeviceImpl() {
	Q_ASSERT( rootNode() );

	loadAttachedNode( rootNode() );

	QSqlQuery selectQuery( "SELECT id, icon, shortcut, name FROM snipets", SnipetManager::self()->database() );

	bool result = selectQuery.exec();
	Q_ASSERT( result );
	if( ! result ) {
		qWarning( qPrintable( selectQuery.lastError().text() ) );
		return;
	}

	while( selectQuery.next() ) {
		const int id = selectQuery.value( 0 ).toInt();
		const QString icon = selectQuery.value( 1 ).toString();
		const QString shortcut = selectQuery.value( 2 ).toString();
		const QString name = selectQuery.value( 3 ).toString();

		ContentViewNode * node = new ContentViewNode( shortcut, id );
		node->setData( "Snipet", ContentViewNode::NODE_TYPE );
		node->setData( icon.isEmpty() ? ":/images/help-hint.png" : icon, ContentViewNode::NODE_ICON );
		node->setData( shortcut, ContentViewNode::NODE_DISPLAY_NAME );

		node = attachNode( rootNode(), node );
	}

	detachAttachedNode();
}

