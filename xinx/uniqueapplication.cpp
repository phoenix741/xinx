/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
#include "uniqueapplication.h"
#include "private/p_uniqueapplication.h"
#include "mainformimpl.h"
#include "exceptions.h"
#include "xslproject.h"

// Qt header
#include <QString>
#include <QMessageBox>
#include <QThread>
#include <QErrorMessage>

// Standard header
#include <iostream>

#ifdef DBUS
static QDBusConnectionInterface *tryToInitDBusConnection() {
	XINX_TRACE( "tryToInitDBusConnection", "()" );
	
	// Check the D-Bus connection health
	QDBusConnectionInterface* dbusService = 0;
	if (!QDBusConnection::sessionBus().isConnected() || !(dbusService = QDBusConnection::sessionBus().interface())) {
		std::cout << QObject::tr("UniqueApplication: Cannot find the D-Bus session server").toStdString() << std::endl;
		return NULL;
	}
	return dbusService;
}
#endif

/* PrivateUniqueApplication */

PrivateUniqueApplication::PrivateUniqueApplication( UniqueApplication * parent ) : m_mainform(0), m_parent( parent ) {
	XINX_TRACE( "PrivateUniqueApplication", QString( "( 0x%1 )" ).arg( (unsigned int)parent, 0, 16 ) );

#ifndef DBUS
	m_handle = 0;
	m_handleMutex = 0;
	m_handleMutexGbl = 0;
	m_handleEvent = 0;
	m_fileView = 0;
#else
	m_interface = NULL;
#endif
}

PrivateUniqueApplication::~PrivateUniqueApplication() {
	XINX_TRACE( "~PrivateUniqueApplication", "()" );

#ifndef DBUS
	if( m_fileView ) UnmapViewOfFile( m_fileView );
	if( m_handle ) CloseHandle( m_handle );
	if( m_handleEvent ) CloseHandle( m_handleEvent );
	if( m_handleMutex ) CloseHandle( m_handleMutex );
	if( m_handleMutexGbl ) CloseHandle( m_handleMutexGbl );
#endif	
}

#ifdef DBUS
void PrivateUniqueApplication::start() {
	XINX_TRACE( "PrivateUniqueApplication::start", "()" );

	QString appName = "com.editor.xinx";
	QDBusConnectionInterface* dbusService = tryToInitDBusConnection();

	if ( dbusService && (dbusService->registerService(appName) != QDBusConnectionInterface::ServiceRegistered) ) {
		std::cout << QObject::tr("UniqueApplication: Can't setup D-Bus service. Probably already running.").toStdString() << std::endl;
		
		QString pid = QString::number(getpid());
		if( dbusService->registerService(appName + '-' + pid) != QDBusConnectionInterface::ServiceRegistered ) {
			std::cout << QObject::tr("UniqueApplication: Can't really create D-Bus service.").toStdString();
		} else {
//			new XinxAdaptor( this );
//			QDBusConnection::sessionBus().registerObject( "/", this );
			m_interface = new OrgShadowareXinxInterface( appName, "/", QDBusConnection::sessionBus(), this);
		}
		
		m_isUnique = false;
		return;
	}
	
	m_interface = new OrgShadowareXinxInterface( appName, "/", QDBusConnection::sessionBus(), this);

	m_isUnique = true;
} 
#else

void PrivateUniqueApplication::start() {
	XINX_TRACE( "PrivateUniqueApplication::start", "()" );

	SECURITY_DESCRIPTOR securityDesc;
	SECURITY_ATTRIBUTES securityAttr;

	InitializeSecurityDescriptor(&securityDesc, SECURITY_DESCRIPTOR_REVISION);
  	SetSecurityDescriptorDacl(&securityDesc, true, NULL, false);
  	
	securityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttr.lpSecurityDescriptor = &securityDesc;
	securityAttr.bInheritHandle = false;

	m_handleMutex = (HWND)CreateMutex( &securityAttr, false, (WCHAR*)"com.editor.xinx.mutex" );
	int error = GetLastError();
	if( ! m_handleMutex ) perror( "CreateMutex" );
	m_handleMutexGbl = (HWND)CreateMutex( &securityAttr, false, (WCHAR*)"Global\\com.editor.xinx.mutex" );
	if( ! m_handleMutexGbl ) perror( "CreateMutex" );

	if( m_handleMutex == 0 ) {
		std::cout << QCoreApplication::translate("UniqueApplication", "UniqueApplication: Can't create mutex.").toStdString ();
		m_isUnique = false;
		return;
	}
	
	openSharedMem();

	if( error == ERROR_ALREADY_EXISTS ) {
		m_isUnique = false;
		return;
	}

	if( m_handleMutex && m_handle && m_fileView ) {
		QTimer* timer = new QTimer( this );
		connect( timer, SIGNAL(timeout()), this, SLOT(timerApplicationEvent()) );
		timer->start( 500 );
	}

	m_isUnique = true;
}

void PrivateUniqueApplication::openSharedMem() {
	XINX_TRACE( "PrivateUniqueApplication::openSharedMem", "()" );

	#define SIZE 4096

	// Mapping

	m_handle = (HWND)CreateFileMapping( (void*)(0xFFFFFFFF), NULL, PAGE_READWRITE, 0, SIZE, (WCHAR*)"com.editor.xinx.mapping" );
	if( m_handle == 0 ) {
		std::cout << QCoreApplication::translate("UniqueApplication", "UniqueApplication: Error creating shared memory.").toStdString();
		return;
	} 

	m_fileView = (char*)MapViewOfFile( m_handle, FILE_MAP_WRITE, 0, 0, SIZE );
	if( m_fileView == 0 ) {
		std::cout << QCoreApplication::translate("UniqueApplication", "UniqueApplication: Error can't map file").toStdString();
		CloseHandle( m_handle );
		return;
	}
	m_fileView[0] = '\0';
	
	// Event
	m_handleEvent = (HWND)CreateEvent( NULL, false, false, (WCHAR*)"com.editor.xinx.event" );
	
}

