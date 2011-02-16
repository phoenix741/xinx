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

#ifndef CODECOMPLETION_SNIPETITEM_H
#define CODECOMPLETION_SNIPETITEM_H

// Xinx header
#include <codecompletion/model.h>

// Qt header
#include <QApplication>

class TextFileEditor;

namespace CodeCompletion
{

class PrivateSnipetItem;

class SnipetItem : public CodeCompletion::ActionItem
{
	Q_DECLARE_TR_FUNCTIONS(SnipetItem)
public:
	SnipetItem(const QString& snipet, const QString & name);
	virtual ~SnipetItem();

	virtual void execute(Context context, TextFileEditor* editor);
private:
	PrivateSnipetItem * d;
};

}

#endif // CODECOMPLETION_SNIPETITEM_H
