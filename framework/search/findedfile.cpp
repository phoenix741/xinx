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

#include "findedfile.h"
#include "findedline.h"

/* FindedFile */

FindedFile::FindedFile(const QString& filename) : _filename(filename)
{

}

FindedFile::~FindedFile()
{
	clearFindedLine();
}

void FindedFile::setFilename(const QString& value)
{
	_filename = value;
}

const QString& FindedFile::filename() const
{
	return _filename;
}

void FindedFile::addFindedLine(FindedLine* line)
{
	_lines.append(line);
}

void FindedFile::removeFindedLine(int index)
{
	delete _lines.at(index);
	_lines.removeAt(index);
}

void FindedFile::clearFindedLine()
{
	qDeleteAll(_lines);
	_lines.clear();
}

FindedLine* FindedFile::findedLineAt(int index) const
{
	return _lines.at(index);
}

int FindedFile::indexOfFindedLine(FindedLine* line) const
{
	return _lines.indexOf(line);
}

int FindedFile::findedLineSize() const
{
	return _lines.size();
}

FindedFile::const_iterator FindedFile::begin() const
{
	return const_iterator(this, const_iterator::START);
}

FindedFile::const_iterator FindedFile::end() const
{
	return const_iterator(this, const_iterator::END);
}

/* const_iterator */

FindedFile::const_iterator::const_iterator(const FindedFile * list, etype t) : _list(list)
{
	if(t == START)
	{
		_cur = 0;
	}
	if(t == END)
	{
		_cur = _list->findedLineSize();
	}
}

FindedLine* FindedFile::const_iterator::operator*()
{
	return _list->findedLineAt(_cur);
}

bool FindedFile::const_iterator::operator!=(const const_iterator& it)
{
	return (_cur != it._cur);
}

FindedFile::const_iterator & FindedFile::const_iterator::operator++()
{
	_cur++;
	return *this;
}
