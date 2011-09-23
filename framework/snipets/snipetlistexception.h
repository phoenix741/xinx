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
#ifndef __SNIPETLISTEXCEPTION_H__
#define __SNIPETLISTEXCEPTION_H__

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>

// Qt header
#include <QString>

/*!
* \class SnipetListException
* The snipet list exception, is throw when Snipet list load or save file.
*/
class LIBEXPORT SnipetListException : public XinxException
{
public:
	/*!
	* Exception throw by the Snipet list.
	* \param message Message of the error.
	*/
	SnipetListException(const QString & message);
};

#endif // __SNIPETLISTEXCEPTION_H__
