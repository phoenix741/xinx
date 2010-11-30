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

	// Get root node
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

	// Read configuration file without xmlns
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)"string(/config/application/configurationDefinition/definition/@name)", xpathCtx);

	// If we don't find an valid string, we try to read the same xpath, with an xmlns (for version post 1.60)
	if (! (xpathObj && xpathObj->stringval[0]))
	{
		xmlXPathRegisterNs(xpathCtx, (xmlChar*)"x", (xmlChar*)"http://www.generix.fr/technicalframework/gceproperties");
		xpathObj = xmlXPathEvalExpression((xmlChar*)"string(/x:config/x:application/x:configurationDefinition/x:definition/@name)", xpathCtx);
	}
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
