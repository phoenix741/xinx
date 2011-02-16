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
#include "maqtexteditor.h"

#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>
#include <editors/xinxlanguagefactory.h>

// QCodeEdit header
#include <qlanguagedefinition.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>

MaquetteTextEditor::MaquetteTextEditor(QWidget * parent) : XinxCodeEdit(parent)
{
	setHighlighter("Maquette");
}

MaquetteTextEditor::~MaquetteTextEditor()
{

}

bool MaquetteTextEditor::isCommentAvailable()
{
	return false;
}

void MaquetteTextEditor::commentSelectedText(bool uncomment)
{
	QDocumentCursor cursor(textCursor());

	QDocumentCursor cursorStart(textCursor());
	cursorStart.moveTo(cursor.selectionStart());
	bool isStartCommented = editPosition(this, cursorStart) == cpEditComment;

	QDocumentCursor cursorEnd(textCursor());
	cursorEnd.moveTo(cursor.selectionEnd());
	bool isEndCommented =  editPosition(this, cursorEnd) == cpEditComment;

	QString text = cursor.selectedText();
	text = text.replace("/*", "");
	text = text.replace("*/", "");

	cursor.beginEditBlock();

	cursor.removeSelectedText();
	if (!(isStartCommented ^ uncomment))
	{
		// Comment
		if (! uncomment)
			cursor.insertText("/*");
		else
			cursor.insertText("*/");
	}
	cursor.insertText(text);
	if (!(isEndCommented ^ uncomment))
	{
		// End the comment
		if (! uncomment)
			cursor.insertText("*/");
		else
			cursor.insertText("/*");
	}

	cursor.endEditBlock();
}


MaquetteTextEditor::cursorPosition MaquetteTextEditor::editPosition(const XinxCodeEdit * textEdit, const QDocumentCursor & cursor)
{
	QDocumentCursor cursorStartOfComment = textEdit->find("/*", cursor, XinxCodeEdit::FindBackward).selectionStart();
	QDocumentCursor cursorEndOfComment   = textEdit->find("*/", cursor, XinxCodeEdit::FindBackward).selectionStart();

	bool inComment =
		(!(cursorStartOfComment.isNull() || (!cursorEndOfComment.isNull() && (cursorStartOfComment < cursorEndOfComment))));

	if (inComment)
		return cpEditComment;

	int bloc = 0;
	QDocumentCursor c = textEdit->find("{", cursor, XinxCodeEdit::FindBackward);
	while (c < cursor)
	{
		QString text = c.selectedText();
		if (text.contains('{'))
			bloc++;
		else if (text.contains('}'))
			bloc--;

		c = textEdit->find(QRegExp("[\\S]+"), c);
	}

	if (bloc > 0)
		return cpEditTag;
	else
	{
		return cpEditGlobal;
	}
}

MaquetteTextEditor::cursorPosition MaquetteTextEditor::editPosition(const QDocumentCursor & cursor)
{
	return editPosition(this, cursor);
}
