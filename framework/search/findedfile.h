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

#ifndef FINDEDFILE_H
#define FINDEDFILE_H

#include <QString>
#include <QList>

class FindedLine;

class FindedFile
{
public:
	FindedFile(const QString & filename);
	~FindedFile();

	const QString & filename() const;
	void setFilename(const QString & value);

	void addFindedLine(FindedLine * line);
	void removeFindedLine(int index);
	void clearFindedLine();
	FindedLine * findedLineAt(int index) const;
	int indexOfFindedLine(FindedLine * line) const;
	int findedLineSize() const;

	class const_iterator
	{
	public:
		enum etype { START, END };
		const_iterator(const FindedFile * list, etype t);
		FindedLine* operator*();
		bool operator!=(const const_iterator& it);
		const_iterator & operator++();
	private:
		int _cur;
		const FindedFile* _list;
	};

	const_iterator begin() const;
	const_iterator end() const;
private:
	QString _filename;
	QList<FindedLine*> _lines;
};

#endif // FINDEDFILE_H
