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

#ifndef _ENTRIES_H_
#define _ENTRIES_H_
#pragma once

// Xinx header
#include <versioncontrol/rcs.h>

// Qt header
#include <QString>
#include <QDateTime>
#include <QHash>

struct EntriesLine
{
	EntriesLine();
	EntriesLine(const QString & line);

	RCS::rcsState status(const QString & path) const;

	bool hasConflict;
	QChar type; // D pour dossier
	QString filename;
	QString version;
	QDateTime date;
};

struct EntriesFile : public QHash<QString,EntriesLine>
{
	EntriesFile();
	EntriesFile(const QString & file);

	const EntriesLine status(const QString & path) const;

	QString path;
	QDateTime fileDate;
};

struct EntriesList : public QHash<QString,EntriesFile>
{
	const EntriesFile value(const QString & key);
	const EntriesFile value(const QString & key, const EntriesFile & defaultValue);

	const EntriesLine status(const QString & filename);
};

#endif // _ENTRIES_H_
