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
#include "bookmarktexteditorinterface.h"
#include "editors/xinxcodeedit.h"
#include "editors/textfileeditor.h"

/* BookmarkTextEditorInterface */

/*!
 * \ingroup XinxEditors
 * \class BookmarkTextEditorInterface
 * \since 0.9.0.0
 *
 * \brief Re-implementation of BookmarkEditorInterface for text editor.
 *
 * This class re-implement the BookmarkEditorInterface for use with the TextFileEditor parent.
 * This class works as a wrapper of XinxCodeEdit, last implements equivalent method.
 */

/*!
 * \brief Create a BookmarkTextEditorInterface
 */
BookmarkTextEditorInterface::BookmarkTextEditorInterface(TextFileEditor * parent) : BookmarkEditorInterface(parent), m_view(0), m_textEdit(parent)
{
}

/*!
 * \brief Slot called when the bookmark is toogled on a line.
 */
void BookmarkTextEditorInterface::slotBookmarkToggled(int line, bool enabled)
{
	if (enabled)
	{
		gotoBookmarkAt(m_view->listOfBookmark().indexOf(line));
	}
	emit bookmarkModified(0, bookmarkCount());
}

/*!
 * \brief List of bookmarks of the editor
 *
 * In the text editor, bookmarks are line number
 */
QList<int> BookmarkTextEditorInterface::bookmarks() const
{
	return m_view->listOfBookmark();
}

/*!
 * \brief This methode change the state to \p enabled of the bookmark at line \p line.
 *
 * This method overload XinxCodeEdit::setBookmark()
 */
void BookmarkTextEditorInterface::setBookmark(int line, bool enabled)
{
	m_view->setBookmark(line, enabled);
}

/*!
 * \brief This methode toggle the state of the bookmark at the current line.
 *
 * This method overload XinxCodeEdit::setBookmark()
 */
void BookmarkTextEditorInterface::toogledBookmark()
{
	m_view->setBookmark(m_view->currentRow(), !m_view->listOfBookmark().contains(m_view->currentRow()));
}

/*!
 * \brief This methode go to the bookmark \p i.
 *
 * This method overload XinxCodeEdit::gotoLine()
 */
void BookmarkTextEditorInterface::gotoBookmarkAt(int i)
{
	m_view->gotoLine(m_view->listOfBookmark().at(i));
}

/*!
 * \brief This method return a description for the bookmark \p i.
 */
QString BookmarkTextEditorInterface::bookmarkAt(int i)
{
	QString description = tr("In editor '%1' at line %2");
	description = description.arg(m_textEdit->getTitle()).arg(m_view->listOfBookmark().at(i));
	return description;
}

/*!
 * \brief This method return the number of bookmark in the editor
 *
 * This method overload XinxCodeEdit::listOfBookmark().count()
 */
int BookmarkTextEditorInterface::bookmarkCount()
{
	return m_view->listOfBookmark().count();
}

/*!
 * \brief This method go to the previous bookmark
 *
 * This method overload XinxCodeEdit::previousBookmark()
 */
bool BookmarkTextEditorInterface::previousBookmark()
{
	return m_view->previousBookmark();
}

/*!
 * \brief This method go to the next bookmark
 *
 * This method overload XinxCodeEdit::nextBookmark()
 */
bool BookmarkTextEditorInterface::nextBookmark()
{
	return m_view->nextBookmark();
}

/*!
 * \brief This method remove all bookmark from the editor
 *
 * This method overload XinxCodeEdit::clearBookmark()
 */
void BookmarkTextEditorInterface::clearAllBookmark()
{
	m_view->clearBookmark();
}

/*!
 * \brief This method define the XinxCodeEdit editor used in this interface.
 *
 * This methode must be called by TextFileEditor before call all other method used
 * to navigate throw bookmark.
 */
void BookmarkTextEditorInterface::setTextEditor(XinxCodeEdit * textEdit)
{
	if (m_view != textEdit)
	{
		m_view = textEdit;
	}
}
