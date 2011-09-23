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

#ifndef CODECOMPLETION_COMPLETIONMODEL_H
#define CODECOMPLETION_COMPLETIONMODEL_H

// Xinx header
#include <contentview3/node.h>

// Qt header
#include <QStandardItemModel>
#include <QApplication>

class QDocumentCursor;
class TextFileEditor;

namespace CodeCompletion
{

class Context;
class PrivateItem;
class PrivateModel;
class PrivateContentViewNodeItem;
class ItemModelFactory;
class Item;

class LIBEXPORT Model : public QStandardItemModel
{
	Q_OBJECT
public:
	explicit Model(QObject* parent = 0);
	virtual ~Model();

	Context context() const;
	void setContext(Context context);

	void updateModel();
private:

	QScopedPointer<PrivateModel> d;
	friend class Item;
};

}

#endif // CODECOMPLETION_COMPLETIONMODEL_H
