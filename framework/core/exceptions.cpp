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

// Xinx header
#include "core/exceptions.h"
#include "exceptionmanager.h"

/* XinxException */

/*!
 * \class XinxException
 * \brief Base class for exceptions in XINX.
 *
 * This base class include a message in the exception, and save the stack.
 */

XinxException::XinxException() : m_locationLine(-1)
{

}

XinxException::XinxException(const QString & message) : m_message(message), m_locationLine(-1)
{
	m_stack = ExceptionManager::self()->stackTrace(); //[ (unsigned long)QThread::currentThreadId() ];
}

XinxException::XinxException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, const QString & message)
		: m_message(message), m_assertion(assertion), m_locationFile(locationFile), m_locationMethod(locationMethod), m_locationLine(locationLine)
{
	m_stack = ExceptionManager::self()->stackTrace(); //[ (unsigned long)QThread::currentThreadId() ];
}

XinxException::~XinxException() throw()
{

}

/*!
 * Return the message.
 * \return The message of the error.
 */
const QString & XinxException::getMessage() const
{
	return m_message;
}

/*!
 * Return the message in the stl form.
 * \return pointer to the error message.
 */
const char* XinxException::what() const throw()
{
	return qPrintable(m_message);
}

/*!
 * Return the test made that made the error
 */
const QString & XinxException::assertion() const
{
	return m_assertion;
}

/*!
 * Return the location (method name) of the error
 */
const QString & XinxException::locationFile() const
{
	return m_locationFile;
}

int XinxException::locationLine() const
{
	return m_locationLine;
}

const QString & XinxException::locationMethod() const
{
	return m_locationMethod;
}

/*!
 * Return the stack trace.
 * \return The stack trace
 */
const QStringList & XinxException::getStack() const
{
	return m_stack;
}
