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

BookmarkTextEditorInterface::BookmarkTextEditorInterface(TextFileEditor * parent) : BookmarkEditorInterface(parent), m_view(0), m_textEdit(parent)
{
}

void BookmarkTextEditorInterface::slotBookmarkToggled(int line, bool enabled)
{
	Q_UNUSED(enabled);
	Q_UNUSED(line);

	if (enabled)
	{
		gotoBookmarkAt(m_view->listOfBookmark().indexOf(line));
	}
	emit bookmarkModified(0, bookmarkCount());
}

QList<int> BookmarkTextEditorInterface::bookmarks() const
{
	return m_view->listOfBookmark();
}

void BookmarkTextEditorInterface::setBookmark(int line, bool enabled)
{
	m_view->setBookmark(line, enabled);
}

void BookmarkTextEditorInterface::toogledBookmark()
{
	m_view->setBookmark(m_view->currentRow(), !m_view->listOfBookmark().contains(m_view->currentRow()));
}

void BookmarkTextEditorInterface::gotoBookmarkAt(int i)
{
	m_view->gotoLine(m_view->listOfBookmark().at(i));
}

QString BookmarkTextEditorInterface::bookmarkAt(int i)
{
	QString description = tr("In editor '%1' at line %2");
	description = description.arg(m_textEdit->getTitle()).arg(m_view->listOfBookmark().at(i));
	return description;
}

int BookmarkTextEditorInterface::bookmarkCount()
{
	return m_view->listOfBookmark().count();
}

bool BookmarkTextEditorInterface::previousBookmark()
{
	return m_view->previousBookmark();
}

bool BookmarkTextEditorInterface::nextBookmark()
{
	return m_view->nextBookmark();
}

void BookmarkTextEditorInterface::clearAllBookmark()
{
	m_view->clearBookmark();
}

void BookmarkTextEditorInterface::setTextEditor(XinxCodeEdit * textEdit)
{
	if (m_view != textEdit)
	{
		m_view = textEdit;
	}
}
