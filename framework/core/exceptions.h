/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef __EXCEPTIONS_H__
#define __EXCEPTIONS_H__
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>

// Qt header
#include <QStringList>
#include <QHash>
#include <QMap>
#include <QTimer>

class QErrorMessage;
class QWidget;
class XinxErrorMessage;

class LIBEXPORT XinxException
{
public:
	XinxException(const QString & message);
	XinxException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, const QString & message);

	const QString & assertion() const;
	const QString & locationFile() const;
	int locationLine() const;
	const QString & locationMethod() const;

	const QString & getMessage() const;
	const QStringList & getStack() const;
private:
	QString m_message, m_assertion, m_locationFile, m_locationMethod;
	int m_locationLine;
	QStringList m_stack;
};

#define EXCEPT_ELSE(assertion, Exception, method, ...) \
	(assertion ? qt_noop() : throw Exception(#assertion, __FILE__,__LINE__, method, __VA_ARGS__));

class LIBEXPORT ExceptionManager : public QObject
{
	Q_OBJECT
public:
	virtual ~ExceptionManager();

	QStringList stackTrace() const;

	QErrorMessage * errorDialog() const;

	static ExceptionManager * self();

	static void installExceptionHandler();

	void notifyError(QString error, QString plainError, QtMsgType t = QtWarningMsg);
signals:
	void errorTriggered();
private:
	ExceptionManager();

	static ExceptionManager * s_self;

	QHash<unsigned long,QStringList> m_stackTrace;
	QErrorMessage * m_dialog;
	QStringList m_exceptionFilter;
};

class LIBEXPORT ErrorManager : public QObject
{
	Q_OBJECT
public:
	enum MessageType
	{
		MessageInformation = 0x01,
		MessageWarning     = 0x02,
		MessageError       = 0x04
	};
	struct Error
	{
		int line;
		MessageType type;
		QString message;
		QStringList parameters;
	};

	virtual ~ErrorManager();

	static ErrorManager * self();

	void addContextTranslation(const QString & context, const QString & translation);
	void removeContextTranslation(const QString & context);

	void clearMessages(const QString & context);
	void addMessage(const QString & context, int line, MessageType t, const QString & message, const QStringList & parameters);
	void addMessage(const QString & context, int line, MessageType t, const XinxException & exception);

	const QMap<QString, QList<Error> > & errors() const;
signals:
	void changed();
private:
	ErrorManager();

	static ErrorManager * s_self;

	QHash<QString,QString> m_contextTranslation;
	QMap<QString, QList<Error> > m_errors;
	QTimer m_timer;
};

#endif /* __EXCEPTIONS_H__ */
