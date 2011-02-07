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
#include "configurationfile.h"

// Qt header
#include <QDir>
#include <QFile>

#include <QtXmlPatterns>

#include <QDomDocument>
#include <QDomElement>

// TODO: Delete this workaround
#undef major
#undef minor

/* ConfigurationVerstionIncorectException */

ConfigurationVersionIncorectException::ConfigurationVersionIncorectException(QString version) : XinxException(QString("Wrong version number %1").arg(version))
{

}

/* MetaConfigurationException */

MetaConfigurationException::MetaConfigurationException(QString message) : XinxException(message)
{

}

/* MetaConfigurationNotExistException */

MetaConfigurationNotExistException::MetaConfigurationNotExistException(QString filename) : MetaConfigurationException(QString("Meta configuration \"%1\" not found").arg(filename))
{

}

/* ConfigurationVersion */

ConfigurationVersion::ConfigurationVersion(int major, int minor, int release, int build)
{
	m_major = major;
	m_minor = minor;
	m_release = release;
	m_build = build;
}

ConfigurationVersion::ConfigurationVersion(const QString & version, int build)
{
	QString major, minor, release;

	m_build = build;
	if (! version.length())
		throw ConfigurationVersionIncorectException(version);
	if (version.at(0) != 'V')
		throw ConfigurationVersionIncorectException(version);
	int posDot = version.indexOf("."),
	             posLine = version.indexOf("-", posDot);
	if ((posDot < 0) || (posLine < 0))
		throw ConfigurationVersionIncorectException(version);
	major = version.mid(1, posDot - 1);
	minor = version.mid(posDot + 1, posLine - posDot - 1);
	release = version.mid(posLine + 1);
	m_major = major.toInt();
	m_minor = minor.toInt();
	m_release = release.toInt();
}

ConfigurationVersion::~ConfigurationVersion()
{
}

int ConfigurationVersion::major() const
{
	return m_major;
}

int ConfigurationVersion::minor() const
{
	return m_minor;
}

int ConfigurationVersion::release() const
{
	return m_release;
}

int ConfigurationVersion::build() const
{
	return m_build;
}

QString ConfigurationVersion::toString() const
{
	QString version = QString("V%1.%2-%3").arg(m_major, 2, 10, QLatin1Char('0'))
	                  .arg(m_minor, 2, 10, QLatin1Char('0'))
	                  .arg(m_release, 2, 10, QLatin1Char('0'));
	if (m_build > 0)
		version += QString(" (%1)")     .arg(m_build, 2, 10, QLatin1Char('0'));

	return version;
}

bool ConfigurationVersion::isValid() const
{
	return (m_major >= 0) && (m_minor >= 0) && (m_release >= 0);
}

bool ConfigurationVersion::operator!= (ConfigurationVersion version) const
{
	return !(*this == version);
}

bool ConfigurationVersion::operator== (ConfigurationVersion version) const
{
	if (m_major == version.m_major)
	{
		if (m_minor == version.m_minor)
		{
			if (m_release == version.m_release)
			{
				if (m_build == version.m_build)
				{
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool ConfigurationVersion::operator> (ConfigurationVersion version) const
{
	if (m_major == version.m_major)
	{
		if (m_minor == version.m_minor)
		{
			if (m_release == version.m_release)
			{
				if (m_build == version.m_build)
				{
					return false;
				}
				else
					return m_build > version.m_build;
			}
			else
				return m_release > version.m_release;
		}
		else
			return m_minor > version.m_minor;
	}
	else
		return m_major > version.m_major;
}

bool ConfigurationVersion::operator>= (ConfigurationVersion version) const
{
	return (*this > version) || (*this == version);
}

bool ConfigurationVersion::operator< (ConfigurationVersion version) const
{
	return !(*this >= version);
}

bool ConfigurationVersion::operator<= (ConfigurationVersion version) const
{
	return !(*this > version);
}

/* BusinessViewInformation */

BusinessViewInformation::BusinessViewInformation()
{
}

const QString & BusinessViewInformation::configurationFileName() const
{
	return m_configurationFilename;
}

void BusinessViewInformation::setConfigurationFileName(const QString & value)
{
	m_configurationFilename = value;
}

int BusinessViewInformation::configurationNumber() const
{
	return m_configurationNumber;
}

void BusinessViewInformation::setConfigurationNumber(int value)
{
	m_configurationNumber = value;
}

const QString & BusinessViewInformation::businesViewName() const
{
	return m_businessViewName;
}

void BusinessViewInformation::setBusinessViewName(const QString & value)
{
	m_businessViewName = value;
}

const QString & BusinessViewInformation::targetName() const
{
	return m_targetName;
}

void BusinessViewInformation::setTargetName(const QString & value)
{
	m_targetName = value;
}

const QString & BusinessViewInformation::viewstructName() const
{
	return m_viewStructName;
}

void BusinessViewInformation::setViewstructName(const QString & value)
{
	m_viewStructName = value;
}
