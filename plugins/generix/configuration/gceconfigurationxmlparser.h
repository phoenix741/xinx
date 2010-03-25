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
#pragma once
#ifndef GCECONFIGURATIONXMLPARSER_H
#define GCECONFIGURATIONXMLPARSER_H

// Xinx header
#include "configurationfile.h"

// Qt header
#include <QXmlStreamReader>
#include <QIODevice>
#include <QDebug>
#include <QDomDocument>
#include <QFileInfo>
#include <QDir>
#include <QFile>

class GceConfigurationDef;

class GceConfigurationXmlParser : public QXmlStreamReader
{
public:
	GceConfigurationXmlParser();

	bool loadFromFile(const QString & filename);

	/* INPUT */
	GceConfigurationDef * m_parent;
	int m_configurationNumber;

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
	void readPresentationElement(const BusinessViewInformation & information);

	void readPresentation();
	void readPresentationElement();

	QString m_configurationFileName;

	QHash<QString,BusinessViewInformation> m_nameToInformation;
	QMultiHash<QString,QString> m_fileRefToName;
};

#endif // GCECONFIGURATIONXMLPARSER_H
