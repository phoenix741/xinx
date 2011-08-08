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

#ifndef VERSION_H
#define VERSION_H

#include <QString>
#include <QSharedDataPointer>

class VersionData;

class Version
{
public:
	enum VersionNumber
	{
		MAJOR_NUMBER = 0x1,
		MINOR_NUMBER = 0x2,
		MICRO_NUMBER = 0x4,
		BUILD_NUMBER = 0x8,
		ALL_NUMBER   = 0xF
	};
	Q_DECLARE_FLAGS(VersionNumberFlags, VersionNumber)

	Version();
	Version(int major, int minor, int micro, int build);
	Version(const Version & version);
	Version(const QString & version, const QString & separator = QString(".-,"));
	~Version();

	bool isValid() const;

	static Version fromString(const QString & version, const QString & separator = QString(".-,"));
	QString toString(const Version::VersionNumberFlags & flags = Version::ALL_NUMBER, const QLatin1Char & separator = QLatin1Char('.'));

	Version & operator=(const QString & version);
	Version & operator=(const Version & version);

	bool operator==(const Version & version) const;
	bool operator!=(const Version & version) const;

	bool operator<(const Version & version) const;
	bool operator<=(const Version & version) const;
	bool operator>(const Version & version) const;
	bool operator>=(const Version & version) const;
private:
	QSharedDataPointer<VersionData> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Version::VersionNumberFlags)

#endif // VERSION_H
