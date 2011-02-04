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
#include "gceinterfacefactory.h"
#include "gceconfiguration.h"
#include "gceconfigurationdef.h"
#include "gceproperties.h"

// Qt header
#include <QFile>
#include <QDir>

GceInterfaceFactory::GceInterfaceFactory()
{
}

GceInterface * GceInterfaceFactory::createGceInterface(const QString & path)
{
	/*
	 * Recherche l'existance des fichiers
	 * - gce.properties.xml pour la GCE150
	 * - configurationdef.xml pour la GCE140 ou 130
	 * - configuration.xml pour le reste.
	 */
	try
	{
		if (QFile::exists(QDir(path).absoluteFilePath("gce.properties.xml")))
		{
			return new GceProperties(QDir(path).absoluteFilePath("gce.properties.xml"));
		}
		else if (QFile::exists(QDir(path).absoluteFilePath("configurationdef.xml")))
		{
			return new GceConfigurationDef(QDir(path).absoluteFilePath("configurationdef.xml"));
		}
		else if (QFile::exists(QDir(path).absoluteFilePath("configuration.xml")))
		{
			return new GceConfiguration(QDir(path).absoluteFilePath("configuration.xml"));
		}
	}
	catch (GceInterfaceException e)
	{
		return NULL;
	}

	// Si aucun des fichiers n'est trouvé, nous ne somme pas dans une WebModule Generix. Il n'y a donc pas
	// de fichier de configuration. On peut laisser tomber.
	return NULL;
}
