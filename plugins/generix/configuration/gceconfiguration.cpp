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

// Qt header
#include <QXmlStreamReader>
#include <QIODevice>
#include <QFile>
#include <QDebug>
#include <QDir>

/* GceConfigurationParser */

class GceConfigurationParser : public QXmlStreamReader
{
public:
	GceConfigurationParser();

	bool loadFromFile(const QString & filename);

	/* OUTPUT */
	QString m_version, m_rootPath;
	int m_edition;
	QMultiHash<QString,BusinessViewInformation> m_fileRefToInformation;

private:
	void readUnknownElement();
	void readConfigElement();
	void readVersionElement();
	void readApplicationElement();

	void readBusinessViewDef();
	void readBusinessViewDefElement();
	void readPresentation();
	void readPresentationElement();

	QString m_configurationFileName;
	QHash<QString,BusinessViewInformation> m_nameToInformation;
	QMultiHash<QString,QString> m_fileRefToName;
};

GceConfigurationParser::GceConfigurationParser()
{

}

bool GceConfigurationParser::loadFromFile(const QString & filename)
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

	m_fileRefToInformation.clear();
	foreach(const QString & bvName, m_fileRefToName.keys())
	{
		foreach(const QString & fileRef, m_fileRefToName.values(bvName))
		{
			const QString ref = m_rootPath + "/" + fileRef;
			m_fileRefToInformation.insert(ref, m_nameToInformation.value(bvName));
		}
	}

	return ! error();
}

void GceConfigurationParser::readUnknownElement()
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

void GceConfigurationParser::readConfigElement()
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

void GceConfigurationParser::readVersionElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "version"));

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

void GceConfigurationParser::readApplicationElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "application"));

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (QXmlStreamReader::name() == "businessview_def")
			{
				readBusinessViewDef();
			}
			else if (QXmlStreamReader::name() == "presentation")
			{
				readPresentation();
			}
			else
				readUnknownElement();
		}
	}
}

void GceConfigurationParser::readBusinessViewDef()
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

void GceConfigurationParser::readBusinessViewDefElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "businessview"));

	BusinessViewInformation information;
	information.setBusinessViewName(attributes().value("name").toString());
	information.setTargetName(attributes().value("target").toString());
	information.setViewstructName(attributes().value("viewstruct").toString());

	information.setConfigurationFileName(m_configurationFileName);
	information.setConfigurationNumber(-1);

	m_nameToInformation.insert(information.businesViewName(), information);
	readUnknownElement();
}

void GceConfigurationParser::readPresentation()
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

void GceConfigurationParser::readPresentationElement()
{
	Q_ASSERT(isStartElement());

	const QString bv = attributes().value("businessview").toString();
	const QString fr = attributes().value("fileRef").toString();

	if (! m_fileRefToName.values(bv).contains(fr))
		m_fileRefToName.insert(bv, fr);

	readUnknownElement();
}

/* GceConfiguration */

GceConfiguration::GceConfiguration(const QString & filename) : m_configurationFileName(filename)
{
	GceConfigurationParser parser;
	parser.loadFromFile(filename);

	m_fileToInformation = parser.m_fileRefToInformation;
	m_version			= ConfigurationVersion(parser.m_version, parser.m_edition);
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
