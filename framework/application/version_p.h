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

#include "version.h"

class VersionData : public QSharedData
{
public:
	VersionData() : _major(-1), _minor(-1), _micro(-1), _build(-1) { }
	VersionData(const VersionData &other) : QSharedData(other), _major(other._major), _minor(other._minor), _micro(other._micro), _build(other._build) { }
	~VersionData() { }

	void updateFromString(const QString & version, const QString & separator = QString(".-,"));

	int _major, _minor, _micro, _build;
};
