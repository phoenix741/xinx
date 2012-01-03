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
#include "gce150fileresolver.h"
#include "projectproperty/generixproject.h"
#include "configuration/configurationmanager.h"
#include <jobs/xinxjobdelayedexception.h>

// Qt header
#include <QDir>

/* Gce150FileResolver */

Gce150FileResolver::Gce150FileResolver()
{
}

Gce150FileResolver::~Gce150FileResolver()
{
}

QString Gce150FileResolver::staticId()
{
	return "gce150resolver";
}

QString Gce150FileResolver::id()
{
	return staticId();
}

QString Gce150FileResolver::name()
{
	return tr("GCE150 File Resolver");
}

QStringList Gce150FileResolver::resolvePath(const QString& path, const ResolverContextInformation &ctxt)
{
	XinxProject::ProjectPtr project = ctxt.project();
	if (project && project.staticCast<GenerixProject>()->isGenerixActivated() && ConfigurationManager::manager(project))
	{
		QSharedPointer<GceConfiguration> interface = ConfigurationManager::manager(project)->getInterface();
		if (interface)
		{
			return interface->resolvePath(path, ctxt.getProperty("module").toString());
		}

		return QStringList();
	}
	else
	{
		return QStringList();
	}
}

bool Gce150FileResolver::resolveFileName(const QString& nameToResolve, QString& resolvedName, const ResolverContextInformation &ctxt)
{
	XinxProject::ProjectPtr project = ctxt.project();
	if (project && project.staticCast<GenerixProject>()->isGenerixActivated())
	{
		// Si la conf n'est pas encore chargé, on génère une exception il n'est pas possible de résoudre l'alias
		/* L'exception a normallement déjà été levé lors de la création du context */
		if (! ConfigurationManager::manager(project)->isReady())
		{
			throw XinxJobDelayedException();
		}

		QSharedPointer<GceConfiguration> interface = ConfigurationManager::manager(project)->getInterface();
		if (interface)
		{
			resolvedName = interface->resolveFileName(nameToResolve, ctxt.getProperty("module").toString());
		}

		return true;
	}
	else
	{
		return false;
	}
}

ResolverContextInformation Gce150FileResolver::createContextInformation(const QString & filename, const ResolverContextInformation & ctxt)
{
	ResolverContextInformation result = ctxt;

	XinxProject::ProjectPtr project = ctxt.project();
	if (project && project.staticCast<GenerixProject>()->isGenerixActivated())
	{
		// Si la conf n'est pas encore chargé, on génère une exception il n'est pas possible de résoudre l'alias
		if (! ConfigurationManager::manager(project)->isReady())
		{
			throw XinxJobDelayedException();
		}

		QSharedPointer<GceConfiguration> interface = ConfigurationManager::manager(project)->getInterface();
		if (interface)
		{
			QString module;
			foreach(const BusinessViewInformation information, interface->businessView(filename))
			{
				if (! information.moduleName().isEmpty())
				{
					module = information.moduleName();
				}
			}

			if (! module.isEmpty())
			{
				result.setProperty("module", module);
			}
		}
	}

	return result;
}

