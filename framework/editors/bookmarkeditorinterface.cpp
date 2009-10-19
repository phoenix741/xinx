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
#include "editors/bookmarkeditorinterface.h"

/* BookmarkEditorInterface */

BookmarkEditorInterface::BookmarkEditorInterface( QObject * parent ) : QObject( parent ) {
}

void BookmarkEditorInterface::firstBookmark() {
	if( bookmarkCount() > 0 )
		gotoBookmarkAt( 0 );
}

void BookmarkEditorInterface::lastBookmark() {
	if( bookmarkCount() > 0 )
		gotoBookmarkAt( bookmarkCount() - 1 );
}
