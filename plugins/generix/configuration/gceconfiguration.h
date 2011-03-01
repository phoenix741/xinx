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

#ifndef GCEINTERFACE_H
#define GCEINTERFACE_H

// Xinx header
#include "configuration/configurationfile.h"
#include <core/exceptions.h>
#include <QObject>

class GceConfigurationException : public XinxException
{
public:
	GceConfigurationException(const QString & exception) : XinxException(exception) {}
};

class GceConfiguration
{
public:
	GceConfiguration();
	virtual ~GceConfiguration();

	void setFilename(const QString & filename);
	const QString & filename() const ;

	const QStringList & filenames() const;
	void addFilename(const QString & filename);
	void clearFilenames();

	const ConfigurationVersion & version() const;
	void setVersion(const ConfigurationVersion & version);

	const QStringList & dictionnaries();
	void addDictionnary(const QString & dictionnary);
	void clearDictionaries();

	QList<BusinessViewInformation> businessView(const QString & filename) const;
	QList<BusinessViewInformation> businessViews() const;
	void addBusinessView(const QMultiHash<QString,BusinessViewInformation> & businessview);
	void addBusinessView(const QString & path, const BusinessViewInformation & information);
	void clearBusinessView();

	const QHash<QString,QStringList> & aliasPolicy() const;
	void addAliasPolicy(const QString & alias, const QString & value);
	void clearAliasPolicy();

	QString resolveFileName(const QString & filename) const;
	QStringList resolvePath(const QString & filename) const;
private:
	QStringList generateFileName(const QString & filename) const;

	QString _filename;
	QStringList _filenames;
	ConfigurationVersion _version;
	QStringList _dictionnaries;
	QMultiHash<QString,BusinessViewInformation> _informations;
	QHash<QString,QStringList> _alias_policies;
};

#endif // GCEINTERFACE_H
