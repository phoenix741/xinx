/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "version_p.h"
#include <QStringList>

/* VersionData */

void VersionData::updateFromString(const QString & version, const QLatin1Char & separator)
{
	int indice = 1;
	QStringList versionNumberStr = version.split(separator);
	foreach(const QString & numberStr, versionNumberStr)
	{
		int number = numberStr.toInt();
		switch(indice)
		{
		case 1:
			_major = number;
			break;
		case 2:
			_minor = number;
			break;
		case 3:
			_micro = number;
			break;
		case 4:
			_build = number;
			break;
		};
		indice++;
	}
}


/* Version */

Version::Version() : d(new VersionData)
{

}

Version::Version(int major, int minor, int micro, int build) : d(new VersionData)
{
	d->_major = major;
	d->_minor = minor;
	d->_micro = micro;
	d->_build = build;
}

Version::Version(const Version& version) : d(version.d)
{

}

Version::Version(const QString& version, const QLatin1Char& separator)
{
	d->updateFromString(version, separator);
}

Version::~Version()
{

}

bool Version::isValid() const
{
	return Version().operator!=(*this);
}

Version Version::fromString(const QString& version, const QLatin1Char& separator)
{
	return Version(version, separator);
}

QString Version::toString(const Version::VersionNumberFlags& flags, const QLatin1Char& separator)
{
	QStringList versionStr;
	if (flags.testFlag(Version::MAJOR_NUMBER))
	{
		versionStr << QString::number(d->_major);
	}
	if (flags.testFlag(Version::MINOR_NUMBER))
	{
		versionStr << QString::number(d->_minor);
	}
	if (flags.testFlag(Version::MICRO_NUMBER))
	{
		versionStr << QString::number(d->_micro);
	}
	if (flags.testFlag(Version::BUILD_NUMBER))
	{
		versionStr << QString::number(d->_build);
	}
	
	return versionStr.join(QString(separator));
}

Version& Version::operator=(const QString& version)
{
	d->updateFromString(version);
	return *this;
}

bool Version::operator!=(const Version& version) const
{
	return (d->_major != version.d->_major) && (d->_minor != version.d->_minor) && (d->_micro != version.d->_micro) && (d->_build != version.d->_build);
}

bool Version::operator==(const Version& version) const
{
	return (d->_major == version.d->_major) && (d->_minor == version.d->_minor) && (d->_micro == version.d->_micro) && (d->_build == version.d->_build);
}

bool Version::operator<(const Version& version) const
{
	if (d->_major < version.d->_major)
	{
		return true;
	}
	else if (d->_major == version.d->_major)
	{
		if (d->_minor < version.d->_minor)
		{
			return true;
		}
		else if (d->_minor == version.d->_minor)
		{
			if (d->_micro < version.d->_micro)
			{
				return true;
			}
			else if ((d->_micro == version.d->_micro) && (d->_build < version.d->_build))
			{
				return true;
			}
		}
	}
	return false;
}

bool Version::operator<=(const Version& version) const
{
	return operator<(version) || operator==(version);
}

bool Version::operator>(const Version& version) const
{
	if (d->_major > version.d->_major)
	{
		return true;
	}
	else if (d->_major == version.d->_major)
	{
		if (d->_minor > version.d->_minor)
		{
			return true;
		}
		else if (d->_minor == version.d->_minor)
		{
			if (d->_micro > version.d->_micro)
			{
				return true;
			}
			else if ((d->_micro == version.d->_micro) && (d->_build > version.d->_build))
			{
				return true;
			}
		}
	}
	return false;
}

bool Version::operator>=(const Version& version) const
{
	return operator>(version) || operator==(version);
}