void PrivateUniqueApplication::timerApplicationEvent() {
	if( WaitForSingleObject( m_handleMutex, INFINITE ) == WAIT_OBJECT_0 ) {
		if( strlen( m_fileView ) > 0 ) {
			d->m_mainform->openFile( QString(m_fileView) );
			m_fileView[0] = '\0';
		}
		ReleaseMutex( m_handleMutex );
	}
}

#endif


/* UniqueApplication */

UniqueApplication::UniqueApplication( int & argc, char ** argv ) : QApplication( argc, argv ) {
	XINX_TRACE( "UniqueApplication", QString( "( %1, argv )" ).arg( argc ) );

	d = new PrivateUniqueApplication( this );
	d->start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, bool GUIenabled ) : QApplication( argc, argv, GUIenabled ) {
	XINX_TRACE( "UniqueApplication", QString( "( %1, argv, %2 )" ).arg( argc ).arg( GUIenabled ) );

	d = new PrivateUniqueApplication( this );
	d->start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, Type type ) : QApplication( argc, argv, type ) {
	XINX_TRACE( "UniqueApplication", QString( "( %1, argv, type )" ).arg( argc ) );

	d = new PrivateUniqueApplication( this );
	d->start();
}

UniqueApplication::~UniqueApplication() {
	XINX_TRACE( "~UniqueApplication", "()" );

	delete d;
}

bool UniqueApplication::notify ( QObject * receiver, QEvent * event ) {
	// Too verbose
	//XINX_TRACE( "UniqueApplication::notify", QString( "( 0x%1, 0x%2 )" ).arg( (unsigned int)receiver, 0, 16 ).arg( (unsigned int)event, 0, 16 ) );

	try {
		return QApplication::notify( receiver, event );
	} catch( XinxException e ) {
		notifyError( e.getMessage(), e.getStack() );
		return false;
	} catch( ... ) {
		notifyError( "Generic Exception", stackTrace[(unsigned long)QThread::currentThreadId()] );
		return false;
	}
}

// Include for exception
#ifndef Q_WS_WIN
#	include <execinfo.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <cxxabi.h>
#endif

// Constant for exception
#ifdef Q_WS_WIN
	const size_t nbChar = 100;
#endif

void UniqueApplication::notifyError( QString error, QStringList stack ) {
	d->m_mainform->hide();
	
	FILE * file = NULL;
#ifndef Q_WS_WIN
	char * filename = "/tmp/xinx_trace.log";
#else
	char * filename = "c:\\xinx_trace.log";
#endif
	
	file = fopen( filename, "w+" );
	if( file ) {
#ifndef Q_WS_WIN
		void * array[10];
		size_t size, i;
		char ** strings;
	
		size = backtrace( array, 10 );
		strings = backtrace_symbols( array, size );
		fprintf( file, "* GNU/Linux backtrace ...\n" );
		fprintf( file, "Obtained %zd stack frames.\n", size );
		for( i = 0; i < size; i++ ) {
			fprintf( file, "%s\n", strings[i] );
		}
		free( strings );
#endif
		fprintf( file, "* XINX backtrace ...\n" );
		fprintf( file, stack.join( "\n" ).toAscii() );
		
		fprintf( file, error.toAscii() );
		fclose( file );
	}
	QErrorMessage errorMessage;
	errorMessage.showMessage( QString( "<b>Error %2</b><br/>"
									   "Please send %1 at XINX project leader, i attempt to save the project (if possible)" ).arg( filename ).arg( error ) );
	
	if( d->m_mainform && XINXProjectManager::self()->project() ) {
		d->m_mainform->saveProject( true );
		QMessageBox::information( NULL, "XINX Crash", "Project is saved !" );
	}
	exit(1);
}


bool UniqueApplication::isUnique() { 
	XINX_TRACE( "UniqueApplication::isUnique", "()" );

	return d->m_isUnique; 
}

void UniqueApplication::attachMainWindow( MainformImpl * mainform ) {
	XINX_TRACE( "UniqueApplication::attachMainWindow", QString( "( 0x%1 )" ).arg( (unsigned int)mainform, 0, 16 ) );

	d->m_mainform = mainform;
#ifdef DBUS
	new XinxAdaptor( mainform );
	QDBusConnection::sessionBus().registerObject( "/", mainform );
#endif
}

void UniqueApplication::callOpenFile(const QString &fileName) {
	XINX_TRACE( "UniqueApplication::callOpenFile", QString( "( %1 )" ).arg( fileName ) );

#ifndef DBUS
	while( strlen( d->m_fileView ) > 0 ) Sleep( 250 );
	if( WaitForSingleObject( m_handleMutex, INFINITE ) == WAIT_OBJECT_0 ) {
		strncpy( m_fileView, const_cast<char*>(fileName.toAscii().data()), 4096 );
		
		ReleaseMutex( m_handleMutex );
	}
#else
	d->m_interface->openFile( fileName );
#endif
}
