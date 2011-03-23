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
#include "core/xinxconfig.h"
#include <core/version.h>
#include <editors/editormanager.h>
#include <session/sessionmanager.h>

// Qt header
#include <QTextDocument>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QThread>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <QFileInfo>
#include <QProcess>

// Std header for exception
#include <iostream>
#include <csignal>
#ifndef Q_WS_WIN
#   include <execinfo.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <cxxabi.h>
#endif

/* Signal Handler */

static void backup_appli_signal(int signal)
{
	std::signal(SIGSEGV, SIG_DFL);
	std::signal(SIGABRT, SIG_DFL);
	//std::signal(SIGINT, SIG_DFL);
	//std::signal(SIGTERM, SIG_DFL);

	const QString thread  = QThread::currentThread () == qApp->thread () ? ExceptionManager::tr("GUI") : QString::number ((qulonglong)QThread::currentThreadId (), 16);
	QString signalStr;
	switch(signal)
	{
	case SIGINT		: signalStr = ExceptionManager::tr("Interactive attention");		break;
	case SIGILL		: signalStr = ExceptionManager::tr("Illegal instruction");			break;
	case SIGFPE		: signalStr = ExceptionManager::tr("Floating point error");			break;
	case SIGSEGV	: signalStr = ExceptionManager::tr("Segmentation violation");		break;
	case SIGTERM	: signalStr = ExceptionManager::tr("Termination request");			break;
	case SIGABRT	: signalStr = ExceptionManager::tr("Abnormal termination (abort)");	break;
	}

	const QString message = ExceptionManager::tr("Signal %1 emited in thread %2").arg (signalStr).arg (thread);
	ExceptionManager::self ()->notifyError(message, QtFatalMsg, true);
}

/* Message Handler */

static void logError(QString error)
{
	// Create a file where write error
	QFile file(XINXConfig::self()->config().xinxTrace);
	if (file.open(QIODevice::Append))
	{
		QTextStream text(&file);
		text << QDateTime::currentDateTime().toString();
		text << " : ";
		text << error;
		text << "\n";
		file.close();
	}

	std::cout << qPrintable(error) << std::endl;
}

static void xinxMessageHandler(QtMsgType t, const char * m)
{
#ifdef _XINX_RELEASE_MODE_
	if (t == QtDebugMsg) return;
#endif /* _XINX_RELEASE_MODE_ */

	QString error;

	switch (t)
	{
	case QtDebugMsg:
	default:
		error = ExceptionManager::tr("DEBUG");
		break;
	case QtWarningMsg:
		error = ExceptionManager::tr("WARNING");
		break;
	case QtFatalMsg:
		error = ExceptionManager::tr("FATAL");
	}

	error += " : " + QLatin1String(m);

	if (ExceptionManager::directSelf())
	{
		ExceptionManager::directSelf()->notifyError(error, t, t != QtDebugMsg);
	}
	else
	{
		logError(error);

		if (t == QtFatalMsg)
		{
			logError(ExceptionManager::tr("Logger not started"));

#			ifndef _XINX_RELEASE_MODE_
			abort();
#			else
			exit(254);
#			endif /* _XINX_RELEASE_MODE_ */
		}
	}
}

/* ExceptionManager */

/*!
 * \class ExceptionManager
 * \brief The Exception manager contains an error dialog that can be used when a error occure or when we
 * use qWarning, qCritical, qError, qDebug. When an fatal error occure, the signal \e errorTriggered
 * is emited.
 */

/*!
 * \fn void ExceptionManager::errorTriggered(const QString & message);
 * \brief Signal emited when a FATAL error occured.
 *
 * Signal emited when \e notifyError is called and the exception manager is set to be fatal.
 * \param message The message that terminate XINX. Slot called by this signal must be quick and
 * small.
 */

ExceptionManager::ExceptionManager()
{
	QFile exceptionFilterFile(":/rc/exceptionfilter.txt");
	if (exceptionFilterFile.open(QIODevice::ReadOnly))
	{
		QTextStream exceptionFilterStream(&exceptionFilterFile);
		m_exceptionFilter = exceptionFilterStream.readAll().split("\n");
	}
}

ExceptionManager::~ExceptionManager()
{

}

/*!
 * \brief Install the Exception Handler but doesn't create the exception manager if not needed.
 */
void ExceptionManager::installExceptionHandler()
{
	qInstallMsgHandler(xinxMessageHandler);
}

/*!
 * \brief Install the signal handler (must be called in each thread) to transform signal into
 * exception.
 */
void ExceptionManager::installSignalHandler()
{
	std::signal(SIGSEGV, backup_appli_signal);
	std::signal(SIGABRT, backup_appli_signal);
	//std::signal(SIGINT, backup_appli_signal);
	//std::signal(SIGTERM, backup_appli_signal);
}

