/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
#include "gceconfigurationxmlparser2.h"

// Qt header
#include <QDir>

// 150 = V06.01-50
const ConfigurationVersion version150(6, 1, 50, 0);

/* GceConfigurationXmlParser2 */

GceConfigurationXmlParser2::GceConfigurationXmlParser2()
{
}

GceConfigurationXmlParser2::~GceConfigurationXmlParser2()
{
	// Cleanup function for the XML library.
	xmlCleanupParser();
}

bool GceConfigurationXmlParser2::loadFromFile(const QString & filename)
{
	xmlTextReaderPtr reader;
	int ret;

	reader = xmlReaderForFile(qPrintable(filename), NULL, 0);
	if (reader == NULL)
	{
		return false;
	}

	ret = xmlTextReaderRead(reader);
	while (ret == 1)
	{
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
		{
			const xmlChar *name = xmlTextReaderConstName(reader);

			if (QString::fromUtf8((char*)name) == "config")
				readConfigElement(reader);
			else
			{
				//raiseError("The file is not a configuration file.");
				xmlFreeTextReader(reader);
				return false;
			}

			ret = xmlTextReaderRead(reader);
		}
	}
	xmlFreeTextReader(reader);

	if (ConfigurationVersion(m_version) < version150)
	{
		const QDir directoryPath(m_parent ? m_parent->m_directoryPath : "");
		m_fileRefToInformation.clear();
		foreach(const QString & bvName, m_fileRefToName.keys())
		{
			foreach(const QString & fileRef, m_fileRefToName.values(bvName))
			{
				QString ref = m_rootPath + "/" + fileRef;
				if (m_parent)
				{
					const QString absolutePath = QDir(directoryPath.absoluteFilePath(ref)).canonicalPath();
					if (!absolutePath.isEmpty())
						ref = directoryPath.relativeFilePath(absolutePath);
				}

				m_fileRefToInformation.insert(ref, m_nameToInformation.value(bvName));
			}
		}
	}

	return true;
}

void GceConfigurationXmlParser2::readUnknownElement(xmlTextReader * reader)
{
	Q_ASSERT(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT);
	int ret;

	ret = xmlTextReaderRead(reader);
	while (ret == 1)
	{
		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
			break;

		if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
			readUnknownElement(reader);

		ret = xmlTextReaderRead(reader);
	}
}

void GceConfigurationXmlParser2::readConfigElement(xmlTextReader * reader)
{
	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readVersionElement(xmlTextReader * reader)
{
	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readApplicationElement(xmlTextReader * reader)
{
	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readBusinessViewDef(xmlTextReader * reader)
{
	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readBusinessViewDefElement(xmlTextReader * reader)
{
	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readPresentationElement(xmlTextReader * reader, const BusinessViewInformation & information)
{
	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readPresentation(xmlTextReader * reader)
{
	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readPresentationElement(xmlTextReader * reader)
{
	readUnknownElement(reader);
}

