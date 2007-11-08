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

/* Log Variable */

QStringList stackTrace;

/* Trace */

int Trace::m_depth = 0;

Trace::Trace( char* filename, int line, const QString & func_name, const QString & args ) {
    LogMsg( m_depth, filename, line, QString( "%1%2" ).arg( func_name ).arg( args ) );
	m_depth++;		
}

Trace::~Trace() {
	m_depth--;
	stackTrace.removeLast();
}

void Trace::LogMsg( int depth, const char * filename, int line, const QString & fonction ) {
	QString s;
	s += QDateTime::currentDateTime().toString() + " ";
	for( int i = 0; i < depth; i++ )
		s += " ";
	s += QString("> (%1)").arg( depth );
	s += QString( fonction );
	s += QString( " on %1 at line %2." ).arg( filename ).arg( line );
	
	stackTrace.append( s );

/*
#ifndef QT_NO_DEBUG
#	ifdef Q_WS_WIN
		QFile logfile( "c:\\xinx_debug.log" );
#	else 
		QFile logfile( "/tmp/xinx_debug.log" );
#	endif
	if( logfile.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) ) {
		QTextStream logfileStream( &logfile );
		logfileStream << s << endl;
	}
#endif
*/
}
/* XinxException */

XinxException::XinxException( QString message ) {
	XINX_TRACE( "XinxException", QString("(%1)").arg( message ) );
	
	m_message = message;
	m_stack = stackTrace;
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

