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
#include "jstexteditor.h"
#include "config/selfwebpluginsettings.h"

#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>
#include <editors/xinxlanguagefactory.h>

// QCodeEdit header
#include <qlanguagedefinition.h>
#include <qdocumentline.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>

JSTextEditor::JSTextEditor(QWidget * parent) : XinxCodeEdit(parent)
{
	setHighlighter("ECMAScript");
}

JSTextEditor::~JSTextEditor()
{

}

bool JSTextEditor::isCommentAvailable()
{
	return true;
}

void JSTextEditor::commentSelectedText(bool uncomment)
{
	QString functionName;

	QDocumentCursor cursor(textCursor());

	QDocumentCursor cursorStart(textCursor());
	cursorStart.moveTo(cursor.selectionStart());
	bool isStartCommented = editPosition(this, cursorStart, functionName) == cpEditLongComment;

	QDocumentCursor cursorEnd(textCursor());
	cursorEnd.moveTo(cursor.selectionEnd());
	bool isEndCommented =  editPosition(this, cursorEnd, functionName) == cpEditLongComment;

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


JSTextEditor::cursorPosition JSTextEditor::editPosition(const XinxCodeEdit * textEdit, const QDocumentCursor & cursor, QString & functionName)
{
	QDocumentCursor cursorStartOfComment = textEdit->find("/*", cursor, XinxCodeEdit::FindBackward).selectionStart();
	QDocumentCursor cursorEndOfComment   = textEdit->find("*/", cursor, XinxCodeEdit::FindBackward).selectionStart();
	QDocumentCursor cursorLineCommented  = textEdit->find("//", cursor, XinxCodeEdit::FindBackward).selectionStart();

	functionName = QString();

	if (!(cursorStartOfComment.isNull() || (!cursorEndOfComment.isNull() && (cursorStartOfComment < cursorEndOfComment))))
		return cpEditLongComment;
	if (! cursorLineCommented.isNull() && (cursorLineCommented.lineNumber() == cursor.lineNumber()))
		return cpEditSimpleComment;

	QRegExp function("function[\\s]*([a-zA-Z_][a-zA-Z0-9_]*)[\\s]*\\(");
	QDocumentCursor cursorFunction = textEdit->find(function, cursor, XinxCodeEdit::FindBackward);
	QDocumentCursor endOfParam     = textEdit->find(")", cursor, XinxCodeEdit::FindBackward).selectionStart();

	if (cursorFunction.isNull())
		return cpEditGlobal;

	QDocumentCursor cursorOfFunctionName = cursorFunction;
	functionName = cursorOfFunctionName.selectedText();
	function.indexIn(functionName, 0);
	functionName = function.cap(1);

	if (endOfParam.isNull() || (endOfParam < cursorFunction))
	{
		return cpEditParams;
	}

	int bloc = 0;
	QDocumentCursor c = textEdit->find(")", cursorFunction);
	while ((c < cursor) && (c.position() > 0))
	{
		QString text = c.selectedText();
		if (text.contains('{'))
			bloc++;
		else if (text.contains('}'))
			bloc--;

		c = textEdit->find(QRegExp("[\\S]+"), c);
	}

	if (bloc > 0)
		return cpEditFunction;
	else
	{
		functionName = QString();
		return cpEditGlobal;
	}
}

JSTextEditor::cursorPosition JSTextEditor::editPosition(const QDocumentCursor & cursor)
{
	return editPosition(this, cursor, m_functionName);
}

