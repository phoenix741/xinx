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
#include "javascriptmodelcompleter.h"
#include <contentview/contentviewnode.h>

// Qt header
#include <QIcon>

/* JavascriptModelCompleter */

JavascriptModelCompleter::JavascriptModelCompleter( ContentViewNode * root, QObject *parent ) : CompletionNodeModel( root, parent ), m_functionFiltre( QString() ) {

}

JavascriptModelCompleter::~JavascriptModelCompleter() {

}

void JavascriptModelCompleter::setFilter( const QString functionName ) {
	if( m_functionFiltre != functionName ) {
		m_functionFiltre = functionName;
		reset();
	}
}

bool JavascriptModelCompleter::mustElementBeShowed( ContentViewNode * node ) {
	ContentViewNode * p = parent( node );
	if( p && ( p->data( ContentViewNode::NODE_TYPE ).toString() == "function" ) ) {
		return true;
	}

	if( ! p ) {
		return true;
	}

	return false;
}

