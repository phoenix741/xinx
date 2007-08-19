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

#ifdef Q_WS_WIN
	#ifndef QT_QTDBUS
		#define DBUS
	#endif
#else
	#define DBUS
#endif

#include "uniqueapplication.h"
#include <iostream>
#include <QString>
#include <QMessageBox>
#ifdef DBUS
	#include <unistd.h>
	#include <QtDBus>
	#include "studioadaptor.h"
	#include "studiointerface.h"
#else
	#include <windows.h>
	#include <QTimer>
	#include <QtGui>
#endif
#include "xmlvisualstudio.h"

extern XMLVisualStudio * mainWin;

#ifdef DBUS
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

/* PrivateUniqueApplication */

class PrivateUniqueApplication : public QObject {
	Q_OBJECT
public:
	PrivateUniqueApplication( UniqueApplication * parent );
	virtual ~PrivateUniqueApplication();
	
	bool m_isUnique;

#ifndef DBUS
	HWND m_handle, m_handleMutex, m_handleMutexGbl, m_handleEvent;
	char* m_fileView;
	void openSharedMem();
#else
	ComEditorXinxInterface * m_interface;
#endif
	void start();

public slots:
    void closeAllFile();
    void closeProject();
    void newFile();
    void openFile( const QString & filename );
    void openProject( const QString & filename );
    void saveAllFile();
#ifndef DBUS
	void timerApplicationEvent();
#endif
private:
	UniqueApplication * m_parent;
};

PrivateUniqueApplication::PrivateUniqueApplication( UniqueApplication * parent ) : m_parent( parent ) {
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
#ifndef DBUS
	if( m_fileView ) UnmapViewOfFile( m_fileView );
	if( m_handle ) CloseHandle( m_handle );
	if( m_handleEvent ) CloseHandle( m_handleEvent );
	if( m_handleMutex ) CloseHandle( m_handleMutex );
	if( m_handleMutexGbl ) CloseHandle( m_handleMutexGbl );
#endif	
}

void PrivateUniqueApplication::closeAllFile() {
	emit m_parent->closeAllFile();	
}

void PrivateUniqueApplication::closeProject() {
	emit m_parent->closeProject();		
}

void PrivateUniqueApplication::newFile() {
	emit m_parent->newFile();		
}

void PrivateUniqueApplication::openFile( const QString & filename ) {
	emit m_parent->openFile( filename );
}

void PrivateUniqueApplication::openProject( const QString & filename ) {
	emit m_parent->openProject( filename );
}

void PrivateUniqueApplication::saveAllFile() {
	emit m_parent->saveAllFile();		
}

#ifdef DBUS
void PrivateUniqueApplication::start() {
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
			m_interface = new ComEditorXinxInterface( appName, "/", QDBusConnection::sessionBus(), this);
		}
		
		m_isUnique = false;
		return;
	}
	
	new XinxAdaptor( this );
	QDBusConnection::sessionBus().registerObject( "/", this );

	m_interface = new ComEditorXinxInterface( appName, "/", QDBusConnection::sessionBus(), this);

	m_isUnique = true;
} 
#else

void PrivateUniqueApplication::start() {
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
			openFile( QString(m_fileView) );
			m_fileView[0] = '\0';
		}
		ReleaseMutex( m_handleMutex );
	}
}

#endif


/* UniqueApplication */

UniqueApplication::UniqueApplication( int & argc, char ** argv ) : QApplication( argc, argv ) {
	d = new PrivateUniqueApplication( this );
	d->start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, bool GUIenabled ) : QApplication( argc, argv, GUIenabled ) {
	d = new PrivateUniqueApplication( this );
	d->start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, Type type ) : QApplication( argc, argv, type ) {
	d = new PrivateUniqueApplication( this );
	d->start();
}

UniqueApplication::~UniqueApplication() {
	delete d;
}

bool UniqueApplication::notify ( QObject * receiver, QEvent * event ) {
	try {
		return QApplication::notify( receiver, event );
	} catch( ... ) {
		notifyError();
		return false;
	}
}

void UniqueApplication::notifyError() {
	QMessageBox::critical( NULL, "Error", "Shit ! How can it be happen ? What's the hell Ulrich !\nOk. I try to repair that, and you, send me a detailled report (Where ? When ? Who ? How ? Why ?)." );
	if( mainWin )
		mainWin->closeProject( false, true );
	exit(1);
}


bool UniqueApplication::isUnique() { 
	return d->m_isUnique; 
}

void UniqueApplication::callOpenFile(const QString &fileName) {
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

#include "uniqueapplication.moc"
