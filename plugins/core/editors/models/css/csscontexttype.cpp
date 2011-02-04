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

#include "csscontexttype.h"

namespace Core
{
	
namespace CascadingStyleSheet
{
	
/* CssContextType */

CssContextType::CssContextType()
{

}

CssContextType::~CssContextType()
{

}

QString CssContextType::type() const
{
	return CSS_CONTEXT_TYPE;
}

void CssContextType::setPosition(CssContextType::Position value)
{
	_position = value;
}

CssContextType::Position CssContextType::position()
{
	return _position;
}

bool CssContextType::operator==(const CodeCompletion::ContextType& other) const
{
	if (CodeCompletion::ContextType::operator==(other))
	{
		const CssContextType & t = dynamic_cast<const CssContextType &>(other);
		return (t._position == _position);
	}
	else
	{
		return false;
	}
}

}

}

