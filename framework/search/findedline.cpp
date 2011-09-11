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

#include "findedline.h"

FindedLine::FindedLine(int line, const QString& content) : _line(line), _content(content)
{

}

FindedLine::~FindedLine()
{

}

void FindedLine::setLine(int value)
{
	_line = value;
}

int FindedLine::line() const
{
	return _line;
}

void FindedLine::setContent(const QString& value)
{
	_content = value;
}

const QString& FindedLine::content() const
{
	return _content;
}
