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

#ifndef FINDEDLINE_H
#define FINDEDLINE_H

#include <QString>
#include <QSharedData>

class FindedLineData : public QSharedData
{
public:
	FindedLineData();
	FindedLineData(const FindedLineData &other);
	~FindedLineData();

	int _line, _posStart, _posEnd;
	QString _content;
};

class FindedLine
{
public:
	FindedLine();
	FindedLine(const FindedLine & other);
	FindedLine(int line, const QString& content, int posStart, int posEnd);
	~FindedLine();

	void setLine(int value);
	int line() const;

	void setPosStart(int value);
	int posStart() const;

	void setPosEnd(int value);
	int posEnd() const;

	void setContent(const QString & value);
	const QString & content() const;

	bool isValid() const;

	bool operator==(const FindedLine & other) const;
private:
	QExplicitlySharedDataPointer<FindedLineData> d;
};

#endif // FINDEDLINE_H
