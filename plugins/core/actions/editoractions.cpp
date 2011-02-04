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
#include "editoractions.h"
#include <editors/editormanager.h>
#include <editors/textfileeditor.h>

// Qt header

/* UndoAction */

UndoAction::UndoAction(): Action(QIcon(":/images/undo.png"), tr("&Undo"), QKeySequence::Undo)
{
	action ()->setStatusTip(tr("Undo the last action"));
	addEditorSignals(SIGNAL(undoAvailable(bool)));
}

bool UndoAction::isEnabled() const
{
	return EditorManager::self ()->currentEditor() && EditorManager::self ()->currentEditor()->canUndo();
}

void UndoAction::actionTriggered()
{
	AbstractEditor * editor =  EditorManager::self ()->currentEditor();
	if (editor && editor->canUndo ())
		editor->undo();
}

/* RedoAction */

RedoAction::RedoAction(): Action(QIcon(":/images/redo.png"), tr("&Redo"), QKeySequence::Redo)
{
	action ()->setStatusTip(tr("Redo the next action"));
	addEditorSignals(SIGNAL(redoAvailable(bool)));
}

bool RedoAction::isEnabled() const
{
	return EditorManager::self ()->currentEditor() && EditorManager::self ()->currentEditor()->canRedo();
}

void RedoAction::actionTriggered()
{
	AbstractEditor * editor =  EditorManager::self ()->currentEditor();
	if (editor && editor->canRedo ())
		editor->redo();
}

/* SelectAllAction */

SelectAllAction::SelectAllAction(): Action(QIcon(":/images/edit-select-all.png"), tr("&Select All"), QKeySequence::SelectAll)
{
	action ()->setStatusTip(tr("Select all the text of the current editor"));
}

bool SelectAllAction::isEnabled() const
{
	return EditorManager::self ()->currentEditor() && qobject_cast<TextFileEditor*>(EditorManager::self ()->currentEditor());
}

void SelectAllAction::actionTriggered()
{
	TextFileEditor * editor =  qobject_cast<TextFileEditor*>(EditorManager::self ()->currentEditor());
	editor->selectAll();
}
