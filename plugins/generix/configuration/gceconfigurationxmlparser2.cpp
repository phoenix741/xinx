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
#include <QDebug>

// 150 = V06.01-50
const ConfigurationVersion version150(6, 1, 50, 0);

/* GceConfigurationXmlParser2 */

GceConfigurationXmlParser2::GceConfigurationXmlParser2()
{
	m_version = QString();
	m_edition = -1;
	m_quick   = false;
}

GceConfigurationXmlParser2::~GceConfigurationXmlParser2()
{
	// Cleanup function for the XML library.
	//xmlCleanupParser();
}

bool GceConfigurationXmlParser2::loadFromFile(const QString & filename)
{
	m_fileRefToName.clear();
	m_nameToInformation.clear();
	m_configurationFileName = filename;

	xmlTextReaderPtr reader;
	int ret;

	reader = xmlReaderForFile(qPrintable(filename), NULL, 0);
	if (reader == NULL)
	{
		return false;
	}

	try
	{
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
			}

			ret = xmlTextReaderRead(reader);
		}

		if (ConfigurationVersion(m_version) < version150)
		{
			const QDir directoryPath(_gce_configuration ? QFileInfo(_gce_configuration->filename()).absolutePath() : 0);
			m_fileRefToInformation.clear();
			foreach(const QString & bvName, m_fileRefToName.keys())
			{
				foreach(const QString & fileRef, m_fileRefToName.values(bvName))
				{
					QString ref = m_rootPath + "/" + fileRef;
					if (_gce_configuration)
					{
						const QString absolutePath = QDir(directoryPath.absoluteFilePath(ref)).canonicalPath();
						if (!absolutePath.isEmpty())
							ref = directoryPath.relativeFilePath(absolutePath);
					}

					m_fileRefToInformation.insert(ref, m_nameToInformation.value(bvName));
				}
			}
		}
	}
	catch (...)
	{
		// Sortie rapide
	}

	xmlFreeTextReader(reader);

	return true;
}

void GceConfigurationXmlParser2::readUnknownElement(xmlTextReader * reader)
{
	Q_ASSERT(xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT);

	if (!xmlTextReaderIsEmptyElement(reader))
	{
		int startLevel, level;
		startLevel = level = xmlTextReaderDepth(reader);

		int ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
			{
				if (startLevel == level)
				{
					break;
				}
				else
				{
					level--;
				}
			}

			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
			{
				if (!xmlTextReaderIsEmptyElement(reader))
					level++;
			}

			ret = xmlTextReaderRead(reader);
		}
	}
}

void GceConfigurationXmlParser2::readConfigElement(xmlTextReader * reader)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) && (QString::fromUtf8((char*)xmlTextReaderConstName(reader)) == "config"));

	if (!xmlTextReaderIsEmptyElement(reader))
	{
		int ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
				break;

			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
			{
				const xmlChar *name = xmlTextReaderConstName(reader);

				if (QString::fromUtf8((char*)name) == "version")
					readVersionElement(reader);
				else if (!m_quick && (QString::fromUtf8((char*)name) == "application"))
					readApplicationElement(reader);
				else
					readUnknownElement(reader);
			}

			ret = xmlTextReaderRead(reader);
		}
	}
}

void GceConfigurationXmlParser2::readVersionElement(xmlTextReader * reader)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) && (QString::fromUtf8((char*)xmlTextReaderConstName(reader)) == "version"));

	if (xmlTextReaderHasAttributes(reader))
	{
		xmlChar * release = xmlTextReaderGetAttribute(reader, (xmlChar*)"release");
		xmlChar * sub     = xmlTextReaderGetAttribute(reader, (xmlChar*)"sub");

		m_version = QString::fromUtf8((char*)release);
		m_edition = QString::fromUtf8((char*)sub).toInt();

		xmlFree(release);
		xmlFree(sub);

		readUnknownElement(reader);
	}
	else
	{
		if (!xmlTextReaderIsEmptyElement(reader))
		{
			int ret = xmlTextReaderRead(reader);
			while (ret == 1)
			{
				if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
					break;

				if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
				{
					const xmlChar *name = xmlTextReaderConstName(reader);
					xmlChar *value = xmlTextReaderReadString(reader);

					if (QString::fromUtf8((char*)name) == "numero")
						m_version = QString::fromUtf8((char*)value);
					else if (QString::fromUtf8((char*)name) == "edition_speciale")
						m_edition = QString::fromUtf8((char*)value).toInt();

					xmlFree(value);
					readUnknownElement(reader);
				}

				ret = xmlTextReaderRead(reader);
			}
		}
	}
	if (m_quick)
	{
		// File is loaded, quick exit
		throw "Quick quit";
	}
}

