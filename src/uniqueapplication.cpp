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

// Qt header
#include <QString>
#include <QMessageBox>

// Standard header
#include <iostream>
#ifndef WIN32
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#else
#include <windows.h>
#include <dbghelp.h>
#endif

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

PrivateUniqueApplication::PrivateUniqueApplication( UniqueApplication * parent ) : m_mainform(0), m_parent( parent ) {
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
			d->m_mainform->openFile( QString(m_fileView) );
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
#ifndef WIN32
	void * array[10];
	size_t size, i;
	char ** strings;
	FILE * file = NULL;
	
	size = backtrace( array, 10 );
	strings = backtrace_symbols( array, size );
	file = fopen( "/tmp/xinx_trace.log", "w+" );
	if( file ) {
		fprintf( file, "Obtained %zd stack frames.\n", size );
		for( i = 0; i < size; i++ ) {
			fprintf( file, "%s\n", strings[i] );
		}
		fclose( file );
	}
	free( strings );
	QMessageBox::critical( NULL, "XINX Crash", "XINX is unstable and is crashing. Send the file /tmp/xinx_trace.log at Ulrich Van Den Hekke" );
#else
	HANDLE hProcess = GetCurrentProcess();
	
	FILE * file = NULL;
	file = fopen( "c:\\xinx_trace.log", "w+" );
	if( file ) {
		STACKFRAME64 tempStackFrame;
		memset( &tempStackFrame, 0, sizeof(STACKFRAME64) );
		CONTEXT context;
		GetThreadContext( GetCurrentThread(), &context );
		DWORD machineType;

#ifdef _M_IX86
		machineType = IMAGE_FILE_MACHINE_I386;
		tempStackFrame.AddrPC.Offset       = context.Eip;
		tempStackFrame.AddrPC.Mode         = AddrModeFlat;
		tempStackFrame.AddrStack.Offset    = context.Esp;
		tempStackFrame.AddrStack.Mode      = AddrModeFlat;
		tempStackFrame.AddrFrame.Offset    = context.Ebp;
		tempStackFrame.AddrFrame.Mode      = AddrModeFlat;
#elif _M_X64
		machineType = IMAGE_FILE_MACHINE_AMD64;
		tempStackFrame.AddrPC.Offset = context.Rip;
		tempStackFrame.AddrPC.Mode = AddrModeFlat;
		tempStackFrame.AddrFrame.Offset = context.Rsp;
		tempStackFrame.AddrFrame.Mode = AddrModeFlat;
		tempStackFrame.AddrStack.Offset = context.Rsp;
		tempStackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
		machineType = IMAGE_FILE_MACHINE_IA64;
		tempStackFrame.AddrPC.Offset = context.StIIP;
		tempStackFrame.AddrPC.Mode = AddrModeFlat;
		tempStackFrame.AddrFrame.Offset = context.IntSp;
		tempStackFrame.AddrFrame.Mode = AddrModeFlat;
		tempStackFrame.AddrBStore.Offset = context.RsBSP;
		tempStackFrame.AddrBStore.Mode = AddrModeFlat;
		tempStackFrame.AddrStack.Offset = context.IntSp;
		tempStackFrame.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

		ULONG64 buffer[(sizeof(SYMBOL_INFO) + nbChar*sizeof(TCHAR) + sizeof(ULONG64) + 1) / sizeof(ULONG64)];
		PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
		PTSTR undecoratedName = (PTSTR)malloc(sizeof(TCHAR) * nbChar);

		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = nbChar;
		DWORD lineDisplacement;
		IMAGEHLP_LINE64 lineInfo = sizeof( IMAGEHLP_LINE64 );

		while( StackWalk64(machineType, hProcess, GetCurrentThread(), &tempStackFrame, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL) ) {
			// Sanity stack check
			if( tempStackFrame.AddrPC.Offset == 0 )
				break;

			DWORD64 symDisplacement = 0;
			// Try to get the symbol name
			if( SymFromAddr( hProcess, tempStackFrame.AddrPC.Offset, &symDisplacement, pSymbol ) ) {
				UnDecorateSymbolName(pSymbol->Name, undecoratedName, MAX_SYM_NAME, UNDNAME_COMPLETE);
				callStack.push_back(std::string(undecoratedName) + "+" + boost::lexical_cast<std::string>(symDisplacement));

				if(SymGetLineFromAddr64(hProcess, tempStackFrame.AddrPC.Offset, &lineDisplacement, &lineInfo)) {
					fprintf( file, "%s\tl:%d\n", lineInfo.FileName, lineInfo.LineNumber );
				} else {
					fprintf( file, "No info\n" );
				}
			} else {
			}
		}
		free(undecoratedName);

		fileclose( file );
	}
	QMessageBox::critical( NULL, "XINX Crash", "XINX is unstable and is crashing. Send the file c:\\xinx_trace.log at Ulrich Van Den Hekke" );
#endif
	if( d->m_mainform )
		d->m_mainform->saveProject( true );
	exit(1);
}


bool UniqueApplication::isUnique() { 
	return d->m_isUnique; 
}

void UniqueApplication::attachMainWindow( MainformImpl * mainform ) {
	d->m_mainform = mainform;
#ifdef DBUS
	new XinxAdaptor( mainform );
	QDBusConnection::sessionBus().registerObject( "/", mainform );
#endif
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
