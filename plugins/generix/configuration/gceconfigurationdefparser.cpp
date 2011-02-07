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
#include "gceconfigurationdefparser.h"
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

GceConfigurationDefParser::GceConfigurationDefParser()
{

}

GceConfigurationDefParser::GceConfigurationDefParser(const QString & filename) : _directory_path(QFileInfo(filename).absolutePath())
{
	_filename = filename;
}

GceConfigurationDefParser::~GceConfigurationDefParser()
{

}

void GceConfigurationDefParser::startJob()
{
	interface()->setFilename(_filename);
	readConfigurationDef(_filename);
}

void GceConfigurationDefParser::readConfigurationDef(const QString & configurationdefFileName)
{
	interface()->addFilename(configurationdefFileName);

	QFile configurationdef(configurationdefFileName);
	if (! configurationdef.open(QFile::ReadOnly))
		throw GceConfigurationException(tr("Can't open configurationdef file"));

	QDomDocument document;
	if (! document.setContent(&configurationdef))
		throw GceConfigurationException(tr("Can't read the content of the configurationdef file"));

	QDomElement root = document.documentElement();

	QDomElement configuration = root.firstChildElement("configuration");
	if (! configuration.isNull())
	{
		int configurationNumber = 0;
		QDomElement conffile = configuration.firstChildElement("file");
		while (! conffile.isNull())
		{
			const QString conf_filename = QDir(_directory_path).absoluteFilePath(conffile.attribute("name"));
			readConfigurationFile(configurationNumber++, conf_filename);

			emit addConfiguration(conf_filename);

			conffile = conffile.nextSiblingElement("file");
		}
	}
	QDomElement translation = root.firstChildElement("translation");
	if (! translation.isNull())
	{
		QDomElement translationFile = translation.firstChildElement("file");
		while (! translationFile.isNull())
		{
			const QString dico_filename = QDir(_directory_path).absoluteFilePath(translationFile.attribute("name"));
			interface()->addDictionnary(dico_filename);

			emit addDictionary(dico_filename);

			translationFile = translationFile.nextSiblingElement("file");
		}
	}
}

void GceConfigurationDefParser::readConfigurationFile(int configurationIndex, const QString & configurationFileName)
{
	GceConfigurationXmlParser2 parser;
	parser.m_quick = ! SelfGceSettings::self()->config().readConfigurations;
	parser._gce_configuration = interface();
	parser.m_configurationNumber = configurationIndex;
	parser.loadFromFile(configurationFileName);

	interface()->addFilename(configurationFileName);

	interface()->addBusinessView(parser.m_fileRefToInformation);
	if (!parser.m_version.isEmpty())
	{
		interface()->setVersion(ConfigurationVersion(parser.m_version, parser.m_edition));
	}
}

