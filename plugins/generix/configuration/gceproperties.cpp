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
#include "gceproperties.h"

// Qt header
#include <QXmlStreamReader>
#include <QIODevice>
#include <QDebug>
#include <QDomDocument>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStack>

// Libxml2 header
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/tree.h>

/* GceConfiguration150Parser */

class GceConfiguration150Parser : public QXmlStreamReader
{
public:
	GceConfiguration150Parser();

	bool loadFromFile(const QString & filename);

	/* INPUT */
	GceProperties * m_parent;
	int m_configurationNumber;

	/* OUTPUT */
	QString m_version;
	int m_edition;
	QMultiHash<QString,BusinessViewInformation> m_fileRefToInformation;

private:
	void readUnknownElement();
	void readConfigElement();
	void readVersionElement();
	void readApplicationElement();

	void readBusinessViewDef();
	void readBusinessViewDefElement();
	void readPresentationElement(const BusinessViewInformation & information);

	QString m_configurationFileName;
};

GceConfiguration150Parser::GceConfiguration150Parser()
{

}

bool GceConfiguration150Parser::loadFromFile(const QString & filename)
{
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

	return ! error();
}

void GceConfiguration150Parser::readUnknownElement()
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

void GceConfiguration150Parser::readConfigElement()
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

void GceConfiguration150Parser::readVersionElement()
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

void GceConfiguration150Parser::readApplicationElement()
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
			else
				readUnknownElement();
		}
	}
}

void GceConfiguration150Parser::readBusinessViewDef()
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

void GceConfiguration150Parser::readBusinessViewDefElement()
{
	Q_ASSERT(isStartElement() && (QXmlStreamReader::name() == "businessview"));

	BusinessViewInformation information;
	information.setBusinessViewName(attributes().value("name").toString());
	information.setTargetName(attributes().value("target").toString());
	information.setViewstructName(attributes().value("viewstruct").toString());

	information.setConfigurationFileName(m_configurationFileName);
	information.setConfigurationNumber(m_configurationNumber);

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
			readPresentationElement(information);
	}
}

void GceConfiguration150Parser::readPresentationElement(const BusinessViewInformation & information)
{
	Q_ASSERT(isStartElement());

	QString fileRef = m_parent->resolveFileName(attributes().value("fileRef").toString());
	if (QDir(fileRef).isAbsolute())
	{
		fileRef = QDir(m_parent->m_directoryPath).relativeFilePath(fileRef);
	}
	m_fileRefToInformation.insert(fileRef, information);

	readUnknownElement();
}

/* GceProperties */

GceProperties::GceProperties(const QString & filename) : GceConfigurationDef(), m_propertiesFilename(filename)
{
	m_directoryPath = QFileInfo(filename).absolutePath();
	m_policy.insert("ROOT_WEBAPP", QStringList() << m_directoryPath);

	readGceProperties(m_propertiesFilename);
}

GceProperties::~GceProperties()
{
}

void GceProperties::readGceProperties(const QString & propertiesFileName)
{
	m_filenames.append(propertiesFileName);

	// Ouverture du fichier XML
	const int options = XML_PARSE_NOENT;
	xmlDocPtr document = xmlReadFile(qPrintable(propertiesFileName), NULL, options);
	if (document == NULL)
	{
		throw GceInterfaceException(tr("Can't open the gce properties file"));
		return ;
	}

	// Récupération de la racine
	xmlNodePtr root = xmlDocGetRootElement(document);
	if (root == NULL)
	{
		throw GceInterfaceException(tr("Can't read the root element of the gce properties file"));
		xmlFreeDoc(document);
		return;
	}

	//  Create new context to support XPath.
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(document);
	if (xpathCtx == NULL)
	{
		throw GceInterfaceException(tr("Unable to create new XPath context"));
		xmlFreeDoc(document);
		return;
	}

	/* ConfigurationDef */

	const xmlChar * configurationDefPath = (xmlChar*)"string(/config/application/configurationDefinition/definition/@name)";
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(configurationDefPath, xpathCtx);
	m_configurationDef = QDir(m_directoryPath).absoluteFilePath(QLatin1String((char*)xpathObj->stringval));
	xmlXPathFreeObject(xpathObj);

	/* Policies */

	const xmlChar * policiesPath = (xmlChar*)"/config/application/presentation/alias/policy";
	xpathObj = xmlXPathEvalExpression(policiesPath, xpathCtx);
	xmlNodeSetPtr policies = xpathObj->nodesetval;
	for (int i = 0; i < policies->nodeNr; i++)
	{
		if (policies->nodeTab[i]->type != XML_ELEMENT_NODE) continue;
		QStringList mappingList;

		xmlNodePtr policy = policies->nodeTab[i];
		QString policyName = QLatin1String((char*)xmlGetProp(policy, (xmlChar*)"name"));
		xmlNodePtr mappings = policies->nodeTab[i]->children;
		for (xmlNodePtr mapping = mappings; mapping != NULL; mapping = mapping->next)
		{
			if (mapping->type == XML_ELEMENT_NODE)
			{
				QString mappingValue = QLatin1String((char*)xmlGetProp(mapping, (xmlChar*)"value"));
				mappingList.append(mappingValue);
			}
		}

		m_policy.insert(policyName, mappingList);
	}

	xmlXPathFreeObject(xpathObj);

	/* Cleanup */

	xmlXPathFreeContext(xpathCtx);
	xmlFreeDoc(document);

	/* Read configuration def */

	readConfigurationDef(m_configurationDef);

}

void GceProperties::readConfigurationFile(int configurationIndex, const QString & configurationFileName)
{
	GceConfiguration150Parser parser;
	parser.m_parent = this;
	parser.m_configurationNumber = configurationIndex;
	parser.loadFromFile(configurationFileName);
	m_filenames.append(configurationFileName);

	m_fileToInformation += parser.m_fileRefToInformation;
	m_version            = ConfigurationVersion(parser.m_version, parser.m_edition);
}

QStringList GceProperties::generateFileName(const QString & filename)
{
	QStack<QString> nameToResolve;
	QStringList resolvedName;
	QRegExp regexp("(\\{|\\(\\()(.*)(\\}|\\)\\))");

	nameToResolve << filename;

	while (nameToResolve.size())
	{
		const QString & name = nameToResolve.pop();

		if (regexp.indexIn(name) != -1)
		{
			QString key        = regexp.cap(2);
			QStringList values = m_policy.value(key);

			QStringListIterator value(values);
			value.toBack();
			while (value.hasPrevious())
			{
				QString result = name;
				result.replace(regexp.cap(0), value.previous());
				nameToResolve.push(result);
			}
		}
		else
		{
			resolvedName.append(name);
		}
	}

	return resolvedName;
}

QString GceProperties::rootFilename()
{
	return m_propertiesFilename;
}

QStringList GceProperties::filenames()
{
	return m_filenames;
}

QString GceProperties::resolveFileName(const QString & filename)
{
	QStringList names = generateFileName(filename);
	foreach(QString name, names)
	{
		if (QFile::exists(name))
			return name;
	}
	return filename;
}
