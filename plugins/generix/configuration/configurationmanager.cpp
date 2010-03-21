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
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2manager.h>

/* Global */

ConfigurationManager * ConfigurationManager::s_self = 0;

/* ConfigurationManager */

ConfigurationManager::ConfigurationManager()
{
	m_watcher = new QFileSystemWatcher(this);
	connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(clearCache(QString)));
}

ConfigurationManager::~ConfigurationManager()
{
	if (s_self == this)
	{
		s_self = 0;
	}
	foreach(GceInterface * interface, m_interface.values())
	{
		delete interface;
	}
}

ConfigurationManager * ConfigurationManager::self()
{
	if (! s_self)
	{
		s_self = new ConfigurationManager();
		XINXStaticDeleter::self()->add(s_self);
	}
	return s_self;
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

GceInterface * ConfigurationManager::getInterfaceOfProject(XinxProject * project)
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

DictionaryParser * ConfigurationManager::loadDictionary(XinxProject * project)
{
	QStringList dictionaries = getInterfaceOfProject(project)->dictionnaries();
	foreach(const QString & filename, dictionaries)
	{
		ContentView2::Manager::self()->cache()->addToCache(project, filename, "GNX_DICO", "*");
	}
}


