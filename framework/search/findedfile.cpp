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

/* FindedFileData */

class FindedFileData : public QSharedData
{
public:
	FindedFileData();
	FindedFileData(const FindedFileData &other);
	~FindedFileData();

	QString _filename;
	QList<FindedLine> _lines;
};

FindedFileData::FindedFileData()
{
}

FindedFileData::FindedFileData(const FindedFileData &other) : QSharedData(other), _filename(other._filename), _lines(other._lines)
{
}

FindedFileData::~FindedFileData()
{
}

/* FindedFile */

FindedFile::FindedFile() : d(new FindedFileData)
{

}

FindedFile::FindedFile(const FindedFile & other) : d(other.d)
{

}

FindedFile::FindedFile(const QString& filename) : d(new FindedFileData)
{
	d->_filename = filename;
}

FindedFile::~FindedFile()
{

}

void FindedFile::setFilename(const QString& value)
{
	d->_filename = value;
}

const QString& FindedFile::filename() const
{
	return d->_filename;
}

void FindedFile::addFindedLine(const FindedLine & line)
{
	d->_lines.append(line);
}

void FindedFile::removeFindedLine(int index)
{
	d->_lines.removeAt(index);
}

void FindedFile::clearFindedLine()
{
	d->_lines.clear();
}

const FindedLine & FindedFile::findedLineAt(int index) const
{
	return d->_lines.at(index);
}

FindedLine FindedFile::findedLineAtLine(int line) const
{
	foreach(const FindedLine & l, d->_lines)
	{
		if (l.line() == line)
			return l;
	}

	return FindedLine();
}

int FindedFile::indexOfFindedLine(const FindedLine & line) const
{
	return d->_lines.indexOf(line);
}

int FindedFile::findedLineSize() const
{
	return d->_lines.size();
}

bool FindedFile::isValid() const
{
	return (!d->_filename.isEmpty()) || (!d->_lines.isEmpty());
}

FindedFile & FindedFile::operator=(const FindedFile& other)
{
	d = other.d;
	return *this;
}

bool FindedFile::operator==(const FindedFile & other) const
{
	return (d->_filename == other.d->_filename) && (d->_lines == other.d->_lines);
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

const FindedLine & FindedFile::const_iterator::operator*()
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
