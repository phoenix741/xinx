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

// Xinx header
#include "core/exceptions.h"
#include "core/xinxconfig.h"

// Qt header
#include <QTextDocument>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QThread>
#include <QApplication>
#include <QErrorMessage>
#include <QFile>
#include <QTextStream>

// Std header for exception
#include <iostream>
#include <csignal>
#ifndef Q_WS_WIN
#	include <execinfo.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <cxxabi.h>
#endif

/* Log Variable */

ExceptionManager * ExceptionManager::s_self = 0;

/* Message Handler */

static void xinxMessageHandler( QtMsgType t, const char * m ) {
#ifdef _XINX_RELEASE_MODE_
	if( t == QtDebugMsg ) return;
#endif /* _XINX_RELEASE_MODE_ */

	QString rich;

	switch (t) {
	case QtDebugMsg:
	default:
		rich = QErrorMessage::tr("Debug Message:");
		break;
	case QtWarningMsg:
		rich = QErrorMessage::tr("Warning:");
		break;
	case QtFatalMsg:
		rich = QErrorMessage::tr("Fatal Error:");
	}
	rich = QString::fromLatin1("<p><b>%1</b></p>").arg( rich );
	rich += Qt::escape(QLatin1String( m ));

	// ### work around text engine quirk
	if (rich.endsWith(QLatin1String("</p>")))
			rich.chop(4);

	ExceptionManager::self()->notifyError( rich, t );
}

/* ExceptionManager */

ExceptionManager::ExceptionManager() {
	m_dialog = new QErrorMessage(0);
	m_dialog->setWindowTitle( qApp->applicationName() );

	QFile exceptionFilterFile( ":/rc/exceptionfilter.txt" );
	if( exceptionFilterFile.open( QIODevice::ReadOnly ) ) {
		QTextStream exceptionFilterStream( &exceptionFilterFile );
		m_exceptionFilter = exceptionFilterStream.readAll().split( "\n" );
	}
}

ExceptionManager::~ExceptionManager() {
	delete m_dialog;
	if( s_self == this )
		s_self = 0;
}

void ExceptionManager::installExceptionHandler() {
	qInstallMsgHandler( xinxMessageHandler );
}

QHash<unsigned long,QStringList> & ExceptionManager::xinxStackTrace() {
	return m_stackTrace;
}

QStringList ExceptionManager::stackTrace() const {
	QStringList stack;

#ifndef Q_WS_WIN
	void * array[10];
	size_t size, i;
	char ** strings;

	size = backtrace( array, 10 );
	strings = backtrace_symbols( array, size );
	for( i = 0; i < size; i++ ) {
		stack << strings[i];
	}
	free( strings );
#endif
	return stack;
}

QErrorMessage * ExceptionManager::errorDialog() const {
	return m_dialog;
}

void ExceptionManager::notifyError( QString error, QtMsgType t ) {
	if( t == QtFatalMsg ) {
		// On restore les signaux pour �viter d'�tre perturb� pendant la phase de sauvegarde ...
		std::signal(SIGSEGV, SIG_DFL);
		std::signal(SIGABRT, SIG_DFL);
		std::signal(SIGINT, SIG_DFL);
		std::signal(SIGTERM, SIG_DFL);
		//((ExceptionManager*)0)->m_fatal = 1;
		emit errorTriggered();
	}

	foreach( const QString & filter, m_exceptionFilter ) {
		if( QRegExp( filter ).exactMatch( error ) )
			return;
	}

	QStringList stack = stackTrace();

	// Create a file where write error
	QFile file( XINXConfig::self()->config().xinxTrace );
	if( file.open( QIODevice::Append ) ) {
		QTextStream text( &file );
		text << "<br/><u><i>";
		text << QDateTime::currentDateTime().toString();
		text << "</i></u><br/>\n";
		text << "<b>Backtrace :</b><br/>\n";
		text << "<p>\n";
		text << stack.join( "<br/>\n" );
		text << "</p>\n";
		text << error.replace( "\n", "<br/>\n" );;
		file.close();
	}

	std::cout << qPrintable( error ) << std::endl;

	if( t == QtDebugMsg ) return;

	if( QThread::currentThread() == qApp->thread() )
		m_dialog->showMessage( error );
	else
		QMetaObject::invokeMethod( m_dialog, "showMessage", Qt::QueuedConnection, Q_ARG(QString, error));

	if( t == QtFatalMsg ) {
		if( QThread::currentThread() == qApp->thread()  ) {
			m_dialog->exec(); // Pour ne pas quitter de suite
		} else {
			QMetaObject::invokeMethod( m_dialog, "exec", Qt::BlockingQueuedConnection, Q_ARG(QString, error));
		}
		abort();
	}
}

ExceptionManager * ExceptionManager::self() {
	if( ! s_self ) {
		s_self = new ExceptionManager();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}


/* XinxException */

XinxException::XinxException( QString message ) : m_message( message ) {
	m_stack = ExceptionManager::self()->stackTrace(); //[ (unsigned long)QThread::currentThreadId() ];
}

const QString & XinxException::getMessage() const {
	return m_message;
}

const QStringList & XinxException::getStack() const {
	return m_stack;
}