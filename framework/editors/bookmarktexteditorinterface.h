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

#ifndef BOOKMARKTEXTEDITORINTERFACE_H
#define BOOKMARKTEXTEDITORINTERFACE_H

// Xinx header
#include "editors/bookmarkeditorinterface.h"

class XinxCodeEdit;
class TextFileEditor;

class BookmarkTextEditorInterface : public BookmarkEditorInterface {
	Q_OBJECT
public:
	BookmarkTextEditorInterface( TextFileEditor * parent );

	/*!
	 * List of bookmarks of the editor
	 */
	virtual QList<int> bookmarks() const;


	void setBookmark( int line, bool enabled );
	virtual void toogledBookmark();
	virtual void gotoBookmarkAt( int i );
	virtual QString bookmarkAt( int i );
	virtual int bookmarkCount();
	virtual bool previousBookmark();
	virtual bool nextBookmark();
	virtual void clearAllBookmark();

	void setTextEditor( XinxCodeEdit * textEdit );
public slots:
	/*! Slot called when the bookmark is toogled on a line. This slot change the project settings. */
	void slotBookmarkToggled( int line, bool enabled );

private:
	XinxCodeEdit * m_view;
	TextFileEditor * m_textEdit;
};

#endif // BOOKMARKTEXTEDITORINTERFACE_H
