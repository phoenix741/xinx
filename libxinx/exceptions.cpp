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
	
	QFile exceptionFilterFile( ":/rc/exceptionfilter.txt" );
	if( exceptionFilterFile.open( QIODevice::ReadOnly ) ) {
		QTextStream exceptionFilterStream( &exceptionFilterFile );
		m_exceptionFilter = exceptionFilterStream.readAll().split( "\n" );
	}
	
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
		
	file = fopen( filename, "a" );
	if( file ) {
		fprintf( file, qPrintable( QDateTime::currentDateTime().toString() ) );
		fprintf( file, "\n" );
		fprintf( file, "* Backtrace ...\n" );
		fprintf( file, qPrintable( stack.join( "\n" ) ) );
		fprintf( file, "* Error ...\n" );
		fprintf( file, qPrintable( error ) );
		fclose( file );
	}

	foreach( QString filter, m_exceptionFilter ) {
		if( QRegExp( filter ).exactMatch( error ) )
			return;
	}
	
	if( m_fatal && ( QThread::currentThread() == qApp->thread() ) ) 
    	m_dialog->showMessage( error );
    else  
    	QMetaObject::invokeMethod( m_dialog, "showMessage", Qt::QueuedConnection, Q_ARG(QString, error));
	
	std::cerr << qPrintable( error ) << std::endl;
	
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
	m_stack = ExceptionManager::self()->stackTrace(); //[ (unsigned long)QThread::currentThreadId() ];
}

const QString & XinxException::getMessage() const {
	return m_message;
}

const QStringList & XinxException::getStack() const {
	return m_stack;
}
