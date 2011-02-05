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
#include <core/xinxcore.h>
#include <core/xinxsingleton.h>

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

class LIBEXPORT ExceptionManager : public XinxLibSingleton<ExceptionManager>
{
	Q_OBJECT
public:
	virtual ~ExceptionManager();

	QStringList stackTrace() const;

	static void installExceptionHandler();
public slots:
	void notifyError(QString error, QtMsgType t = QtWarningMsg, bool showMessage = true);
signals:
	void errorTriggered();
private:
	ExceptionManager();

	QHash<unsigned long,QStringList> m_stackTrace;
	QStringList m_exceptionFilter;
	friend class XinxLibSingleton<ExceptionManager>;
};

class LIBEXPORT ErrorManager : public XinxLibSingleton<ErrorManager>
{
	Q_OBJECT
public:
	struct Error
	{
		int line;
		QtMsgType type;
		QString message;
		QStringList parameters;

		bool operator==(const Error & e) const
		{
			return (line == e.line) && (type == e.type) && (message == e.message) && (parameters == e.parameters);
		}
	};

	virtual ~ErrorManager();

	const QMap<QString, QList<Error> > & errors() const;
public slots:
	void addContextTranslation(const QString & context, const QString & translation);
	void removeContextTranslation(const QString & context);

	void clearMessages(const QString & context);
	void addMessage(const QString & context, int line, QtMsgType t, const QString & message, const QStringList & parameters = QStringList());
	void addMessage(const QString & context, int line, QtMsgType t, const XinxException & exception);
signals:
	void changed();
private:
	ErrorManager();

	QHash<QString,QString> m_contextTranslation;
	QMap<QString, QList<Error> > m_errors;
	QTimer m_timer;
	friend class XinxLibSingleton<ErrorManager>;
};

#endif /* __EXCEPTIONS_H__ */
