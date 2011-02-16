/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "jscontextparser.h"
#include <editors/xinxcodeedit.h>

// QCodeEdit header
#include <qdocumentcursor.h>
#include <editors/textfileeditor.h>
#include <editors/widgeteditor/html/htmlfileeditor.h>
#include <editors/widgeteditor/xsl/xslfileeditor.h>

namespace Core
{

namespace JavaScript
{

JsContextType::Position JsContextParser::editPosition(const QDocumentCursor& cursor, QString& functionName) const
{
	QDocumentCursor cursorStartOfComment = editor()->textEdit()->find("/*", cursor, XinxCodeEdit::FindBackward).selectionStart();
	QDocumentCursor cursorEndOfComment   = editor()->textEdit()->find("*/", cursor, XinxCodeEdit::FindBackward).selectionStart();
	QDocumentCursor cursorLineCommented  = editor()->textEdit()->find("//", cursor, XinxCodeEdit::FindBackward).selectionStart();

	functionName = QString();

	if (!(cursorStartOfComment.isNull() || (!cursorEndOfComment.isNull() && (cursorStartOfComment < cursorEndOfComment))))
		return JsContextType::LONG_COMMENT;
	if (! cursorLineCommented.isNull() && (cursorLineCommented.lineNumber() == cursor.lineNumber()))
		return JsContextType::SHORT_COMMENT;

	QRegExp function("function[\\s]*([a-zA-Z_][a-zA-Z0-9_]*)[\\s]*\\(");
	QDocumentCursor cursorFunction = editor()->textEdit()->find(function, cursor, XinxCodeEdit::FindBackward);
	QDocumentCursor endOfParam     = editor()->textEdit()->find(")", cursor, XinxCodeEdit::FindBackward).selectionStart();

	if (cursorFunction.isNull())
		return JsContextType::NONE;

	QDocumentCursor cursorOfFunctionName = cursorFunction;
	functionName = cursorOfFunctionName.selectedText();
	function.indexIn(functionName, 0);
	functionName = function.cap(1);

	if (endOfParam.isNull() || (endOfParam < cursorFunction))
	{
		return JsContextType::FUNCTION_PARAMS;
	}

	int bloc = 0;
	QDocumentCursor c = editor()->textEdit()->find(")", cursorFunction);
	while ((c < cursor) && (c.position() > 0))
	{
		QString text = c.selectedText();
		if (text.contains('{'))
			bloc++;
		else if (text.contains('}'))
			bloc--;

		c = editor()->textEdit()->find(QRegExp("[\\S]+"), c);
	}

	if (bloc > 0)
		return JsContextType::FUNCTION_CONTENT;
	else
	{
		functionName = QString();
		return JsContextType::NONE;
	}
}

JsContextParser::JsContextParser(): ContextParser()
{

}

JsContextParser::~JsContextParser()
{

}

void JsContextParser::updateContext()
{
	if (!editor()->fileTypePluginInterface()) return;
	if (editor()->fileTypePluginInterface()->name() != "JSFileEditor") return;

	JsContextType * type = new JsContextType;
	QString functionName;
	JsContextType::Position position = editPosition(editor()->textEdit()->textCursor(), functionName);

	type->setPosition(position);
	type->setFunctionName(functionName);

	context().addContext(type);
}

}

}



