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
#include "commentactions.h"
#include <editors/editormanager.h>
#include <editors/xinxcodeedit.h>

#include "editors/widgeteditor/css/cssfileeditor.h"
#include "editors/widgeteditor/html/htmlfileeditor.h"
#include "editors/widgeteditor/js/jsfileeditor.h"
#include "editors/widgeteditor/xml/xmlfileeditor.h"
#include "editors/widgeteditor/xsl/xslfileeditor.h"

// Qt header
#include <QDebug>
#include <QInputDialog>

/* CommentAction */

CommentAction::CommentAction() : XinxAction::Action(tr("&Comment"), QString("Ctrl+Shift+C"))
{
	action()->setStatusTip(tr("Comment the selected text"));
	action()->setWhatsThis(tr("Comment the selected text by using the language syntax. <ul><li>In <b>XML</b> like format <i>&lt;!-- comment --&gt;</i></li> <li>In <b>Javascript</b> : <i>/* comment */</i> </li></ul>"));

	addEditorSignals(SIGNAL(copyAvailable(bool)));
}

bool CommentAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * tfe = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! tfe)                   return false;
	return tfe->textEdit()->isCommentAvailable();
}

bool CommentAction::isEnabled() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return editor->textEdit()->textCursor().hasSelection();
}

void CommentAction::actionTriggered()
{
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->commentSelectedText();
}

/* UncommentAction */

UncommentAction::UncommentAction() : XinxAction::Action(tr("&Uncomment"), QString("Ctrl+Shift+D"))
{
	action()->setStatusTip(tr("Uncomment the selected text if commented"));
	action()->setWhatsThis(tr("See the comment helper function"));

	addEditorSignals(SIGNAL(copyAvailable(bool)));
}

bool UncommentAction::isEnabled() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! editor) return false;
	return editor->textEdit()->textCursor().hasSelection();
}

bool UncommentAction::isVisible() const
{
	if (! EditorManager::self()) return false;
	TextFileEditor * tfe = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	if (! tfe)                   return false;
	return tfe->textEdit()->isCommentAvailable();
}

void UncommentAction::actionTriggered()
{
	TextFileEditor * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor());
	editor->textEdit()->commentSelectedText(true);
}


