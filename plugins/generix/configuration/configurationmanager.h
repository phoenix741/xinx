/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

// Qt header
#include <QObject>
#include <QHash>
#include <QFileSystemWatcher>

// Xinx header
#include "configuration/gceinterface.h"
#include <project/xinxproject.h>

class ConfigurationManager : public QObject {
	Q_OBJECT
public:
	virtual ~ConfigurationManager();

	GceInterface * getInterfaceOfProject( XinxProject * project );
	GceInterface * getInterfaceOfDirectory( const QString & directory );

	void cleanCache();

	static ConfigurationManager * self();
private slots:
	void clearCache( const QString & filename );
private:

	ConfigurationManager();

	QFileSystemWatcher * m_watcher;
	QHash<QString,QString> m_fileToDirectory;
	QHash<QString,GceInterface*> m_interface;

	static ConfigurationManager * s_self;
};

#endif // CONFIGURATIONMANAGER_H
