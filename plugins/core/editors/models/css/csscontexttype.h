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

#ifndef CORE_CASCADINGSTYLESHEET_CONTEXT_H
#define CORE_CASCADINGSTYLESHEET_CONTEXT_H

#include <codecompletion/context.h>
#include <QHash>
#include <QStack>

#define CSS_CONTEXT_TYPE "CSS"

namespace Core
{

namespace CascadingStyleSheet
{

class CssContextType : public CodeCompletion::ContextType
{
public:
	enum Position
	{
		NONE	= 0,
		COMMENT	= 1,
		TAG		= 2
	};

	CssContextType();
	virtual ~CssContextType();

	Position position();
	void setPosition(Position value);

	virtual QString type() const;
	virtual bool operator==(const ContextType & other) const;
private:
	Position _position;
};

}

}

#endif // CORE_CASCADINGSTYLESHEET_CONTEXT_H
