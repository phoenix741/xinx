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

#ifndef VERSIONLABEL_H
#define VERSIONLABEL_H

#include <QLabel>
#include <application/version.h>

class VersionLabelPrivate;

class VersionLabel : public QLabel
{
	Q_OBJECT
public:
    VersionLabel(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~VersionLabel();

	const Version & currentVersion() const;
	const Version & stableVersion() const;
	const Version & unstableVersion() const;
	
private:
	QScopedPointer<VersionLabelPrivate> d;
};

#endif // VERSIONLABEL_H
