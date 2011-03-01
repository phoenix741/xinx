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

#include "gceconfiguration.h"
#include <QStack>
#include <QFile>

GceConfiguration::GceConfiguration()
{
}

GceConfiguration::~GceConfiguration()
{
}

void GceConfiguration::setFilename(const QString& filename)
{
	_filename = filename;
}

const QString& GceConfiguration::filename() const
{
	return _filename;
}

void GceConfiguration::addFilename(const QString& filename)
{
	_filenames.append(filename);
}

const QStringList& GceConfiguration::filenames() const
{
	return _filenames;
}

void GceConfiguration::clearFilenames()
{
	_filenames.clear();
}

void GceConfiguration::setVersion(const ConfigurationVersion& version)
{
	_version = version;
}

const ConfigurationVersion& GceConfiguration::version() const
{
	return _version;
}

const QStringList& GceConfiguration::dictionnaries()
{
	return _dictionnaries;
}

void GceConfiguration::addDictionnary(const QString& dictionnary)
{
	_dictionnaries.append(dictionnary);
}

void GceConfiguration::clearDictionaries()
{
	_dictionnaries.clear();
}

void GceConfiguration::addBusinessView(const QString& path, const BusinessViewInformation& information)
{
	_informations.insert(path, information);
}

QList< BusinessViewInformation > GceConfiguration::businessView(const QString& filename) const
{
	return _informations.values(filename);
}

QList< BusinessViewInformation > GceConfiguration::businessViews() const
{
	return _informations.values();
}

void GceConfiguration::clearBusinessView()
{
	_informations.clear();
}

void GceConfiguration::addBusinessView(const QMultiHash< QString, BusinessViewInformation >& businessview)
{
	_informations += businessview;
}

void GceConfiguration::addAliasPolicy(const QString& alias, const QString& value)
{
	_alias_policies[alias].append(value);
}

const QHash< QString, QStringList >& GceConfiguration::aliasPolicy() const
{
	return _alias_policies;
}

void GceConfiguration::clearAliasPolicy()
{
	_alias_policies.clear();
}

QStringList GceConfiguration::generateFileName(const QString& filename) const
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
			QStringList values = _alias_policies.value(key);

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

QString GceConfiguration::resolveFileName(const QString& filename) const
{
	if (_alias_policies.size())
	{
		QStringList names = generateFileName(filename);
		foreach(QString name, names)
		{
			if (QFile::exists(name))
				return name;
		}
	}
	return filename;
}

QStringList GceConfiguration::resolvePath(const QString& filename) const
{
	if (_alias_policies.size())
	{
		return generateFileName(filename);
	}
	return QStringList();
}