void GceConfigurationXmlParser2::readApplicationElement(xmlTextReader * reader)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) && (QString::fromUtf8((char*)xmlTextReaderConstName(reader)) == "application"));

	if (!xmlTextReaderIsEmptyElement(reader))
	{
		int ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
				break;

			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
			{
				const xmlChar *name = xmlTextReaderConstName(reader);

				if (QString::fromUtf8((char*)name) == "businessview_def")
				{
					readBusinessViewDef(reader);
				}
				else if ((ConfigurationVersion(m_version) < version150) && (QString::fromUtf8((char*)name) == "presentation"))
				{
					readPresentation(reader);
				}
				else
					readUnknownElement(reader);
			}

			ret = xmlTextReaderRead(reader);
		}
	}
}

void GceConfigurationXmlParser2::readBusinessViewDef(xmlTextReader * reader)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) && (QString::fromUtf8((char*)xmlTextReaderConstName(reader)) == "businessview_def"));

	if (!xmlTextReaderIsEmptyElement(reader))
	{
		int ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
				break;

			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
			{
				const xmlChar *name = xmlTextReaderConstName(reader);

				if (QString::fromUtf8((char*)name) == "businessview")
				{
					readBusinessViewDefElement(reader);
				}
				else
					readUnknownElement(reader);
			}

			ret = xmlTextReaderRead(reader);
		}
	}
}

void GceConfigurationXmlParser2::readBusinessViewDefElement(xmlTextReader * reader)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) && (QString::fromUtf8((char*)xmlTextReaderConstName(reader)) == "businessview"));

	xmlChar * name       = xmlTextReaderGetAttribute(reader, (xmlChar*)"name");
	xmlChar * target     = xmlTextReaderGetAttribute(reader, (xmlChar*)"target");
	xmlChar * viewstruct = xmlTextReaderGetAttribute(reader, (xmlChar*)"viewstruct");

	BusinessViewInformation information;
	information.setBusinessViewName(QString::fromUtf8((char*)name));
	information.setTargetName(QString::fromUtf8((char*)target));
	information.setViewstructName(QString::fromUtf8((char*)viewstruct));

	information.setConfigurationFileName(m_configurationFileName);
	information.setConfigurationNumber(m_configurationNumber);

	m_nameToInformation.insert(information.businesViewName(), information);

	xmlFree(name);
	xmlFree(target);
	xmlFree(viewstruct);

	if (ConfigurationVersion(m_version) >= version150)
	{
		if (!xmlTextReaderIsEmptyElement(reader))
		{
			int ret = xmlTextReaderRead(reader);
			while (ret == 1)
			{
				if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
					break;

				if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
					readPresentationElement(reader, information);

				ret = xmlTextReaderRead(reader);
			}
		}
	}
	else
	{
		readUnknownElement(reader);
	}
}

void GceConfigurationXmlParser2::readPresentationElement(xmlTextReader * reader, const BusinessViewInformation & information)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT));

	xmlChar * xmlFileRef = xmlTextReaderGetAttribute(reader, (xmlChar*)"fileRef");
	QString fileRef = QString::fromUtf8((char*)xmlFileRef);
	xmlFree(xmlFileRef);

	if (_gce_configuration)
	{
		fileRef = _gce_configuration->resolveFileName(fileRef);
		if (QDir(fileRef).isAbsolute())
		{
			fileRef = QDir(QFileInfo(_gce_configuration->filename()).absolutePath()).relativeFilePath(fileRef);
		}
	}
	m_fileRefToInformation.insert(fileRef, information);

	readUnknownElement(reader);
}

void GceConfigurationXmlParser2::readPresentation(xmlTextReader * reader)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) && (QString::fromUtf8((char*)xmlTextReaderConstName(reader)) == "presentation"));

	xmlChar * rootPath = xmlTextReaderGetAttribute(reader, (xmlChar*)"rootPath");
	if (rootPath)
		m_rootPath = QString::fromUtf8((char*)rootPath);
	xmlFree(rootPath);

	if (!xmlTextReaderIsEmptyElement(reader))
	{
		int ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT)
				break;

			if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT)
				readPresentationElement(reader);

			ret = xmlTextReaderRead(reader);
		}
	}
}

void GceConfigurationXmlParser2::readPresentationElement(xmlTextReader * reader)
{
	Q_ASSERT((xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT));

	xmlChar * xmlBv         = xmlTextReaderGetAttribute(reader, (xmlChar*)"businessview");
	xmlChar * xmlFileRef    = xmlTextReaderGetAttribute(reader, (xmlChar*)"fileRef");

	const QString bv = QString::fromUtf8((char*)xmlBv);
	const QString fr = QString::fromUtf8((char*)xmlFileRef);

	xmlFree(xmlBv);
	xmlFree(xmlFileRef);

	if (! m_fileRefToName.values(bv).contains(fr))
		m_fileRefToName.insert(bv, fr);

	readUnknownElement(reader);
}

