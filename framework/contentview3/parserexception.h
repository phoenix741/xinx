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
#ifndef CONTENTVIEW3_PARSEREXCEPTION_H
#define CONTENTVIEW3_PARSEREXCEPTION_H

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>

namespace ContentView3
{

class LIBEXPORT ParserException : public XinxException
{
public:
	ParserException(QString message, int line, int column = 0);
	ParserException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message, int line, int column = 0);

	int getLine() const;
	int getColumn() const;
private:
	int m_line, m_column;
};


}

#endif // CONTENTVIEW3_PARSEREXCEPTION_H
