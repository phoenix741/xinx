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

#ifndef CONTENTVIEW3_CACHEEXCEPTION_H
#define CONTENTVIEW3_CACHEEXCEPTION_H

// Xinx header
#include <core/exceptions.h>

// Qt header
#include <QString>

namespace ContentView3
{

class LIBEXPORT CacheParserNotFoundException : public XinxException
{
public:
	CacheParserNotFoundException(const QString& assertion, const QString& locationFile, int locationLine, const QString& locationMethod, const QString& message, const QString& filename);
	virtual ~CacheParserNotFoundException() throw();
};

}

#endif // CONTENTVIEW3_CACHEEXCEPTION_H
