/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "core/xinxcore.h"
#include "core/xinxconfig.h"
#include "plugins/xinxpluginsloader.h"
#include "editors/xinxformatscheme.h"
#include "editors/xinxlanguagefactory.h"
#include <plugins/xinxpluginelement.h>
#include <plugins/interfaces/files.h>

// Qt header
#include <QPluginLoader>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>

// QCodeEdit header
#include <qlanguagefactory.h>
#include <qnfadefinition.h>

/* Static function */

static bool pluginsLessThan(XinxPluginElement * s1, XinxPluginElement * s2)
{
	if (s1->isStatic() && (!s2->isStatic())) return true;
	if ((!s1->isStatic()) && s2->isStatic()) return false;
	return s1->name() < s2->name();
}

/* XinxPluginsLoader */

XinxPluginsLoader::XinxPluginsLoader()
{

}

XinxPluginsLoader::~XinxPluginsLoader()
{
	qDeleteAll(plugins());
}

QList<XinxPluginElement*> XinxPluginsLoader::plugins() const
{
	QList<XinxPluginElement*> result = m_plugins.values();
	qSort(result.begin(), result.end(), pluginsLessThan);
	return result;
}

XinxPluginElement * XinxPluginsLoader::plugin(const QString & name)
{
	return m_plugins.value(name);
}

void XinxPluginsLoader::addPlugin(QObject * plugin, bool staticLoaded)
{
	/* Manage all XINX Plugin */
	IXinxPlugin * iXinxPlugin = qobject_cast<IXinxPlugin*>(plugin);
	if (! iXinxPlugin) return;

	QString name = plugin->metaObject()->className();

	// Initialize the plugin (if needed)
	if (! iXinxPlugin->initializePlugin(XINXConfig::self()->config().language))
	{
		qCritical() << "Can't load " << name << " plugin.";
		return;
	}

	// Create a glue to use the plugin with XinxPluginsLoader
	XinxPluginElement * element = new XinxPluginElement(plugin, staticLoaded);

	// Set the status of the plugin with the configuration
	element->setActivated(XINXConfig::self()->config().plugins.value(name, true));
	m_plugins.insert(name, element);

	//! \todo move this line in setActivated method of the plugin
	// Create default tools given by the plugin
	QPair<QString,QString> tools;
	foreach(tools, iXinxPlugin->pluginTools())
	XINXConfig::self()->addDefaultTool(tools.first, tools.second);

	// Create possible extention definition
	IFilePlugin * interface = qobject_cast<IFilePlugin*>(plugin);
	if (interface)
	{
		foreach(IFileTypePlugin * t, interface->fileTypes())
		{
			// If the plugin contains format and language description, we loaded it.
			IFileTextPlugin * textPlugin = dynamic_cast<IFileTextPlugin*>(t);
			if (textPlugin)
			{
				// Format
				QFormatScheme * scheme = textPlugin->createFormatScheme(XINXConfig::self());
				if (! scheme)
				{
					scheme = XINXConfig::self()->languageFactory()->defaultFormatScheme();
				}
				else
				{
					XINXConfig::self()->addFormatScheme(textPlugin->highlighterId(), qobject_cast<XinxFormatScheme*>(scheme));
				}

				// Language
				QDomDocument doc;
				QLanguageFactory::LangData data;
				doc.setContent(textPlugin->createLanguageDescription());
				QNFADefinition::load(doc, &data, scheme);
				XINXConfig::self()->languageFactory()->addLanguage(data);
			}
		}
	}
}

void XinxPluginsLoader::loadPlugins()
{
	foreach(QObject * plugin, QPluginLoader::staticInstances())
		addPlugin(plugin, true);

	foreach(const QString & directory, QDir::searchPaths("plugins"))
	{
		QDir pluginsDir = QDir(directory);

		foreach(const QString & fileName, pluginsDir.entryList(QStringList() << "*.dll" << "*.so", QDir::Files))
		{
			QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
			QObject * plugin = loader.instance();
			if (plugin)
				addPlugin(plugin);
			else
				qDebug() << loader.errorString();
		}
	}
}

QList<IFileTypePlugin*> XinxPluginsLoader::fileTypes() const
{
	QList<IFileTypePlugin*> result;

	foreach(XinxPluginElement * element, plugins())
	{
		IFilePlugin * interface = qobject_cast<IFilePlugin*>(element->plugin());
		if (element->isActivated() && interface)
		{
			result += interface->fileTypes();
		}
	}

	return result;
}

IFileTypePlugin* XinxPluginsLoader::fileType(const QString & name) const
{
	QList<IFileTypePlugin*> list = fileTypes();
	foreach(IFileTypePlugin * type, list)
	{
		if(type->name() == name)
		{
			return type;
		}
	}
	return NULL;
}

QList<IFileTypePlugin*> XinxPluginsLoader::matchedFileType(const QString & filename) const
{
	QList<IFileTypePlugin*> types = fileTypes(), result;
	foreach(IFileTypePlugin* plugin, types)
	{
		QStringList patterns = plugin->match().split(" ");
		foreach(const QString & match, patterns)
		{
			QRegExp pattern(match, Qt::CaseInsensitive, QRegExp::Wildcard);
			if (pattern.exactMatch(filename))
				result << plugin;
		}
	}
	return result;
}

QString XinxPluginsLoader::allManagedFileFilter() const
{
	QStringList result;
	QList<IFileTypePlugin*> types = fileTypes();
	foreach(IFileTypePlugin* plugin, types)
	{
		result += plugin->match();
	}
	return tr("All managed files (%1)").arg(result.join(" "));
}

QString XinxPluginsLoader::fileTypeFilter(IFileTypePlugin * fileType)
{
	return tr("All %1 (%2)").arg(fileType->description()).arg(fileType->match());
}

QString XinxPluginsLoader::exampleOfHighlighter(const QString & name) const
{
	foreach(XinxPluginElement * element, plugins())
	{
		IFilePlugin * interface = qobject_cast<IFilePlugin*>(element->plugin());
		if (element->isActivated() && interface)
		{
			foreach(IFileTypePlugin * p, interface->fileTypes())
			{
				IFileTextPlugin * textFileType = dynamic_cast<IFileTextPlugin*>(p);
				if (textFileType && textFileType->highlighterId().toLower() == name.toLower())
				{
					return textFileType->fileExample();
				}
			}
		}
	}
	return QString();
}

XinxFormatScheme * XinxPluginsLoader::scheme(const QString & highlighter, XINXConfig * config)
{
	foreach(XinxPluginElement * element, plugins())
	{
		IFilePlugin * interface = qobject_cast<IFilePlugin*>(element->plugin());
		if (element->isActivated() && interface)
		{
			foreach(IFileTypePlugin * p, interface->fileTypes())
			{
				IFileTextPlugin * textFileType = dynamic_cast<IFileTextPlugin*>(p);
				if (textFileType && textFileType->highlighterId().toLower() == highlighter.toLower())
				{
					return textFileType->createFormatScheme(config);
				}
			}
		}
	}
	return 0;
}

QStringList XinxPluginsLoader::openDialogBoxFilters() const
{
	QStringList result;
	QList<IFileTypePlugin*> types = fileTypes();
	result += allManagedFileFilter();
	foreach(IFileTypePlugin* plugin, types)
	{
		result += fileTypeFilter(plugin);
	}
	return result;
}

QStringList XinxPluginsLoader::managedFilters() const
{
	QStringList result;
	foreach(IFileTypePlugin* plugin, fileTypes())
	{
		result += plugin->match().split(" ");
	}
	return result;
}
