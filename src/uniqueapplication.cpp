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

#include <QString>
#include <QtDBus>
 
#include "uniqueapplication.h"
#include "studioadaptor.h"

static QDBusConnectionInterface *tryToInitDBusConnection() {
	// Check the D-Bus connection health
	QDBusConnectionInterface* dbusService = 0;
	if (!QDBusConnection::sessionBus().isConnected() || !(dbusService = QDBusConnection::sessionBus().interface())) {
		std::cout << "UniqueApplication: Cannot find the D-Bus session server" << std::endl;
		return NULL;
	}
	return dbusService;
}

UniqueApplication::UniqueApplication( int & argc, char ** argv ) : QApplication( argc, argv ) {
	start();
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, bool GUIenabled ) : QApplication( argc, argv, GUIenabled ) {
	start();	
}

UniqueApplication::UniqueApplication( int & argc, char ** argv, Type type ) : QApplication( argc, argv, type ) {
	start();	
}

UniqueApplication::UniqueApplication( Display * display, Qt::HANDLE visual, Qt::HANDLE colormap ) : QApplication( display, visual, colormap ) {
	start();	
}

UniqueApplication::UniqueApplication( Display * display, int & argc, char ** argv, Qt::HANDLE visual, Qt::HANDLE colormap ) : QApplication( display, argc, argv, visual, colormap ) {
	start();	
}
	
UniqueApplication::~UniqueApplication() {
	
}

void UniqueApplication::start() {
	QString appName = "com.generix.xmlstudio";
	QDBusConnectionInterface* dbusService = tryToInitDBusConnection();
	
	if ( (dbusService) && (dbusService->registerService(appName) != QDBusConnectionInterface::ServiceRegistered) ) {
		std::cout << "UniqueApplication: Can't setup D-Bus service. Probably already running." << std::endl;
		
	
		QString pid = QString::number(getpid());
		if( dbusService->registerService(appName + '-' + pid) != QDBusConnectionInterface::ServiceRegistered ) {
			std::cout << "UniqueApplication: Can't really create D-Bus service.";
		} else {
			new XmlstudioAdaptor( this );
			QDBusConnection::sessionBus().registerObject( "/", this );
		}
		
		m_isUnique = false;
		return;
	}
	m_isUnique = true;
	
	new XmlstudioAdaptor( this );
	QDBusConnection::sessionBus().registerObject( "/", this );
}

void UniqueApplication::sendSignalOpen(const QString &fileName) {
	emit open(fileName);
}
