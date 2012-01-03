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

#pragma once
#ifndef GCECONFIGURATIONXMLPARSER2_H
#define GCECONFIGURATIONXMLPARSER2_H

// Xinx header
#include "configurationfile.h"
#include "gceconfigurationdefparser.h"

// Libxml
#include <libxml/xmlreader.h>

class GceConfigurationXmlParser2
{
public:
	GceConfigurationXmlParser2();
	~GceConfigurationXmlParser2();

	bool loadFromFile(const QString & filename, const QString & module);

	/* INPUT */
	QSharedPointer<GceConfiguration> _gce_configuration;
	int m_configurationNumber;
	bool m_quick;

	/* OUTPUT */
	QString m_version, m_rootPath;
	int m_edition;
	QMultiHash<QString,BusinessViewInformation> m_fileRefToInformation;
private:
	void readUnknownElement(xmlTextReader * reader);
	void readConfigElement(xmlTextReader * reader);
	void readVersionElement(xmlTextReader * reader);
	void readApplicationElement(xmlTextReader * reader);

	void readBusinessViewDef(xmlTextReader * reader);
	void readBusinessViewDefElement(xmlTextReader * reader);
	void readPresentationElement(xmlTextReader * reader, const BusinessViewInformation & information);

	void readPresentation(xmlTextReader * reader);
	void readPresentationElement(xmlTextReader * reader);

	QString m_configurationFileName;
	QString _module;

	QHash<QString,BusinessViewInformation> m_nameToInformation;
	QMultiHash<QString,QString> m_fileRefToName;
};


#endif // GCECONFIGURATIONXMLPARSER2_H
