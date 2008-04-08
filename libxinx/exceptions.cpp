/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "exceptions.h"

// Qt header
#include <QTextDocument>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QThread>
#include <QApplication>
#include <QErrorMessage>

// Std header for exception
#include <iostream>
#ifndef Q_WS_WIN
#	include <execinfo.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <cxxabi.h>
#endif

/* Log Variable */

ExceptionManager * ExceptionManager::s_self = 0;

/* Trace */

QHash<unsigned long,int> Trace::m_depth;

Trace::Trace( char* filename, int line, const QString & func_name, const QString & args ) {
	m_handle = (unsigned long)QThread::currentThreadId();
	
    LogMsg( m_depth[m_handle], filename, line, QString( "%1%2" ).arg( func_name ).arg( args ) );
	m_depth[m_handle]++;		
}

Trace::~Trace() {
	m_depth[m_handle]--;		
	ExceptionManager::self()->xinxStackTrace()[m_handle].removeLast();
}

void Trace::LogMsg( int depth, const char * filename, int line, const QString & fonction ) {
	QString s;
	s += QDateTime::currentDateTime().toString() + " ";
	for( int i = 0; i < depth; i++ )
		s += " ";
	s += QString("> (%1)").arg( depth );
	s += QString( fonction );
	s += QString( " on %1 at line %2 (thread %3)." ).arg( filename ).arg( line ).arg( m_handle );
	
	ExceptionManager::self()->xinxStackTrace()[m_handle].append( s );

#ifdef Q_WS_WIN
	QFile logfile( QString( "c:\\xinx_debug_%1.log" ).arg( m_handle ) );
#else 
	QFile logfile( QString( "/tmp/xinx_debug_%1.log" ).arg( m_handle ) );
#endif
	if( logfile.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) ) {
		QTextStream logfileStream( &logfile );
		logfileStream << s << endl;
	}
}

/* Message Handler */

static void xinxMessageHandler( QtMsgType t, const char * m ) {
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
        ExceptionManager::self()->setFatal( true );
    }
    rich = QString::fromLatin1("<p><b>%1</b></p>").arg( rich );
    rich += Qt::escape(QLatin1String( m ));

    // ### work around text engine quirk
    if (rich.endsWith(QLatin1String("</p>")))
        rich.chop(4);

    ExceptionManager::self()->notifyError( rich );
}

/* ExceptionManager */

ExceptionManager::ExceptionManager() : m_fatal( false ) {
	m_dialog = new QErrorMessage(0);
	m_dialog->setWindowTitle( qApp->applicationName() );
	qInstallMsgHandler( xinxMessageHandler );
}

ExceptionManager::~ExceptionManager() {
	delete m_dialog;
	if( s_self == this )
		s_self = 0;
}

void ExceptionManager::setFatal( bool value ) { 
	m_fatal = value; 
}

bool ExceptionManager::fatal() const { 
	return m_fatal; 
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

void ExceptionManager::notifyError( QString error, QStringList stack ) {
	if( m_fatal ) { 
		//((ExceptionManager*)0)->m_fatal = 1; 
		emit errorTriggered(); 
	}

	if (stack.isEmpty())
		stack << stackTrace();
	
	// Create a file where write error
	FILE * file = NULL;
#ifndef Q_WS_WIN
	char * filename = "/tmp/xinx_trace.log";
#else
	char * filename = "c:\\xinx_trace.log";
#endif
		
	file = fopen( filename, "w+" );
	if( file ) {
		fprintf( file, "* Backtrace ...\n" );
		fprintf( file, qPrintable( stack.join( "\n" ) ) );
		fprintf( file, "* Error ...\n" );
		fprintf( file, qPrintable( error ) );
		fclose( file );
	}

    if( QThread::currentThread() == qApp->thread() ) 
    	m_dialog->showMessage( error );
    else 
        QMetaObject::invokeMethod( m_dialog, "showMessage", Qt::QueuedConnection, Q_ARG(QString, error));

    if( m_fatal ) m_dialog->exec(); // Pour ne pas quitter de suite
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
	XINX_TRACE( "XinxException", QString("( %1 )").arg( message ) );
	
	m_stack = ExceptionManager::self()->stackTrace(); //[ (unsigned long)QThread::currentThreadId() ];
}

const QString & XinxException::getMessage() const {
	XINX_TRACE( "XinxException::getMessage", "()" );

	return m_message;
}

const QStringList & XinxException::getStack() const {
	XINX_TRACE( "XinxException::getStack", "()" );

	return m_stack;
}


/* XinxAssertException */

XinxAssertException::XinxAssertException( const char *assertion, const char *file, int line ) : XinxException( QString( "ASSERT: \"%1\" in file %2, line %3" ).arg( assertion ).arg( file ).arg( line ) ) {
	XINX_TRACE( "XinxAssertException", QString( "(%1, %2, %3)" ).arg( assertion ).arg( file ).arg( line ) );
}
