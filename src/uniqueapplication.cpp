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

#include <iostream>
#include <unistd.h>

#include <QString>

#ifndef Q_WS_WIN
	#include <QtDBus>
	#include "studioadaptor.h"
	#include "studiointerface.h"
#else
	#include <QTimer>
	#include <QtGui>
#endif
 
#include "uniqueapplication.h"

#ifndef Q_WS_WIN
static QDBusConnectionInterface *tryToInitDBusConnection() {
	// Check the D-Bus connection health
	QDBusConnectionInterface* dbusService = 0;
	if (!QDBusConnection::sessionBus().isConnected() || !(dbusService = QDBusConnection::sessionBus().interface())) {
		std::cout << QObject::tr("UniqueApplication: Cannot find the D-Bus session server").toStdString() << std::endl;
		return NULL;
	}
	return dbusService;
}
#endif

UniqueApplication::UniqueApplication( int & argc, char ** argv ) : QApplication( argc, argv ) {
#ifdef Q_WS_WIN
	m_handle = 0;
	m_handleMutex = 0;
	m_handleMutexGbl = 0;
	m_handleEvent = 0;
	m_fileView = 0;
#endif
	start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, bool GUIenabled ) : QApplication( argc, argv, GUIenabled ) {
#ifdef Q_WS_WIN
	m_handle = 0;
	m_handleMutex = 0;
	m_handleMutexGbl = 0;
	m_handleEvent = 0;
	m_fileView = 0;
#endif
	start();	
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, Type type ) : QApplication( argc, argv, type ) {
#ifdef Q_WS_WIN
	m_handle = 0;
	m_handleMutex = 0;
	m_handleMutexGbl = 0;
	m_handleEvent = 0;
	m_fileView = 0;
#endif
	start();	
}

#ifndef Q_WS_WIN
UniqueApplication::UniqueApplication( Display * display, Qt::HANDLE visual, Qt::HANDLE colormap ) : QApplication( display, visual, colormap ) {
	start();	
}

UniqueApplication::UniqueApplication( Display * display, int & argc, char ** argv, Qt::HANDLE visual, Qt::HANDLE colormap ) : QApplication( display, argc, argv, visual, colormap ) {
	start();	
}
#endif	

UniqueApplication::~UniqueApplication() {
#ifdef Q_WS_WIN
	if( m_fileView ) UnmapViewOfFile( m_fileView );

	if( m_handle ) CloseHandle( m_handle );
	if( m_handleEvent ) CloseHandle( m_handleEvent );
	if( m_handleMutex ) CloseHandle( m_handleMutex );
	if( m_handleMutexGbl ) CloseHandle( m_handleMutexGbl );
#endif
}

void UniqueApplication::start() {
#ifndef Q_WS_WIN
	QString appName = "com.generix.xmlstudio";
	QDBusConnectionInterface* dbusService = tryToInitDBusConnection();

	if ( (dbusService) && (dbusService->registerService(appName) != QDBusConnectionInterface::ServiceRegistered) ) {
		std::cout << QObject::tr("UniqueApplication: Can't setup D-Bus service. Probably already running.").toStdString() << std::endl;
		
		QString pid = QString::number(getpid());
		if( dbusService->registerService(appName + '-' + pid) != QDBusConnectionInterface::ServiceRegistered ) {
			std::cout << QObject::tr("UniqueApplication: Can't really create D-Bus service.").toStdString();
		} else {
			new XmlstudioAdaptor( this );
			QDBusConnection::sessionBus().registerObject( "/", this );
		}
		
		m_isUnique = false;
		return;
	}
	
	new XmlstudioAdaptor( this );
	QDBusConnection::sessionBus().registerObject( "/", this );

	com::generix::xmlstudio * iface = new com::generix::xmlstudio(QString(), QString(), QDBusConnection::sessionBus(), this);;
	connect( iface, SIGNAL(open(QString)), this, SIGNAL(hasFileToOpen(QString)) );

#else
	SECURITY_DESCRIPTOR securityDesc;
	SECURITY_ATTRIBUTES securityAttr;

	InitializeSecurityDescriptor(&securityDesc, SECURITY_DESCRIPTOR_REVISION);
  	SetSecurityDescriptorDacl(&securityDesc, true, NULL, false);
  	
	securityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttr.lpSecurityDescriptor = &securityDesc;
	securityAttr.bInheritHandle = false;

	m_handleMutex = (HWND)CreateMutex( &securityAttr, false, (WCHAR*)"com.generix.xmlstudio.mutex" );
	int error = GetLastError();
	m_handleMutexGbl = (HWND)CreateMutex( &securityAttr, false, (WCHAR*)"Global\\com.generix.xmlstudio.mutex" );

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
#endif

	m_isUnique = true;
}

#ifdef Q_WS_WIN

void UniqueApplication::openSharedMem() {
	#define SIZE 4096

	// Mapping

	m_handle = (HWND)CreateFileMapping( (void*)(0xFFFFFFFF), NULL, PAGE_READWRITE, 0, SIZE, (WCHAR*)"com.generix.xmlstudio.mapping" );
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
	m_handleEvent = (HWND)CreateEvent( NULL, false, false, (WCHAR*)"com.generix.xmlstudio.event" );
	
}

#endif

void UniqueApplication::sendSignalOpen(const QString &fileName) {
#ifdef Q_WS_WIN
	while( strlen(m_fileView) > 0 ) Sleep( 250 );
	if( WaitForSingleObject( m_handleMutex, INFINITE ) == WAIT_OBJECT_0 ) {
		strncpy( m_fileView, const_cast<char*>(fileName.toAscii().data()), 4096 );
		
		ReleaseMutex( m_handleMutex );
	}
#else
	emit open(fileName);
#endif
}

#ifdef Q_WS_WIN

void UniqueApplication::timerApplicationEvent() {
	if( WaitForSingleObject( m_handleMutex, INFINITE ) == WAIT_OBJECT_0 ) {
		if( strlen( m_fileView ) > 0 ) {
			emit hasFileToOpen( QString(m_fileView) );
			m_fileView[0] = '\0';
		}
		ReleaseMutex( m_handleMutex );
	}
}

#endif
