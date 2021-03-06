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
#include "externalfileresolver.h"
#include "core/xinxcore.h"
#include "plugins/xinxpluginsloader.h"
#include <plugins/xinxpluginelement.h>
#include <plugins/interfaces/resolver.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QDir>
#include <QDebug>

/* ExternalFileResolver */

ExternalFileResolver::ExternalFileResolver(XinxProject::ProjectPtrWeak project) : _project(project)
{
}

ExternalFileResolver::~ExternalFileResolver()
{

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

QString ExternalFileResolver::resolveFileName(const QString & nameToResolve, const ResolverContextInformation & ctxt)
{
	XinxProject::ProjectPtr project = _project.toStrongRef();

	QString resolvedName = m_externalFileResolverCache.value(qMakePair(nameToResolve, ctxt));
	if (! resolvedName.isEmpty() && QFile::exists(resolvedName))
		return resolvedName;

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		IResolverPlugin * resolverPlugin = qobject_cast<IResolverPlugin*>(plugin->plugin());
		if (! resolverPlugin)  continue;

		foreach(IFileResolverPlugin * resolver, resolverPlugin->fileResolvers())
		{
			resolvedName = nameToResolve;
			bool hasResolve = resolver->resolveFileName(nameToResolve, resolvedName, ctxt);
			if (hasResolve && QFile::exists(resolvedName))
			{
				m_externalFileResolverCache.insert(qMakePair(nameToResolve, ctxt), resolvedName);
				return QDir::cleanPath(resolvedName);
			}
		}
	}

	/* Failed to find the rigth file with resolver */
	resolvedName = QDir(ctxt.currentPath()).absoluteFilePath(nameToResolve);
	if (QFile::exists(resolvedName))
	{
		return QDir::cleanPath(resolvedName);
	}
	else
	{
		/* The file doesn't exist */
		return resolvedName;
	}
}

QStringList ExternalFileResolver::resolvePath(const QString& nameToResolve, const ResolverContextInformation &ctxt)
{
	XinxProject::ProjectPtr project = _project.toStrongRef();
	QStringList result;

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		IResolverPlugin * resolverPlugin = qobject_cast<IResolverPlugin*>(plugin->plugin());
		if (! resolverPlugin)  continue;

		foreach(IFileResolverPlugin * resolver, resolverPlugin->fileResolvers())
		{
			result << resolver->resolvePath(nameToResolve, ctxt);
		}
	}

	QString resolvedName = QDir(ctxt.currentPath()).absoluteFilePath(nameToResolve);
	if (QFile::exists(resolvedName))
	{
		result << resolvedName;
	}

	return result;
}

ResolverContextInformation ExternalFileResolver::createContextInformation(const QString & filename, const ResolverContextInformation & ctxt)
{
	XinxProject::ProjectPtr project = _project.toStrongRef();
	ResolverContextInformation result = ctxt;
	if (result.isEmpty())
	{
		result.setOriginalFileName(filename);
		result.setProject(project);
		result.setCurrentPath(project->projectPath());
	}

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		IResolverPlugin * resolverPlugin = qobject_cast<IResolverPlugin*>(plugin->plugin());
		if (! resolverPlugin)  continue;

		foreach(IFileResolverPlugin * resolver, resolverPlugin->fileResolvers())
		{
			result = resolver->createContextInformation(filename, result);
		}
	}

	return result;
}

void ExternalFileResolver::clearCache()
{
	m_externalFileResolverCache.clear();
}
