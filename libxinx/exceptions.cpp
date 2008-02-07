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
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QThread>

/* Log Variable */

QHash<unsigned long,QStringList> stackTrace;

/* Trace */

QHash<unsigned long,int> Trace::m_depth;

Trace::Trace( char* filename, int line, const QString & func_name, const QString & args ) {
	m_handle = (unsigned long)QThread::currentThreadId();
	
    LogMsg( m_depth[m_handle], filename, line, QString( "%1%2" ).arg( func_name ).arg( args ) );
	m_depth[m_handle]++;		
}

Trace::~Trace() {
	m_depth[m_handle]--;		
	stackTrace[m_handle].removeLast();
}

void Trace::LogMsg( int depth, const char * filename, int line, const QString & fonction ) {
	QString s;
	s += QDateTime::currentDateTime().toString() + " ";
	for( int i = 0; i < depth; i++ )
		s += " ";
	s += QString("> (%1)").arg( depth );
	s += QString( fonction );
	s += QString( " on %1 at line %2 (thread %3)." ).arg( filename ).arg( line ).arg( m_handle );
	
	stackTrace[m_handle].append( s );

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
/* XinxException */

XinxException::XinxException( QString message ) : m_message( message ) {
	XINX_TRACE( "XinxException", QString("( %1 )").arg( message ) );
	
	m_stack = stackTrace[ (unsigned long)QThread::currentThreadId() ];
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

