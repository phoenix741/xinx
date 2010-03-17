/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
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

class QErrorMessage;
class QWidget;
class XinxErrorMessage;

/*!
 * Base class for exceptions in XINX. This base class include a message in the exception, and save
 * the stack.
 */
class LIBEXPORT XinxException
{
public:
	XinxException(QString message);
	/*!
	 * Return the message.
	 * \return The message of the error.
	 */
	const QString & getMessage() const;
	/*!
	 * Return the stack trace.
	 * \return The stack trace
	 */
	const QStringList & getStack() const;
private:
	QString m_message;
	QStringList m_stack;
};

/*!
 * The Exception manager contains an error dialog that can be used when a error occure or when we
 * use qWarning, qCritical, qError, qDebug. When an fatal error occure, the signal \e errorTriggered
 * is emited.
 */
class LIBEXPORT ExceptionManager : public QObject
{
	Q_OBJECT
public:
	virtual ~ExceptionManager();

	/*!
	 * Stack trace generated by the Operating System. The application must be compiled with
	 * -rdynamic in Gnu/Linux. This stack is unused on Windows.
	 */
	QStringList stackTrace() const;

	/*!
	 * The dialog created and used to show message. This dialog is automatically called when Qt
	 * macro (qDebug, ...) is used.
	 */
	QErrorMessage * errorDialog() const;

	/*!
	 * Return the exception manager of the application and created it if needed.
	 */
	static ExceptionManager * self();

	/*!
	 * Install the Exception Handler but doesn't create the exception manager if not needed.
	 */
	static void installExceptionHandler();

	/*!
	 *Show a dialog and save the error in a trace file.
	 * \param error Message to store in the file
	 * \param stack Stack to store when the error occure.
	 */
	void notifyError(QString error, QString plainError, QtMsgType t = QtWarningMsg);
signals:
	/*!
	 * Signal emited when \e notifyError is called and the exception manager is set to be fatal.
	 */
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
	enum MessageType {
		MessageInformation = 0x01,
		MessageWarning     = 0x02,
		MessageError       = 0x04
	};
	struct Error
	{
		MessageType type;
		QString message;
		QStringList parameters;
	};

	virtual ~ErrorManager();

	static ErrorManager * self();

	void clearMessages(const QString & context);
	void addMessage(const QString & context, MessageType t, const QString & message, const QStringList & parameters);
	void addMessage(const QString & context, MessageType t, const XinxException & exception);

	const QMap<QString, QList<Error> > & errors() const;
signals:
	void changed();
private:
	ErrorManager();

	static ErrorManager * s_self;

	QMap<QString, QList<Error> > m_errors;
};

#endif /* __EXCEPTIONS_H__ */
