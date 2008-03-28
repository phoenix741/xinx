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
#include <QDebug>

static QDBusConnectionInterface *tryToInitDBusConnection() {
	XINX_TRACE( "tryToInitDBusConnection", "()" );
	
	// Check the D-Bus connection health
	QDBusConnectionInterface* dbusService = 0;
	if (!QDBusConnection::sessionBus().isConnected() || !(dbusService = QDBusConnection::sessionBus().interface())) {
		qWarning() << QObject::tr("UniqueApplication: Cannot find the D-Bus session server");;
		return NULL;
	}
	return dbusService;
}

/* PrivateUniqueApplication */

PrivateUniqueApplication::PrivateUniqueApplication( UniqueApplication * parent ) : m_mainform(0), m_parent( parent ) {
	XINX_TRACE( "PrivateUniqueApplication", QString( "( 0x%1 )" ).arg( (unsigned int)parent, 0, 16 ) );

	m_interface = NULL;
}

void PrivateUniqueApplication::start() {
	XINX_TRACE( "PrivateUniqueApplication::start", "()" );

	QString appName = "com.editor.xinx";
	QDBusConnectionInterface* dbusService = tryToInitDBusConnection();

	if ( dbusService && (dbusService->registerService(appName) != QDBusConnectionInterface::ServiceRegistered) ) {
		qWarning() << QObject::tr("UniqueApplication: Can't setup D-Bus service. Probably already running.");
		
		QString pid = QString::number(getpid());
		if( dbusService->registerService(appName + '-' + pid) != QDBusConnectionInterface::ServiceRegistered ) {
			qWarning() << QObject::tr("UniqueApplication: Can't really create D-Bus service.");
		} else {
			m_interface = new OrgShadowareXinxInterface( appName, "/", QDBusConnection::sessionBus(), this);
		}
		
		m_isUnique = false;
		return;
	}
	
	m_interface = new OrgShadowareXinxInterface( appName, "/", QDBusConnection::sessionBus(), this);

	m_isUnique = true;
} 

void PrivateUniqueApplication::slotErrorTriggered() {
	if( m_mainform ) {
		// Hide the main widget
		m_mainform->hide();
		
		if( XINXProjectManager::self()->project() )
			m_mainform->saveProject( true );
	}
}

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
		qFatal( qPrintable( e.getMessage() ) );
		return true;
	} catch( ... ) {
		qFatal( "Generic Exception" );
		return true;
	}
}

bool UniqueApplication::isUnique() { 
	XINX_TRACE( "UniqueApplication::isUnique", "()" );

	return d->m_isUnique; 
}

void UniqueApplication::attachMainWindow( MainformImpl * mainform ) {
	XINX_TRACE( "UniqueApplication::attachMainWindow", QString( "( 0x%1 )" ).arg( (unsigned int)mainform, 0, 16 ) );

	d->m_mainform = mainform;
	connect( ExceptionManager::self(), SIGNAL(errorTriggered()), d, SLOT(slotErrorTriggered()) );

	new XinxAdaptor( mainform );
	QDBusConnection::sessionBus().registerObject( "/", mainform );
}

void UniqueApplication::callOpenFile(const QString &fileName) {
	XINX_TRACE( "UniqueApplication::callOpenFile", QString( "( %1 )" ).arg( fileName ) );

	d->m_interface->openFile( fileName );
}
