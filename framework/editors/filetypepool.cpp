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

#include "filetypepool.h"
#include <core/xinxconfig.h>
#include <plugins/xinxpluginelement.h>
#include <plugins/xinxpluginsloader.h>
#include <plugins/interfaces/files.h>
#include <QDomDocument>
#include <qnfadefinition.h>
#include <editors/xinxlanguagefactory.h>
#include <editors/xinxformatscheme.h>

/* PrivateFileTypePool */

class PrivateFileTypePool
{
public:
	QMultiHash<QString,IFileTypePluginPtr> _filetypes;
	QMultiHash<QString,QString> _highlighter;
	QHash<QString,XinxFormatSchemePtr> _formatschemes;
};

/* FileTypePool */

FileTypePool::FileTypePool() : d(new PrivateFileTypePool)
{
	updateFileTypes();

	connect(XinxPluginsLoader::self(), SIGNAL(pluginActivated(QString)), this, SLOT(activatePlugin(QString)));
	connect(XinxPluginsLoader::self(), SIGNAL(pluginDesactivated(QString)), this, SLOT(desactivatePlugin(QString)));
}

FileTypePool::~FileTypePool()
{

}

void FileTypePool::updateFileTypes()
{
	d->_filetypes.clear();

	foreach(XinxPluginElement * element, XinxPluginsLoader::self()->plugins())
	{
		activatePlugin(element->plugin()->metaObject()->className());
	}
}

QList<IFileTypePluginPtr> FileTypePool::fileTypes() const
{
	return d->_filetypes.values();
}

IFileTypePluginPtr FileTypePool::fileType(const QString& name) const
{
	QList<IFileTypePluginPtr> list = fileTypes();
	foreach(IFileTypePluginPtr type, list)
	{
		if (type->name() == name)
		{
			return type;
		}
	}
	return IFileTypePluginPtr();
}

QList<IFileTypePluginPtr> FileTypePool::matchedFileType(const QString& filename) const
{
	QList<IFileTypePluginPtr> types = fileTypes(), result;
	foreach(IFileTypePluginPtr plugin, types)
	{
		QStringList patterns = plugin->match().split(" ");
		foreach(const QString & match, patterns)
		{
			QRegExp pattern(match, Qt::CaseInsensitive, QRegExp::Wildcard);
			if (pattern.exactMatch(filename))
			{
				result << plugin;
			}
		}
	}
	return result;
}


QString FileTypePool::allManagedFileFilter() const
{
	QStringList result;
	QList<IFileTypePluginPtr> types = fileTypes();
	foreach(IFileTypePluginPtr plugin, types)
	{
		result += plugin->match();
	}
	return tr("All managed files (%1)").arg(result.join(" "));
}

QString FileTypePool::fileTypeFilter(IFileTypePluginPtr fileType)
{
	return tr("All %1 (%2)").arg(fileType->description()).arg(fileType->match());
}

QStringList FileTypePool::openDialogBoxFilters() const
{
	QStringList result;
	QList<IFileTypePluginPtr> types = fileTypes();
	result += allManagedFileFilter();
	foreach(IFileTypePluginPtr plugin, types)
	{
		result += fileTypeFilter(plugin);
	}
	return result;
}

QStringList FileTypePool::managedFilters() const
{
	QStringList result;
	foreach(IFileTypePluginPtr plugin, fileTypes())
	{
		result += plugin->match().split(" ");
	}
	return result;
}

QString FileTypePool::exampleOfHighlighter(const QString & name) const
{
	foreach(IFileTypePluginPtr p, fileTypes())
	{
		IFileTextPluginPtr textFileType = p.dynamicCast<IFileTextPlugin>();
		if (textFileType && textFileType->highlighterId().toLower() == name.toLower())
		{
			return textFileType->fileExample();
		}
	}

	return QString();
}

XinxFormatSchemePtr FileTypePool::scheme(const QString & highlighter) const
{
	return d->_formatschemes.value(highlighter);
}

void FileTypePool::activatePlugin(const QString& name)
{
	XinxPluginElement* element = XinxPluginsLoader::self()->plugin(name);

	IFilePlugin * interface = qobject_cast<IFilePlugin*>(element->plugin());
	if (element->isActivated() && interface)
	{
		foreach(IFileTypePlugin* p, interface->createFileTypes())
		{
			IFileTypePluginPtr ptr(p);
			d->_filetypes.insert(element->name(), ptr);

			// If the plugin contains format and language description, we loaded it.
			IFileTextPluginPtr textPlugin = ptr.dynamicCast<IFileTextPlugin>();
			if (textPlugin)
			{
				// Format
				XinxFormatSchemePtr scheme(textPlugin->createFormatScheme(XINXConfig::self()));
				if (scheme)
				{
					XINXConfig::self()->addFormatScheme(textPlugin->highlighterId(), scheme);

					d->_highlighter.insert(textPlugin->name(), textPlugin->highlighterId());
					d->_formatschemes.insert(textPlugin->highlighterId(), scheme);

					// Language
					QDomDocument doc;
					QLanguageFactory::LangData data;
					doc.setContent(textPlugin->createLanguageDescription());
					// Can't unload is it a problem ?
					QNFADefinition::load(doc, &data, scheme.data());
					XINXConfig::self()->languageFactory()->addLanguage(data);
				}
			}
		}
	}
}

void FileTypePool::desactivatePlugin(const QString& name)
{
	d->_filetypes.remove(name);

	foreach(const QString & highlighterId, d->_highlighter.values(name))
	{
		d->_formatschemes.remove(highlighterId);
	}

	d->_highlighter.remove(name);
}

