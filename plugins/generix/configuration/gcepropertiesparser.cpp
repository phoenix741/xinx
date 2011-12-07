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
#include "gcepropertiesparser.h"

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

/* GcePropertiesParser */

GcePropertiesParser::GcePropertiesParser(const QString & filename) : GceConfigurationDefParser()
{
	setPriority(-100);
	_filename = filename;
	_directory_path = QFileInfo(_filename).absolutePath();
}

GcePropertiesParser::~GcePropertiesParser()
{
}

void GcePropertiesParser::startJob()
{
	interface()->setFilename(_filename);
	readGceProperties(_filename);
}

void GcePropertiesParser::readGceProperties(const QString & propertiesFileName)
{
	interface()->addFilename(propertiesFileName);

	// Ouverture du fichier XML
	const int options = XML_PARSE_NOENT;
	xmlDocPtr document = xmlReadFile(qPrintable(propertiesFileName), NULL, options);
	if (document == NULL)
	{
		throw GceConfigurationException(tr("Can't open the gce properties file"));
		return ;
	}

	// Get root node
	xmlNodePtr root = xmlDocGetRootElement(document);
	if (root == NULL)
	{
		throw GceConfigurationException(tr("Can't read the root element of the gce properties file"));
		xmlFreeDoc(document);
		return;
	}

	//  Create new context to support XPath.
	xmlXPathContextPtr xpathCtx = xmlXPathNewContext(document);
	if (xpathCtx == NULL)
	{
		throw GceConfigurationException(tr("Unable to create new XPath context"));
		xmlFreeDoc(document);
		return;
	}

	/* ConfigurationDef */

	// Read configuration file without xmlns
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar*)"string(/config/application/configurationDefinition/definition/@name)", xpathCtx);

	// If we don't find an valid string, we try to read the same xpath, with an xmlns (for version post 1.60)
	if (!(xpathObj && xpathObj->stringval && xpathObj->stringval[0]))
	{
		if (xpathObj)
		{
			xmlXPathFreeObject(xpathObj);
		}
		xmlXPathRegisterNs(xpathCtx, (xmlChar*)"gce", (xmlChar*)"http://www.generix.fr/technicalframework/gceproperties");
		xpathObj = xmlXPathEvalExpression((xmlChar*)"string(/gce:config/gce:application/gce:configurationDefinition/gce:definition/@name)", xpathCtx);
	}
	QString configurationDef = QDir(_directory_path).absoluteFilePath(QLatin1String((char*)xpathObj->stringval));
	xmlXPathFreeObject(xpathObj);

	/* Policies */

	xpathObj = xmlXPathEvalExpression((xmlChar*)"/config/application/presentation/alias/policy", xpathCtx);
	// If we don't find an valid string, we try to read the same xpath, with an xmlns (for version post 1.60)
	if (!(xpathObj && xpathObj->nodesetval && xpathObj->nodesetval->nodeNr))
	{
		if (xpathObj)
		{
			xmlXPathFreeObject(xpathObj);
		}
		xmlXPathRegisterNs(xpathCtx, (xmlChar*)"gce", (xmlChar*)"http://www.generix.fr/technicalframework/gceproperties");
		xpathObj = xmlXPathEvalExpression((xmlChar*)"/gce:config/gce:application/gce:presentation/gce:alias/gce:policy", xpathCtx);
	}
	if (xpathObj && xpathObj->nodesetval)
	{
		interface()->addAliasPolicy("ROOT_WEBAPP", _directory_path);

		xmlNodeSetPtr policies = xpathObj->nodesetval;
		for (int i = 0; i < policies->nodeNr; i++)
		{
			if (policies->nodeTab[i]->type != XML_ELEMENT_NODE) continue;

			xmlNodePtr policy = policies->nodeTab[i];
			QString policyName = QLatin1String((char*)xmlGetProp(policy, (xmlChar*)"name"));
			xmlNodePtr mappings = policies->nodeTab[i]->children;
			for (xmlNodePtr mapping = mappings; mapping != NULL; mapping = mapping->next)
			{
				if (mapping->type == XML_ELEMENT_NODE)
				{
					QString mappingValue = QLatin1String((char*)xmlGetProp(mapping, (xmlChar*)"value"));
					interface()->addAliasPolicy(policyName, mappingValue);
				}
			}
		}

		xmlXPathFreeObject(xpathObj);
	}
	else if (xpathObj)
	{
		xmlXPathFreeObject(xpathObj);
	}

	/* Cleanup */

	xmlXPathFreeContext(xpathCtx);
	xmlFreeDoc(document);

	/* Read configuration def */

	readConfigurationDef(configurationDef);

}
