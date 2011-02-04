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

#ifndef CODECOMPLETION_COMPLETER_H
#define CODECOMPLETION_COMPLETER_H

#include <QCompleter>
#include <QScopedPointer>
#include "context.h"

class TextFileEditor;

namespace CodeCompletion {

class Item;
class ItemModelFactory;
class PrivateCompleter;
class Model;

class LIBEXPORT Completer : public QCompleter
{
	Q_OBJECT
public:
	explicit Completer(TextFileEditor* parent = 0);
	virtual ~Completer();
	
	void setModel(Model * model);
	Model * model() const;
	
	void setContext(Context context);
	Context context() const;

	void setEditor(TextFileEditor * editor);
	TextFileEditor * editor() const;
public slots:
	void complete(bool automaticCall = true, const QString & prefix = QString());
	void complete(int row);
	void complete(Item * item);
	void complete(const QModelIndex & index);
protected:
	bool eventFilter(QObject *o, QEvent *e);
private slots:
	void resizeColumns(int, int);
private:
	QScopedPointer<PrivateCompleter> d;
};

}

#endif // CODECOMPLETION_COMPLETER_H
