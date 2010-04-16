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
#include "gceconfigurationdef.h"
#include "gceconfigurationxmlparser.h"
#include "gceconfigurationxmlparser2.h"
#include "config/selfgcesettings.h"

// Qt header
#include <QXmlStreamReader>
#include <QIODevice>
#include <QDebug>
#include <QDomDocument>
#include <QFileInfo>
#include <QDir>
#include <QFile>

/* GceConfigurationDef */

GceConfigurationDef::GceConfigurationDef()
{

}

GceConfigurationDef::GceConfigurationDef(const QString & filename)
{
	m_directoryPath = QFileInfo(filename).absolutePath();

	readConfigurationDef(filename);
}

GceConfigurationDef::~GceConfigurationDef()
{

}

void GceConfigurationDef::readConfigurationDef(const QString & configurationdefFileName)
{
	m_filenames.append(configurationdefFileName);

	m_configurationFileName = configurationdefFileName;

	QFile configurationdef(configurationdefFileName);
	if (! configurationdef.open(QFile::ReadOnly))
		throw GceInterfaceException(tr("Can't open configurationdef file"));

	QDomDocument document;
	if (! document.setContent(&configurationdef))
		throw GceInterfaceException(tr("Can't read the content of the configurationdef file"));

	QDomElement root = document.documentElement();

	QDomElement configuration = root.firstChildElement("configuration");
	if (! configuration.isNull())
	{
		int configurationNumber = 0;
		QDomElement conffile = configuration.firstChildElement("file");
		while (! conffile.isNull())
		{
			readConfigurationFile(configurationNumber++, QDir(m_directoryPath).absoluteFilePath(conffile.attribute("name")));

			conffile = conffile.nextSiblingElement("file");
		}
	}
	QDomElement translation = root.firstChildElement("translation");
	if (! translation.isNull())
	{
		QDomElement translationFile = translation.firstChildElement("file");
		while (! translationFile.isNull())
		{
			m_dictionnaries += QDir(m_directoryPath).absoluteFilePath(translationFile.attribute("name"));

			translationFile = translationFile.nextSiblingElement("file");
		}
	}
}

void GceConfigurationDef::readConfigurationFile(int configurationIndex, const QString & configurationFileName)
{
	GceConfigurationXmlParser2 parser;
	parser.m_quick = ! SelfGceSettings::self()->config().readConfigurations;
	parser.m_parent = this;
	parser.m_configurationNumber = configurationIndex;
	parser.loadFromFile(configurationFileName);
	m_filenames.append(configurationFileName);

	m_fileToInformation += parser.m_fileRefToInformation;
	if (!parser.m_version.isEmpty())
		m_version            = ConfigurationVersion(parser.m_version, parser.m_edition);
}

ConfigurationVersion GceConfigurationDef::version()
{
	return m_version;
}

QStringList GceConfigurationDef::dictionnaries()
{
	return m_dictionnaries;
}

QString GceConfigurationDef::rootFilename()
{
	return m_configurationFileName;
}

QStringList GceConfigurationDef::filenames()
{
	return m_filenames;
}

QList<BusinessViewInformation> GceConfigurationDef::businessView(const QString & filename)
{
	return m_fileToInformation.values(filename);
}

QList<BusinessViewInformation> GceConfigurationDef::businessViews()
{
	return m_fileToInformation.values();
}

QString GceConfigurationDef::resolveFileName(const QString & filename)
{
	return filename;
}
