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

#ifndef GCECONFIGURATION_H
#define GCECONFIGURATION_H

// Xinx header
#include "gceinterface.h"

// Qt header
#include <QString>
#include <QMultiHash>
#include <QCoreApplication>

class GceConfiguration : public GceInterface {
	Q_DECLARE_TR_FUNCTIONS(GceConfiguration)
public:
	GceConfiguration( const QString & filename );
	virtual ~GceConfiguration();

	virtual QString rootFilename();
	virtual QStringList filenames();
	virtual ConfigurationVersion version();
	virtual QStringList dictionnaries();
	virtual QList<BusinessViewInformation> businessView( const QString & filename );
	virtual QList<BusinessViewInformation> businessViews();
	virtual QString resolveFileName( const QString & filename );
private:
	QString m_configurationFileName;
	ConfigurationVersion m_version;
	QMultiHash<QString,BusinessViewInformation> m_fileToInformation;
};

#endif // GCECONFIGURATION_H