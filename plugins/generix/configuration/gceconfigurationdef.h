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

#ifndef GCECONFIGURATIONDEF_H
#define GCECONFIGURATIONDEF_H

// Xinx header
#include "gceinterface.h"

// Qt header
#include <QString>
#include <QMultiHash>
#include <QCoreApplication>

class GceConfigurationXmlParser;
class GceConfigurationXmlParser2;

class GceConfigurationDef : public GceInterface
{
	Q_DECLARE_TR_FUNCTIONS(GceConfigurationDef)
public:
	GceConfigurationDef();
	GceConfigurationDef(const QString & filename);
	virtual ~GceConfigurationDef();

	virtual QString rootFilename();
	virtual QStringList filenames();
	virtual ConfigurationVersion version();
	virtual QStringList dictionnaries();
	virtual QList<BusinessViewInformation> businessView(const QString & filename);
	virtual QList<BusinessViewInformation> businessViews();
	virtual QString resolveFileName(const QString & filename);
protected:
	virtual void readConfigurationDef(const QString & configurationdefFileName);
	virtual void readConfigurationFile(int configurationIndex, const QString & configurationFileName);

	QStringList m_dictionnaries, m_filenames;
	QString m_configurationFileName, m_directoryPath;
	ConfigurationVersion m_version;
	QMultiHash<QString,BusinessViewInformation> m_fileToInformation;

	friend class GceConfigurationXmlParser;
	friend class GceConfigurationXmlParser2;
};

#endif // GCECONFIGURATIONDEF_H
