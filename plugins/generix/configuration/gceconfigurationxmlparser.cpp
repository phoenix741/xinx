/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include "gceconfigurationxmlparser.h"
#include "gceconfigurationdef.h"

// 150 = V06.01-50
const ConfigurationVersion version150(6, 1, 50, 0);

/* GceConfigurationXmlParser */


GceConfigurationXmlParser::GceConfigurationXmlParser()
{

}

bool GceConfigurationXmlParser::loadFromFile(const QString & filename)
{
	m_fileRefToName.clear();
	m_nameToInformation.clear();
	m_configurationFileName = filename;

	QFile device(filename);
	if (! device.open(QFile::ReadOnly))
	{
		return false;
	}

	setDevice(&device);

	while (! atEnd())
	{
		readNext();

		if (isStartElement())
		{
			if (name() == "config")
				readConfigElement();
			else
				raiseError("The file is not a configuration file.");
		}
	}

	if (ConfigurationVersion(m_version) < version150)
	{
		const QDir directoryPath(m_parent->m_directoryPath);
		m_fileRefToInformation.clear();
		foreach(const QString & bvName, m_fileRefToName.keys())
		{
			foreach(const QString & fileRef, m_fileRefToName.values(bvName))
			{
				QString ref = m_rootPath + "/" + fileRef;
				const QString absolutePath = QDir(directoryPath.absoluteFilePath(ref)).canonicalPath();
				if (!absolutePath.isEmpty())
					ref = directoryPath.relativeFilePath(absolutePath);

				m_fileRefToInformation.insert(ref, m_nameToInformation.value(bvName));
			}
		}
	}
	return ! error();
}

void GceConfigurationXmlParser::readUnknownElement()
{
	Q_ASSERT(isStartElement());

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
			readUnknownElement();
	}
}

void GceConfigurationXmlParser::readConfigElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "config"));

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "version")
				readVersionElement();
			else if (QXmlStreamReader::name() == "application")
				readApplicationElement();
			else
				readUnknownElement();
		}
	}
}

void GceConfigurationXmlParser::readVersionElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "version"));

	if (!attributes().value("release").isNull())
	{
		m_version = attributes().value("release").toString();
		m_edition = attributes().value("sub").toString().toInt();
		readUnknownElement();
	}
	else
	{
		while (!atEnd())
		{
			readNext();

			if (isEndElement())
				break;

			if (isStartElement())
			{
				if (QXmlStreamReader::name() == "numero")
					m_version = readElementText();
				else if (QXmlStreamReader::name() == "edition_speciale")
					m_edition = readElementText().toInt();
				else
					readUnknownElement();
			}
		}
	}
}

void GceConfigurationXmlParser::readApplicationElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "application"));

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			QString name = QXmlStreamReader::name().toString();
			if (QXmlStreamReader::name() == "businessview_def")
			{
				readBusinessViewDef();
			}
			else if ((ConfigurationVersion(m_version) < version150) && (QXmlStreamReader::name() == "presentation"))
			{
				readPresentation();
			}
			else
				readUnknownElement();
		}
	}
}

void GceConfigurationXmlParser::readBusinessViewDef()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "businessview_def"));

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "businessview")
			{
				readBusinessViewDefElement();
			}
			else
				readUnknownElement();
		}
	}
}

void GceConfigurationXmlParser::readBusinessViewDefElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "businessview"));

	BusinessViewInformation information;
	information.setBusinessViewName(attributes().value("name").toString());
	information.setTargetName(attributes().value("target").toString());
	information.setViewstructName(attributes().value("viewstruct").toString());

	information.setConfigurationFileName(m_configurationFileName);
	information.setConfigurationNumber(m_configurationNumber);

	m_nameToInformation.insert(information.businesViewName(), information);

	if (ConfigurationVersion(m_version) >= version150)
	{
		while (!atEnd())
		{
			readNext();

			if (isEndElement())
				break;

			if (isStartElement())
				readPresentationElement(information);
		}
	}
	else
	{
		readUnknownElement();
	}
}

void GceConfigurationXmlParser::readPresentationElement(const BusinessViewInformation & information)
{
	Q_ASSERT(isStartElement());

	QString fileRef = attributes().value("fileRef").toString();
	if (m_parent)
	{
		fileRef = m_parent->resolveFileName(fileRef);
		if (QDir(fileRef).isAbsolute())
		{
			fileRef = QDir(m_parent->m_directoryPath).relativeFilePath(fileRef);
		}
	}
	m_fileRefToInformation.insert(fileRef, information);

	readUnknownElement();
}

void GceConfigurationXmlParser::readPresentation()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "presentation"));
	if (! attributes().value("rootPath").isEmpty())
		m_rootPath = attributes().value("rootPath").toString();

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
			readPresentationElement();
	}
}

void GceConfigurationXmlParser::readPresentationElement()
{
	Q_ASSERT(isStartElement());

	const QString bv = attributes().value("businessview").toString();
	const QString fr = attributes().value("fileRef").toString();

	if (! m_fileRefToName.values(bv).contains(fr))
		m_fileRefToName.insert(bv, fr);

	readUnknownElement();
}
