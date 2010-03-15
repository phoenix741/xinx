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

#ifndef GCEPROPERTIES_H
#define GCEPROPERTIES_H

// Xinx header
#include "gceinterface.h"
#include "gceconfigurationdef.h"

// Qt header
#include <QString>
#include <QMultiHash>
#include <QCoreApplication>

class GceProperties : public GceConfigurationDef
{
	Q_DECLARE_TR_FUNCTIONS(GceProperties)
public:
	GceProperties(const QString & filename);
	virtual ~GceProperties();

	QStringList generateFileName(const QString & filename);
	virtual QString rootFilename();
	virtual QStringList filenames();
	virtual QString resolveFileName(const QString & filename);
protected:
	virtual void readGceProperties(const QString & propertiesFileName);
	virtual void readConfigurationFile(int configurationIndex, const QString & configurationFileName);

	QStringList m_dictionnaries, m_filenames;
	QString m_propertiesFilename, m_configurationDef;

	QHash<QString,QStringList> m_policy;

	friend class GceConfiguration150Parser;
};

#endif // GCEPROPERTIES_H
