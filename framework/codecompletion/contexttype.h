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

#ifndef CODECOMPLETION_CONTEXTTYPE_H
#define CODECOMPLETION_CONTEXTTYPE_H

// Xinx header
#include <core/lib-config.h>

namespace CodeCompletion
{

class PrivateContext;

class LIBEXPORT ContextType
{
public:
	ContextType();
	virtual ~ContextType();

	virtual QString type() const = 0;
	virtual bool operator==(const ContextType & other) const;
private:
	Q_DISABLE_COPY(ContextType)
};

}

#endif // CODECOMPLETION_CONTEXTTYPE_H
