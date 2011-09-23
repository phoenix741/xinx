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

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QStringList>
#include <QHash>
#include <QMap>
#include <QTimer>

// Std header
#include <exception>

class QWidget;
class XinxErrorMessage;

class LIBEXPORT XinxException : public std::exception
{
public:
	XinxException();
	explicit XinxException(const QString & message);
	XinxException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, const QString & message);
	virtual ~XinxException() throw();

	const QString & assertion() const;
	const QString & locationFile() const;
	int locationLine() const;
	const QString & locationMethod() const;

	const QString & getMessage() const;
	const QStringList & getStack() const;

	virtual const char* what() const throw();
private:
	QString m_message, m_assertion, m_locationFile, m_locationMethod;
	int m_locationLine;
	QStringList m_stack;
};

#define EXCEPT_ELSE(assertion, Exception, method, ...) \
	(assertion ? qt_noop() : throw Exception(#assertion, __FILE__,__LINE__, method, __VA_ARGS__));

#endif /* __EXCEPTIONS_H__ */
