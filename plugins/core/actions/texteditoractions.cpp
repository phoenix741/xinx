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
#include "texteditoractions.h"
#include <editors/editormanager.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <core/xinxconfig.h>

// Qt header
#include <QDebug>
#include <QInputDialog>

/* GotoLineAction */

GotoLineAction::GotoLineAction() : XinxAction::Action(tr("Go to &Line ..."), QString("Ctrl+G"))
{
	action()->setStatusTip(tr("Move the cursor to line, choose by user"));
	action()->setWhatsThis(tr("This action open a dialog box to choose the line number where you want to go."));
}

bool GotoLineAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor)                   return false;
	return true;
}

void GotoLineAction::actionTriggered()
{
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	int currentLine = editor->textEdit()->currentRow() + 1;
	int maxLine     = editor->textEdit()->countRow();

	bool ok;
	int line = QInputDialog::getInt(qApp->activeWindow(), tr("Goto Line"), tr("Line:"), currentLine, 1, maxLine, 1, &ok);
	if (ok)
		editor->textEdit()->gotoLine(line);
}

/* DuplicateLinesAction */

DuplicateLinesAction::DuplicateLinesAction(): Action(tr("&Duplicate current line"), QKeySequence("Ctrl+D"))
{
	action ()->setStatusTip(tr("Duplicate the current line"));
}

bool DuplicateLinesAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void DuplicateLinesAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->duplicateLines();
}

/* MoveLineUpAction */

MoveLineUpAction::MoveLineUpAction(): Action(QIcon(":/images/1uparrow.png"), tr("&Move line up"), QKeySequence("Ctrl+Shift+Up"))
{
	action ()->setStatusTip(tr("Move the current line up"));
}

bool MoveLineUpAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void MoveLineUpAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->moveLineUp();
}

/* MoveLineDownAction */

MoveLineDownAction::MoveLineDownAction(): Action(QIcon(":/images/1downarrow.png"), tr("&Move line down"), QKeySequence("Ctrl+Shift+Down"))
{
	action ()->setStatusTip(tr("Move the current line down"));
}

bool MoveLineDownAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void MoveLineDownAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->moveLineDown();
}

/* UpperSelectedTextAction */

UpperSelectedTextAction::UpperSelectedTextAction(): Action(QIcon(":/images/fontsizeup.png"), tr("&Upper Case"), QKeySequence::mnemonic("Ctrl+U"))
{
	action ()->setStatusTip(tr("Make the selected character upper case"));
	addEditorSignals(SIGNAL(copyAvailable(bool)));
}

bool UpperSelectedTextAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

bool UpperSelectedTextAction::isEnabled() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	if (! editor->textEdit()->textCursor().hasSelection()) return false;
	return true;
}

void UpperSelectedTextAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->upperSelectedText();
}

/* LowerSelectedTextAction */

LowerSelectedTextAction::LowerSelectedTextAction(): Action(QIcon(":/images/fontsizedown.png"), tr("&Lower Case"), QKeySequence::mnemonic("Ctrl+L"))
{
	action ()->setStatusTip(tr("Make the selected character lower case"));
	addEditorSignals(SIGNAL(copyAvailable(bool)));
}

bool LowerSelectedTextAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

bool LowerSelectedTextAction::isEnabled() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	if (! editor->textEdit()->textCursor().hasSelection()) return false;
	return true;
}

void LowerSelectedTextAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->lowerSelectedText();
}

/* ShowSpecialCharsAction */

ShowSpecialCharsAction::ShowSpecialCharsAction(): Action(QIcon(":/images/draw-text.png"), tr("Show space and &Tabulation"), QKeySequence())
{
	action ()->setCheckable(true);
	action ()->setChecked(XINXConfig::self()->config().editor.showTabulationAndSpace);
}

bool ShowSpecialCharsAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void ShowSpecialCharsAction::actionTriggered()
{
	if (XINXConfig::self()->config().editor.showTabulationAndSpace != action ()->isChecked())
	{
		XINXConfig::self()->config().editor.showTabulationAndSpace = action ()->isChecked();
		XINXConfig::self()->save();
	}
}

/* IndentAction */

IndentAction::IndentAction(): Action(QIcon(":/images/format-indent-more.png"), tr("Indent Code"), QKeySequence("Tab"))
{

}

bool IndentAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void IndentAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->indent();
}

/* UnindentAction */

UnindentAction::UnindentAction(): Action(QIcon(":/images/format-indent-less.png"), tr("Unindent code"), QKeySequence("Shift+Tab"))
{

}

bool UnindentAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void UnindentAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->indent(true);
}

/* AutoIndentAction */

AutoIndentAction::AutoIndentAction(): Action(tr("Pretty print"), QKeySequence("Ctrl+Shift+F"))
{

}

bool AutoIndentAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void AutoIndentAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->autoIndent();
}

/* CompleteAction */

CompleteAction::CompleteAction(): Action(tr("Completer"), QKeySequence("Ctrl+E"))
{

}

bool CompleteAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void CompleteAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->complete();
}

/* HighlightWordAction */

HighlightWordAction::HighlightWordAction(): Action(tr("Highlight same word"), QKeySequence("Ctrl+U"))
{

}

bool HighlightWordAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return true;
}

void HighlightWordAction::actionTriggered()
{
	TextFileEditor * editor = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->callTextHighlighter();
}
