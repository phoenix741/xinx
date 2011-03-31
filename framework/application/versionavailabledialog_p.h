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

#ifndef VERSIONLABEL_P_H
#define VERSIONLABEL_P_H

#include "versionavailabledialog.h"
#include <QNetworkReply>

//! \private
class VersionAvailableDialogPrivate : public QObject
{
	Q_OBJECT
public:
	VersionAvailableDialog * _dialog;
	QString _homepage, _stable_changelog, _unstable_changelog;
	Version _current, _stable, _unstable, _update_version;
	QStringList _changelog;

	QNetworkAccessManager * _manager;
    int _width;

	void updateText();
public slots:
	void releaseFinished();
	void changelogFinished();
};

#endif // VERSIONLABEL_P_H
