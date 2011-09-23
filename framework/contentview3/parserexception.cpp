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

#include "parserexception.h"

namespace ContentView3
{

/* ParserException */

/*!
* \ingroup ContentView3
* \class ParserException
* \brief Exception throw when the model can't be updated.
*/

/*!
* \brief Create the exception with a message and a line.
* \param message Error of the exception.
* \param line Line where the error is.
* \param column Column where the error is.
*/
ParserException::ParserException(QString message, int line, int column) : XinxException(message), m_line(line), m_column(column)
{

}

ParserException::ParserException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message, int line, int column) : XinxException(assertion, locationFile, locationLine, locationMethod, message), m_line(line), m_column(column)
{

}

/*!
* \brief Return the line where the error is.
* \return The line of the error.
*/
int ParserException::getLine() const
{
	return m_line;
}

/*!
* \brief Return the column where the error is.
* \return the column of the error.
*/
int ParserException::getColumn() const
{
	return m_column;
}

}
