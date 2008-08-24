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
#include "mainformimpl.h"
#include "exceptions.h"
#include "xslproject.h"

#include "mainformimpl.h"
#include "dbus/orgshadowarexinxinterface.h"

// Qt header
#include <QString>
#include <QMessageBox>
#include <QThread>
#include <QDebug>

// Qt header
#include <QObject>
#include <QtDBus>

// Standard header
#include <unistd.h>

static QDBusConnectionInterface *tryToInitDBusConnection() {
	// Check the D-Bus connection health
	QDBusConnectionInterface* dbusService = 0;
	if (!QDBusConnection::sessionBus().isConnected() || !(dbusService = QDBusConnection::sessionBus().interface())) {
		qWarning() << QObject::tr("UniqueApplication: Cannot find the D-Bus session server (%1)").arg( qt_error_string() );;
		return NULL;
	}
	return dbusService;
}

/* UniqueApplication */

UniqueApplication::UniqueApplication( int & argc, char ** argv ) : QApplication( argc, argv ), m_mainform(0), m_interface(0) {
	start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, bool GUIenabled ) : QApplication( argc, argv, GUIenabled ), m_mainform(0), m_interface(0) {
	start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, Type type ) : QApplication( argc, argv, type ), m_mainform(0), m_interface(0) {
	start();
}

UniqueApplication::~UniqueApplication() {

}

bool UniqueApplication::notify ( QObject * receiver, QEvent * event ) {
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
	return m_isUnique;
}

void UniqueApplication::attachMainWindow( MainformImpl * mainform ) {
	m_mainform = mainform;
	connect( ExceptionManager::self(), SIGNAL(errorTriggered()), this, SLOT(slotErrorTriggered()) );
}

void UniqueApplication::callOpenFile(const QString &fileName) {
	m_interface->openFile( fileName );
}


void UniqueApplication::start() {
	QString appName = "org.shadoware.xinx";
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

void UniqueApplication::slotErrorTriggered() {
	if( m_mainform ) {
		// Hide the main widget
		m_mainform->hide();

		if( XINXProjectManager::self()->project() )
			m_mainform->saveProject( true );
	}
}