/*!
 * \brief Stack trace generated by the Operating System. The application must be compiled with
 * -rdynamic in Gnu/Linux. This stack is unused on Windows.
 */
QStringList ExceptionManager::stackTrace() const
{
	QStringList stack;

#ifndef Q_WS_WIN
	void * array[10];
	size_t size, i;
	char ** strings;

	size = backtrace(array, 10);
	strings = backtrace_symbols(array, size);
	for (i = 0; i < size; i++)
	{
		stack << strings[i];
	}
	free(strings);
#endif
	return stack;
}

void ExceptionManager::openTicketDialog(const QString& message, const QStringList& stack) const
{
	if (!QProcess::startDetached("xinxcrash", QStringList() << "--version" << VERSION << "--message" << message << "--stack" << stack.join(";;")))
	{
		QMessageBox::critical(qApp->activeWindow(), tr("XINX Crash"), tr("XINX Crash with the following messsage : %1. XINX can't open the assistant to help you to create a ticket, you must create ticket on XINX site manually. Thanks'").arg(message));
	}
}

/*!
 * \brief Show a dialog and save the error in a trace file.
 * \param error Message to store in the file
 * \param t The message type.
 * \param showMessage If set to false, the message isn't show to the user. If the \e t is QtFatalMsg the message is always show.
 */
void ExceptionManager::notifyError(QString error, QtMsgType t, bool showMessage)
{
	foreach(const QString & filter, m_exceptionFilter)
	{
		if (QRegExp(filter).exactMatch(error))
			return;
	}

	logError(error);

	if (t == QtFatalMsg)
	{
		// On restore les signaux pour eviter d'etre perturbe pendant la phase de sauvegarde ...
		std::signal(SIGSEGV, SIG_DFL);
		std::signal(SIGABRT, SIG_DFL);
		std::signal(SIGINT, SIG_DFL);
		std::signal(SIGTERM, SIG_DFL);

		emit errorTriggered(error);


		ExceptionManager::self()->openTicketDialog(error, stackTrace());
		XinxSession::SessionManager::self()->createRecoverSession();


#ifndef _XINX_RELEASE_MODE_
		abort();
#else
		exit(254);
#endif /* _XINX_RELEASE_MODE_ */
	}
	else
	{
		if (showMessage)
		{
			ErrorManager::self()->addMessage(qApp->applicationName(), -1, t, error);
		}
	}
}

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

/* ErrorManager */

ErrorManager::ErrorManager()
{
	qRegisterMetaType<QtMsgType>("QtMsgType");
	qRegisterMetaType<XinxException>("XinxException");

	m_timer.setSingleShot(true);
	m_timer.setInterval(500);
	connect(&m_timer, SIGNAL(timeout()), this, SIGNAL(changed()));
}

ErrorManager::~ErrorManager()
{

}

void ErrorManager::addContextTranslation(const QString & context, const QString & translation)
{
	m_contextTranslation.insert(context, translation);
}

void ErrorManager::removeContextTranslation(const QString & context)
{
	m_contextTranslation.remove(context);
}

void ErrorManager::clearMessages(const QString & context)
{
	const QString ctxt = m_contextTranslation.value(context).isEmpty() ? context : m_contextTranslation.value(context);

	m_errors.remove(ctxt);

	if (QThread::currentThread() == qApp->thread())
		m_timer.start();
	else
		QMetaObject::invokeMethod(&m_timer, "start", Qt::BlockingQueuedConnection);
}

void ErrorManager::addMessage(const QString & context, int line, QtMsgType t, const QString & message, const QStringList & parameters)
{
	const QString ctxt = m_contextTranslation.value(context).isEmpty() ? context : m_contextTranslation.value(context);

	struct Error e = {line, t, message, parameters};
	if (!m_errors[ctxt].contains(e))
		m_errors[ctxt].append(e);

	if (QThread::currentThread() == qApp->thread())
		m_timer.start();
	else
		QMetaObject::invokeMethod(&m_timer, "start", Qt::BlockingQueuedConnection);
}

void ErrorManager::addMessage(const QString & context, int line, QtMsgType t, const XinxException & exception)
{
	const QString ctxt = m_contextTranslation.value(context).isEmpty() ? context : m_contextTranslation.value(context);

	struct Error e = {line, t, exception.getMessage(), QStringList()};
	if (!m_errors[ctxt].contains(e))
		m_errors[ctxt].append(e);

	if (QThread::currentThread() == qApp->thread())
		m_timer.start();
	else
		QMetaObject::invokeMethod(&m_timer, "start", Qt::BlockingQueuedConnection);
}

const QMap<QString, QList<ErrorManager::Error> > & ErrorManager::errors() const
{
	return m_errors;
}
