﻿/*
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

#ifndef CODECOMPLETION_ACTIONITEM_H
#define CODECOMPLETION_ACTIONITEM_H

// Xinx header
#include <codecompletion/item.h>

// Qt header
#include <QApplication>

namespace CodeCompletion
{

class LIBEXPORT ActionItem : public Item
{
	Q_DECLARE_TR_FUNCTIONS(ActionItem)
public:
	explicit ActionItem(const QString & libelle, const QString & code);
	virtual ~ActionItem();
};

}

#endif // CODECOMPLETION_ACTIONITEM_H
