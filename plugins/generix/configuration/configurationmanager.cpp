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

// Xinx header
#include "configurationmanager.h"
#include <core/xinxcore.h>
#include <core/filewatcher.h>
#include "configuration/gceconfigurationparserfactory.h"
#include "projectproperty/generixproject.h"
#include "docks/dictionary/dictionary_parser.h"
#include <jobs/xinxjobmanager.h>
#include <QTimer>

/* ConfigurationManager */

ConfigurationManager::ConfigurationManager(XinxProject::ProjectPtr project) : QObject(project.data()), _updateCacheTimer(new QTimer), _watcher(new FilesWatcher), _interface(new GceConfiguration), _project(project.toWeakRef())
{
	_updateCacheTimer->setSingleShot (true);
	_updateCacheTimer->setInterval (5000);

	connect(_watcher.data (), SIGNAL(fileChanged(QString)), _updateCacheTimer.data (), SLOT(start()));
	connect(_updateCacheTimer.data (), SIGNAL(timeout()), this, SLOT(updateCache()));

	updateCache();
}

ConfigurationManager::~ConfigurationManager()
{

}

ConfigurationManager* ConfigurationManager::manager(XinxProject::ProjectPtr project)
{
	if (! project) return NULL;
	QObject * object = project->getObject("generix");
	return qobject_cast<ConfigurationManager*>(object);
}

void ConfigurationManager::addDictionary(const QString & filename)
{
	XinxProject::ProjectPtr project = _project.toStrongRef();
	if (! project) return;

	QFile * device = new QFile(filename);
	if (device->open(QFile::ReadOnly))
	{
		ContentView3::FilePtr file = project->cache ()->cachedFile (filename);

		Generix::Dictionary::Parser * dictionaryParser = new Generix::Dictionary::Parser();
		dictionaryParser->setFile(file);
		dictionaryParser->setDevice(device);

		project->cache ()->addFileToCache (dictionaryParser, false, ContentView3::Cache::PROJECT);
	}
}

void ConfigurationManager::addConfiguration(const QString & filename)
{
	if (!_watcher->files().contains(filename))
	{
		_watcher->addPath(filename);
	}
	emit changed();
}

void ConfigurationManager::updateCache()
{
	XinxProject::ProjectPtr project = _project.toStrongRef();
	if (! project) return;

	_interface->clearAliasPolicy();
	_interface->clearBusinessView();
	_interface->clearDictionaries();
	_interface->clearFilenames();

	GceParser * parser = GceConfigurationParserFactory::createGceParser(project->projectPath());
	if (parser)
	{
		parser->setInterface(_interface.data());

		connect(parser, SIGNAL(addConfiguration(QString)), this, SLOT(addConfiguration(QString)), Qt::QueuedConnection);
		connect(parser, SIGNAL(addDictionary(QString)), this, SLOT(addDictionary(QString)), Qt::QueuedConnection);
		connect(parser, SIGNAL(jobEnding()), this, SIGNAL(changed()));

		XinxJobManager::self()->addJob(parser);
	}
}

GceConfiguration * ConfigurationManager::getInterface()
{
	return _interface.data();
}


