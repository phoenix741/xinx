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

#ifndef BOOKMARKEDITORINTERFACE_H
#define BOOKMARKEDITORINTERFACE_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>

class LIBEXPORT BookmarkEditorInterface : public QObject
{
	Q_OBJECT
public:
	BookmarkEditorInterface(QObject * parent);

	/*! Return a string reprensents the content of the \em i -ième bookmark. */
	virtual QString bookmarkAt(int i) = 0;
	/*! Return the number of bookmark. */
	virtual int bookmarkCount() = 0;

public slots :
	/*!
	 * Toogled a bookmark on the selected element.
	 */
	virtual void toogledBookmark() = 0;
	/*!
	 * Go to the first Bookmark of the editor, if the editor have one bookmark.
	 */
	virtual void firstBookmark();
	/*!
	 * Go to the previous bookmark or return false if there is no more bookmark.
	 */
	virtual bool previousBookmark() = 0;
	/*!
	 * Go to the next bookmark or return false if ther is no more bookmark.
	 */
	virtual bool nextBookmark() = 0;
	/*!
	 * Go to the last bookmark of the editor, if possible.
	 */
	virtual void lastBookmark();
	/*!
	 * Go to the \em i Bookmark of the editor.
	 */
	virtual void gotoBookmarkAt(int i) = 0;
	/*!
	 * Delete all existing bookmark
	 */
	virtual void clearAllBookmark() = 0;

signals:
	/*!
	 * Signal emited when the list of bookmark is updated. When the signal is emited
	 * a range is gived to tell which row is modified.
	 * \param minValue The first bound
	 * \param maxValue The second bound
	 */
	void bookmarkModified(int minValue, int maxValue);
};

#endif // BOOKMARKEDITORINTERFACE_H
