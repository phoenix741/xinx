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
#include "csscontextparser.h"
#include <editors/xinxcodeedit.h>

// QCodeEdit header
#include <qdocumentcursor.h>
#include <editors/textfileeditor.h>
#include <editors/widgeteditor/html/htmlfileeditor.h>
#include <editors/widgeteditor/xsl/xslfileeditor.h>

namespace Core
{

namespace CascadingStyleSheet
{

CssContextType::Position CssContextParser::editPosition(const QDocumentCursor& cursor) const
{
	QDocumentCursor cursorStartOfComment = editor()->textEdit()->find("/*", cursor, XinxCodeEdit::FindBackward).selectionStart();
	QDocumentCursor cursorEndOfComment   = editor()->textEdit()->find("*/", cursor, XinxCodeEdit::FindBackward).selectionStart();

	bool inComment =
		(!(cursorStartOfComment.isNull() || (!cursorEndOfComment.isNull() && (cursorStartOfComment < cursorEndOfComment))));

	if (inComment)
		return CssContextType::COMMENT;

	int bloc = 0;
	QDocumentCursor c = editor()->textEdit()->find("{", cursor, XinxCodeEdit::FindBackward);
	while (c < cursor)
	{
		QString text = c.selectedText();
		if (text.contains('{'))
			bloc++;
		else if (text.contains('}'))
			bloc--;

		c = editor()->textEdit()->find(QRegExp("[\\S]+"), c);
	}

	if (bloc > 0)
		return CssContextType::TAG;
	else
	{
		return CssContextType::NONE;
	}
}

CssContextParser::CssContextParser(): ContextParser()
{

}

CssContextParser::~CssContextParser()
{

}

void CssContextParser::updateContext()
{
	if (!editor()->fileTypePluginInterface()) return;
	if (editor()->fileTypePluginInterface()->name() != "CSSFileEditor") return;

	CssContextType * type = new CssContextType;
	CssContextType::Position position = editPosition(editor()->textEdit()->textCursor());

	type->setPosition(position);

	context().addContext(type);
}

}

}



