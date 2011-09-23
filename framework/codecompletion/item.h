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

#ifndef CODECOMPLETION_ITEM_H
#define CODECOMPLETION_ITEM_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QStandardItem>

class QDocumentCursor;
class TextFileEditor;

namespace CodeCompletion
{

class PrivateItem;
class Context;

/*
* FIXME: Documentation
*/
class LIBEXPORT Item : public QStandardItem
{
public:
	Item();
	virtual ~Item();

	void setCompletionText(const QString & value);
	QString completionText() const;

	void setCompletionType(const QString & value);
	QString completionType();

	void setCompletionHelper(const QString & value);
	QString completionHelper() const;

	const QString & contextType() const;
	void setContextType(const QString & value);

	const QString & keyString() const;
	void setKeyString(const QString & value);

	virtual void insertCompletionStart(Context context, TextFileEditor* editor, QDocumentCursor & cursor);
	virtual void insertCompletionEnd(Context context, TextFileEditor* editor, QDocumentCursor & cursor);
	virtual void execute(Context context, TextFileEditor* editor);
private:
	QScopedPointer<PrivateItem> d;
};


}

#endif // CODECOMPLETION_ITEM_H
