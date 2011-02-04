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

#include "snipetcontexttype.h"

namespace CodeCompletion {

/* PrivateSnipetContextType */

class PrivateSnipetContextType : public QSharedData
{
public:
    bool _load_snipet;
};

/* SnipetContextType */
	
SnipetContextType::SnipetContextType() : d(new PrivateSnipetContextType)
{

}

SnipetContextType::~SnipetContextType()
{

}

void SnipetContextType::setLoadSnipet(bool value)
{
	d->_load_snipet = value;
}

bool SnipetContextType::loadSnipet() const
{
	return d->_load_snipet;
}

QString SnipetContextType::type() const
{
	return SNIPET_CONTEXT_TYPE;
}

bool SnipetContextType::operator==(const CodeCompletion::ContextType& other) const
{	
    if (CodeCompletion::ContextType::operator==(other))
	{
		const SnipetContextType & t = dynamic_cast<const SnipetContextType &>(other);
		return t.d->_load_snipet == d->_load_snipet;
	}
	else
	{
		return false;
	}
}

}
