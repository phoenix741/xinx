/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
#include "externalfileresolver.h"
#include "core/xinxcore.h"
#include "plugins/xinxpluginsloader.h"

// Qt header
#include <QDir>
#include <QDebug>

/* Static member */

ExternalFileResolver * ExternalFileResolver::s_self = 0;

/* ExternalFileResolver */

ExternalFileResolver::ExternalFileResolver()
{
}

ExternalFileResolver::~ExternalFileResolver()
{
	if (s_self == this)
	{
		s_self = 0;
	}
}

QStringList ExternalFileResolver::externalFileResoverNames() const
{
	QStringList result;

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		IResolverPlugin * resolverPlugin = qobject_cast<IResolverPlugin*>(plugin->plugin());
		if (! resolverPlugin)  continue;

		foreach(IFileResolverPlugin * resolver, resolverPlugin->fileResolvers())
		{
			result << resolver->name();
		}
	}
	return result;
}

QStringList ExternalFileResolver::externalFileResoverIds() const
{
	QStringList result;

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		IResolverPlugin * resolverPlugin = qobject_cast<IResolverPlugin*>(plugin->plugin());
		if (! resolverPlugin)  continue;

		foreach(IFileResolverPlugin * resolver, resolverPlugin->fileResolvers())
		{
			result << resolver->id();
		}
	}
	return result;
}

IFileResolverPlugin * ExternalFileResolver::externalFileResover(const QString & id) const
{
	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		IResolverPlugin * resolverPlugin = qobject_cast<IResolverPlugin*>(plugin->plugin());
		if (! resolverPlugin)  continue;

		foreach(IFileResolverPlugin * resolver, resolverPlugin->fileResolvers())
		{
			if (resolver->id() != id) continue;

			return resolver;
		}
	}
	return 0;
}

QString ExternalFileResolver::resolveFileName(const QString & nameToResolve, const QString & currentPath)
{
	QString resolvedName = m_externalFileResolverCache.value(qMakePair(nameToResolve,currentPath));
	if (! resolvedName.isEmpty() && QFile::exists(resolvedName))
		return resolvedName;

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		IResolverPlugin * resolverPlugin = qobject_cast<IResolverPlugin*>(plugin->plugin());
		if (! resolverPlugin)  continue;

		foreach(IFileResolverPlugin * resolver, resolverPlugin->fileResolvers())
		{
			if (resolver->isActivated())
			{
				resolvedName = resolver->resolveFileName(nameToResolve, currentPath);
				if (QFile::exists(resolvedName))
				{
					m_externalFileResolverCache.insert(qMakePair(nameToResolve, currentPath), resolvedName);
					return QFileInfo(resolvedName).canonicalFilePath();
				}
			}
		}
	}

	return QFileInfo(QDir(currentPath).absoluteFilePath(nameToResolve)).canonicalFilePath();
}

void ExternalFileResolver::clearCache()
{
	m_externalFileResolverCache.clear();
}

ExternalFileResolver * ExternalFileResolver::self()
{
	if (! s_self)
	{
		s_self = new ExternalFileResolver();
		XINXStaticDeleter::self()->add(s_self);
	}
	return s_self;
}
