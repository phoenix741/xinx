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

/*!
 * \ingroup XinxEditors
 * \class BookmarkEditorInterface
 * \since 0.9.0.0
 *
 * \brief The BookmarkEditorInterface is used by AbstractEditor's user class to
 * navigate throw Bookmark.
 *
 * A bookmark is mark in the editor to permit to return quickly in this position in the editor.
 * An editor can have many bookmark.
 *
 * This class is used to toggle the bookmark on the line, to go to the previous/next bookmark.
 */

/*!
 * \brief Create the BookmarkEditorInterface.
 *
 * This class must be re-implemented.
 */
BookmarkEditorInterface::BookmarkEditorInterface(QObject * parent) : QObject(parent)
{
}

/*!
 * \fn virtual QString BookmarkEditorInterface::bookmarkAt(int i) = 0
 * \brief Return a string that represent the content of the \p i -ième bookmark.
 */

/*!
 * \fn virtual int BookmarkEditorInterface::bookmarkCount() = 0
 * \brief Return the number of bookmark.
 */

/*!
 * \fn virtual void BookmarkEditorInterface::toogledBookmark() = 0
 * \brief Toogled a bookmark on the selected element.
 */

/*!
 * \fn virtual bool BookmarkEditorInterface::previousBookmark() = 0
 * \brief Go to the previous bookmark or return false if there is no more bookmark.
 */

/*!
 * \fn virtual bool BookmarkEditorInterface::nextBookmark() = 0
 * \brief Go to the next bookmark or return false if ther is no more bookmark.
 */

/*!
 * \fn virtual void BookmarkEditorInterface::gotoBookmarkAt(int i) = 0
 * \brief Go to the \p i Bookmark of the editor.
 */

/*!
 * \fn virtual void BookmarkEditorInterface::clearAllBookmark() = 0
 * \brief Delete all existing bookmark
 */

/*!
 * \fn void BookmarkEditorInterface::bookmarkModified(int minValue, int maxValue)
 * Signal emited when the list of bookmark is updated. When the signal is emited
 * a range is gived to tell which row is modified.
 * \param minValue The first bound
 * \param maxValue The second bound
 */

/*!
 * \brief If the editor have one bookmark, go to the first Bookmark of the editor.
 */
void BookmarkEditorInterface::firstBookmark()
{
	if (bookmarkCount() > 0)
		gotoBookmarkAt(0);
}

/*!
 * \brief If the editor have one bookmark, go to the last bookmark of the editor.
 */
void BookmarkEditorInterface::lastBookmark()
{
	if (bookmarkCount() > 0)
		gotoBookmarkAt(bookmarkCount() - 1);
}
