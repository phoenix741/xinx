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
#include <QScopedPointer>

// Xinx header
#include "configuration/gceconfiguration.h"
#include <project/xinxprojectproject.h>
#include <core/filewatcher.h>

class ConfigurationManager : public QObject
{
	Q_OBJECT
public:
	ConfigurationManager(XinxProject::ProjectPtr project);
	virtual ~ConfigurationManager();

	static ConfigurationManager * manager(XinxProject::ProjectPtr project);

	QSharedPointer<GceConfiguration> getInterface();

	void refresh();
	bool isReady() const;
signals:
	void configurationChanged();
	void dictionaryChanged();
private slots:
	void dictionaryUpdated(ContentView3::FilePtr file);
	void cacheUpdated();
	void updateCache();
	void addDictionary(const QString & filename);
	void addConfiguration(const QString & filename);
private:
	QScopedPointer<QTimer> _updateCacheTimer;
	QScopedPointer<FilesWatcher> _watcher;
	QSharedPointer<GceConfiguration> _interface;
	XinxProject::ProjectPtrWeak _project;
	bool _is_ready;
};

#endif // CONFIGURATIONMANAGER_H
