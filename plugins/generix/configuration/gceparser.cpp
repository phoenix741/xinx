/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#include "gceparser.h"

GceParser::GceParser() : _interface(0)
{

}

GceParser::GceParser(const QString & filename) : _interface(0), _filename(filename)
{

}

GceParser::~GceParser()
{

}

void GceParser::setInterface(QSharedPointer<GceConfiguration> interface)
{
	_interface = interface;
}

QSharedPointer<GceConfiguration> GceParser::interface() const
	{
		return _interface;
	}

QString GceParser::description() const
{
	return tr("Parse the configuration file %1").arg(_filename);
}

