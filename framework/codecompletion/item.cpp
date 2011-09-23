/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
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

#include "item.h"
#include <codecompletion/context.h>
#include <qdocumentcursor.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>

namespace CodeCompletion
{

/* PrivateItem */

class PrivateItem
{
	public:
		QString _contextType, _keyString;
};

/* Item */

Item::Item() : d(new PrivateItem)
{
}

Item::~Item()
{

}

void Item::setCompletionText(const QString & value)
{
	setData(value, Qt::UserRole + 1);
	if (text().isEmpty())
	{
		setText(value);
	}
}

QString Item::completionText() const
{
	return data(Qt::UserRole + 1).toString();
}

void Item::setCompletionType(const QString & value)
{
	setData(value, Qt::UserRole + 2);
}

QString Item::completionType()
{
	return data(Qt::UserRole + 2).toString();
}

void Item::setCompletionHelper(const QString & value)
{
	setData(value, Qt::UserRole + 3);
}

QString Item::completionHelper() const
{
	return data(Qt::UserRole + 3).toString();
}

void Item::setContextType(const QString& value)
{
	d->_contextType = value;
}

const QString& Item::contextType() const
{
	return d->_contextType;
}

const QString & Item::keyString() const
{
	return d->_keyString;
}

void Item::setKeyString(const QString & value)
{
	d->_keyString = value;
}

void Item::insertCompletionStart(Context context, TextFileEditor* editor, QDocumentCursor & cursor)
{
	Q_UNUSED(cursor);
	// The default implementation do nothing
}

void Item::insertCompletionEnd(Context context, TextFileEditor* editor, QDocumentCursor & cursor)
{
	Q_UNUSED(cursor);
	// The default implementation do nothing
}

void Item::execute(Context context, TextFileEditor* editor)
{
	const QString completionForm		= completionHelper();
	const QString completionFormStart	= completionForm.section("%1",0, 0);
	const QString completionFormEnd		= completionForm.section("%1",1, 1);
	const QString value					= completionText();

	QDocumentCursor tc = editor->textEdit()->textCursor();
	tc.setAutoUpdated(true);

	bool insertCompletion = true;
	QDocumentCursor completionStartCursor(tc);
	completionStartCursor.movePosition(completionFormStart.length(), QDocumentCursor::PreviousCharacter, QDocumentCursor::KeepAnchor);
	if (completionStartCursor.selectedText() == completionFormStart)
	{
		QDocumentCursor tc2(tc);
		tc2.movePosition(completionFormStart.length(), QDocumentCursor::PreviousCharacter);
		insertCompletionStart(context, editor, tc2);
		insertCompletion = false;
	}
	else
	{
		insertCompletionStart(context, editor, tc);
		tc.insertText(completionFormStart);
	}

	tc.insertText(value);

	if (insertCompletion)
	{
		tc.insertText(completionFormEnd);
		insertCompletionEnd(context, editor, tc);
	}

	editor->textEdit()->setTextCursor(tc);
}

}