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
#include "snipetcompletionnodemodel.h"
#include "snipets/snipetcompletionparser.h"
#include "contentview/contentviewnode.h"

SnipetCompletionNodeModel::SnipetCompletionNodeModel( ContentViewNode * root, QObject *parent ) : CompletionNodeModel( root ? root : SnipetCompletionParser::self()->rootNode(), parent ) {
	if( root )
		startTimer( 0 );
}

SnipetCompletionNodeModel::~SnipetCompletionNodeModel() {
	SnipetCompletionParser::self()->rootNode()->removeModel( this, (unsigned long)SnipetCompletionParser::self()->rootNode() );
}

void SnipetCompletionNodeModel::timerEvent( QTimerEvent * event ) {
	killTimer( event->timerId() );

	QMutexLocker locker( mutex() );

	SnipetCompletionParser::self()->rootNode()->addModel( this, (unsigned long)SnipetCompletionParser::self()->rootNode() );
	addAllNodes( 0, SnipetCompletionParser::self()->rootNode() );
}
