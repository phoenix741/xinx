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

#pragma once
#ifndef _CONTEXTPARSERPOOL_H_
#define _CONTEXTPARSERPOOL_H_

#include <core/xinxsingleton.h>
#include <QScopedPointer>
#include <codecompletion/context.h>

class TextFileEditor;

namespace CodeCompletion
{

class PrivatePool;
class ItemInterface;

class Pool : public XinxLibSingleton<Pool>
{
	Q_OBJECT
public:
	~Pool();

public slots:
	void updateParsers();
	void updateContext(TextFileEditor * editor, Context context);
	void generate(ItemInterface * interface, Context context);
private:
	Q_DISABLE_COPY(Pool)

	Pool();
	friend class XinxLibSingleton<Pool>;

	QScopedPointer<PrivatePool> d;
};

}

#endif /* _CONTEXTPARSERPOOL_H_ */
