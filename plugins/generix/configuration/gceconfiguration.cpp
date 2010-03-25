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
#include "gceconfiguration.h"
#include "gceconfigurationxmlparser.h"

// Qt header
#include <QXmlStreamReader>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include <QDir>

/* GceConfiguration */

GceConfiguration::GceConfiguration(const QString & filename) : m_configurationFileName(filename)
{
	GceConfigurationXmlParser parser;
	parser.m_configurationNumber = -1;
	parser.m_parent = 0;
	parser.loadFromFile(filename);

	m_fileToInformation = parser.m_fileRefToInformation;
	m_version           = ConfigurationVersion(parser.m_version, parser.m_edition);
}

GceConfiguration::~GceConfiguration()
{
}

ConfigurationVersion GceConfiguration::version()
{
	return m_version;
}

QString GceConfiguration::rootFilename()
{
	return m_configurationFileName;
}

QStringList GceConfiguration::filenames()
{
	return QStringList() << m_configurationFileName;
}

QStringList GceConfiguration::dictionnaries()
{
	return QStringList();
}

QList<BusinessViewInformation> GceConfiguration::businessView(const QString & filename)
{
	return m_fileToInformation.values(filename);
}

QList<BusinessViewInformation> GceConfiguration::businessViews()
{
	return m_fileToInformation.values();
}

QString GceConfiguration::resolveFileName(const QString & filename)
{
	return filename;
}
