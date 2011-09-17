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
#include <QSharedData>

/* FindedLineData */

FindedLineData::FindedLineData() : _line(-1), _posStart(-1), _posEnd(-1)
{
}

FindedLineData::FindedLineData(const FindedLineData &other) : QSharedData(other), _line(other._line), _posStart(other._posStart), _posEnd(other._posEnd), _content(other._content)
{
}

FindedLineData::~FindedLineData()
{
}

/* FindedLine */

FindedLine::FindedLine()
{

}

FindedLine::FindedLine(int line, const QString& content, int posStart, int posEnd) : d(new FindedLineData)
{
	d->_line = line;
	d->_posStart =posStart;
	d->_posEnd = posEnd;
	d->_content = content;
}

FindedLine::FindedLine(const FindedLine & other) : d(other.d)
{

}

FindedLine::~FindedLine()
{

}

void FindedLine::setLine(int value)
{
	d->_line = value;
}

int FindedLine::line() const
{
	return d->_line;
}

void FindedLine::setPosStart(int value)
{
	d->_posStart = value;
}

int FindedLine::posStart() const
{
	return d->_posStart;
}

void FindedLine::setPosEnd(int value)
{
	d->_posEnd = value;
}

int FindedLine::posEnd() const
{
	return d->_posEnd;
}

void FindedLine::setContent(const QString& value)
{
	d->_content = value;
}

const QString& FindedLine::content() const
{
	return d->_content;
}

bool FindedLine::isValid() const
{
	return (!d->_content.isEmpty()) || (d->_line != -1) || (d->_posStart != -1) || (d->_posEnd != -1);
}

bool FindedLine::operator==(const FindedLine & other) const
{
	return (d->_content == other.d->_content) || (d->_line == other.d->_line) || (d->_posStart != other.d->_posStart) || (d->_posEnd != other.d->_posEnd);
}
