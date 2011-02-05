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
#include "configuration/gceinterfacefactory.h"
#include "projectproperty/generixproject.h"
#include "docks/dictionary/dictionary_parser.h"

/* ConfigurationManager */

ConfigurationManager::ConfigurationManager()
{
	m_watcher = new QFileSystemWatcher(this);
	connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(clearCache(QString)));
}

ConfigurationManager::~ConfigurationManager()
{
	foreach(GceInterface * interface, m_interface.values())
	{
		delete interface;
	}
}

void ConfigurationManager::cleanCache()
{
	foreach(GceInterface * interface, m_interface.values())
	{
		delete interface;
	}

	foreach(const QString & key, m_interface.keys())
	{
		m_watcher->removePath(key);
	}

	m_fileToGceInterfaceKey.clear();
	m_interface.clear();
}

void ConfigurationManager::clearCache(const QString & filename)
{
	QString directory = m_fileToGceInterfaceKey.value(filename);
	if (! directory.isEmpty())
	{
		foreach(GceInterface * interface, m_interface.values(directory))
		{
			delete interface;
		}
		m_interface.remove(directory);
		foreach(const QString & key, m_fileToGceInterfaceKey.keys(directory))
		{
			m_watcher->removePath(key);
			m_fileToGceInterfaceKey.remove(key);
		}
	}
}

GceInterface * ConfigurationManager::getInterfaceOfDirectory(const QString & directory)
{
	if (m_interface.contains(directory))
	{
		return m_interface.value(directory);
	}

	/* Create the interface */
	GceInterface * interface = GceInterfaceFactory::createGceInterface(directory);
	if (interface)
	{
		m_interface.insert(directory, interface);
		foreach(const QString & file, interface->filenames())
		{
			m_fileToGceInterfaceKey.insert(file, directory);
			m_watcher->addPath(file);
		}
	}

	return interface;
}

GceInterface * ConfigurationManager::getInterfaceOfProject(XinxProject::Project * project)
{
	GenerixProject * gnxProject = static_cast<GenerixProject*>(project);
	if (gnxProject)
	{
		return getInterfaceOfDirectory(gnxProject->webModuleLocation());
	}
	else
	{
		return 0;
	}
}

void ConfigurationManager::loadDictionary(XinxProject::Project * project)
{
	GceInterface* gce = getInterfaceOfProject(project);
	if (gce)
	{
		QStringList dictionaries = getInterfaceOfProject(project)->dictionnaries();
		foreach(const QString & filename, dictionaries)
		{
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
	}
}


