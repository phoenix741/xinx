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

bool Gce150FileResolver::resolveFileName(const QString& nameToResolve, QString& resolvedName, const QString& currentPath, XinxProject::ProjectPtr project)
{
	Q_UNUSED(currentPath);

	if (project && project.staticCast<GenerixProject>()->isGenerixActivated())
	{
		GceConfiguration * interface = ConfigurationManager::manager(project)->getInterface();
		if (interface)
		{
			resolvedName = interface->resolveFileName(nameToResolve);
		}

		return true;
	}
	else
	{
		return false;
	}

}
