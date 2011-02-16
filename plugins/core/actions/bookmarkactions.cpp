/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2006-2010 by Ulrich Van Den Hekke                         *
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
#include "bookmarkactions.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

/* BookmarkAction */

BookmarkAction::BookmarkAction() :
		XinxAction::Action(QIcon(":/images/bookmark.png"), tr("&Bookmark"), QKeySequence("Ctrl+B"))
{

}

bool BookmarkAction::isEnabled() const
{
	return EditorManager::self()->currentEditor() != NULL;
}

void BookmarkAction::actionTriggered()
{
	AbstractEditor * editor = EditorManager::self()->currentEditor();
	Q_ASSERT_X(editor, "PreviousBookmarkAction::actionTriggered", "Editor must be defined");
	editor->bookmarkInterface()->toogledBookmark();
}

/* NextBookmarkAction */

NextBookmarkAction::NextBookmarkAction() :
		XinxAction::Action(QIcon(":/images/bookmarknext.png"), tr("&Next bookmark"), QKeySequence("F2"))
{

}

bool NextBookmarkAction::isEnabled() const
{
	return EditorManager::self()->currentEditor() != NULL;
}

void NextBookmarkAction::actionTriggered()
{
	int index = EditorManager::self()->currentIndex();
	AbstractEditor * editor = EditorManager::self()->currentEditor();
	Q_ASSERT_X(editor, "PreviousBookmarkAction::actionTriggered", "Editor must be defined");
	if ((! editor->bookmarkInterface()->nextBookmark()) && (index < EditorManager::self()->editorsCount()))
	{
		do
		{
			index ++ ;
			editor = EditorManager::self()->editor(index);
		}
		while ((index < EditorManager::self()->editorsCount()) && (editor->bookmarkInterface()->bookmarkCount() == 0));

		if (index < EditorManager::self()->editorsCount())
		{
			EditorManager::self()->setCurrentEditor(index);
			editor->bookmarkInterface()->firstBookmark();
		}
	}
}

/* PreviousBookmarkAction */

PreviousBookmarkAction::PreviousBookmarkAction() :
		XinxAction::Action(QIcon(":/images/bookmarkprevious.png"), tr("&Previous bookmark"), QKeySequence("Shift+F2"))
{

}

bool PreviousBookmarkAction::isEnabled() const
{
	return EditorManager::self()->currentEditor() != NULL;
}

void PreviousBookmarkAction::actionTriggered()
{
	int index = EditorManager::self()->currentIndex();
	AbstractEditor * editor = EditorManager::self()->currentEditor();
	Q_ASSERT_X(editor, "PreviousBookmarkAction::actionTriggered", "Editor must be defined");
	if ((! editor->bookmarkInterface()->previousBookmark()) && (index > 0))
	{
		do
		{
			index -- ;
			editor = EditorManager::self()->editor(index);
		}
		while ((index >= 0) && (editor->bookmarkInterface()->bookmarkCount() == 0));

		if (index >= 0)
		{
			EditorManager::self()->setCurrentEditor(index);
			editor->bookmarkInterface()->lastBookmark();
		}
	}
}

/* BookmarkAction */

ClearAllBookmarkAction::ClearAllBookmarkAction() :
		XinxAction::Action(QIcon(":/images/clearallbookmarks.png"), tr("&Clear all bookmark"), QKeySequence("Ctrl+F2"))
{

}

bool ClearAllBookmarkAction::isEnabled() const
{
	return EditorManager::self()->editorsCount() > 0;
}

void ClearAllBookmarkAction::actionTriggered()
{
	for (int i = 0; i < EditorManager::self()->editorsCount() ; i++)
	{
		EditorManager::self()->editor(i)->bookmarkInterface()->clearAllBookmark();
	}
}
