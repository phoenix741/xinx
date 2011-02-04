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
#include "clipboardactions.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

/* CutAction */

CutAction::CutAction() : XinxAction::Action(QIcon(":/images/editcut.png"), tr("Cu&t"), QKeySequence::Cut)
{
	action ()->setStatusTip(tr("Cut the selection to the clipboard"));
	addEditorSignals(SIGNAL(copyAvailable(bool)));
}

bool CutAction::isEnabled() const
{
	return EditorManager::self ()->currentEditor() && EditorManager::self ()->currentEditor()->canCopy();
}

void CutAction::actionTriggered()
{
	AbstractEditor * editor =  EditorManager::self ()->currentEditor();
	if (editor && editor->canCopy())
		editor->cut();
}

/* CopyAction */

CopyAction::CopyAction() : XinxAction::Action(QIcon(":/images/editcopy.png"), tr("&Copy"), QKeySequence::Copy)
{
	action ()->setStatusTip(tr("Copy the selection to the clipboard"));
	addEditorSignals(SIGNAL(copyAvailable(bool)));
}

bool CopyAction::isEnabled() const
{
	return EditorManager::self ()->currentEditor() && EditorManager::self ()->currentEditor()->canCopy();
}

void CopyAction::actionTriggered()
{
	AbstractEditor * editor =  EditorManager::self ()->currentEditor();
	if (editor && editor->canCopy())
		editor->copy();
}

/* PasteAction */

PasteAction::PasteAction() : XinxAction::Action(QIcon(":/images/editpaste.png"), tr("&Paste"), QKeySequence::Paste)
{
	action ()->setStatusTip(tr("Paste the clipboard's contents"));
	addEditorSignals(SIGNAL(pasteAvailable(bool)));
}

bool PasteAction::isEnabled() const
{
	return EditorManager::self ()->currentEditor() && EditorManager::self ()->currentEditor()->canPaste();
}

void PasteAction::actionTriggered()
{
	AbstractEditor * editor =  EditorManager::self ()->currentEditor();
	if (editor && editor->canPaste ())
		editor->paste();
}
