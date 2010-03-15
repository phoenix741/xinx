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
#include "entries.h"

// Qt header
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

/* EntriesLine */

EntriesLine::EntriesLine() : hasConflict(false)
{

}

EntriesLine::EntriesLine(const QString & line)
{
	QStringList entry = line.split('/');
	if ((entry.size() >= 1) && (entry.at(0).size() >= 1)) type = entry.at(0).at(0);
	if (entry.size() >= 2) filename = entry.at(1);
	if (type != 'D')
	{
		if (entry.size() >= 3) version = entry.at(2);
		if ((entry.size() >= 4) && (! entry.at(3).trimmed().isEmpty()) && (entry.at(3) != "dummy timestamp") && (entry.at(3) != "Result of merge"))
		{
			if (entry.at(3).contains('+'))
			{
				date = QDateTime::fromString(entry.at(3).section('+', -1, -1).simplified());      // , "ddd MMM d hh:mm:ss yyyy"
				hasConflict = true;
			}
			else
			{
				date = QDateTime::fromString(entry.at(3).simplified());    // , "ddd MMM d hh:mm:ss yyyy"
				hasConflict = false;
			}
			date.setTimeSpec(Qt::UTC);
		}
	}
}

RCS::rcsState EntriesLine::status(const QString & path) const
{
	QFileInfo info(QDir(path).absoluteFilePath(filename));
	QDateTime fileDate = info.lastModified().toUTC();

	/* Clear ms */
	QTime modifiedTime = fileDate.time();
	modifiedTime = QTime(modifiedTime.hour(), modifiedTime.minute(), modifiedTime.second());
	fileDate.setTime(modifiedTime);

	if (! info.exists())
	{
		if ((version.size() > 0) && (version.at(0) == '-'))
			return RCS::LocallyRemoved;
		return RCS::NeedsCheckout;
	}

	if (version == "0")
		return RCS::LocallyAdded;

	if (info.isDir())
		return RCS::Updated;

	if (hasConflict)
		return RCS::UnresolvedConflict;

	if (date.isNull() || (date != fileDate))
		return RCS::LocallyModified;

	return RCS::Updated;
}

/* EntriesFile */

EntriesFile::EntriesFile()
{

}

EntriesFile::EntriesFile(const QString & file)
{
	QDir pathDir = QFileInfo(file).absoluteDir();
	pathDir.cdUp();
	path = pathDir.path();
	fileDate = QFileInfo(file).lastModified();

	QFile entriesFile(file);
	if (entriesFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream entriesTextStream(&entriesFile);
		QString entryLine;

		while (!(entryLine = entriesTextStream.readLine()).isNull())
		{
			EntriesLine e(entryLine);
			insert(e.filename, e);
		}
	}
}

const EntriesLine EntriesFile::status(const QString & path) const
{
	QString filename = QFileInfo(path).fileName();
	return value(filename);
}

/* EntriesList */

const EntriesFile EntriesList::value(const QString & key)
{
	return value(key, EntriesFile());
}

const EntriesFile EntriesList::value(const QString & key, const EntriesFile & defaultValue)
{
	const EntriesFile & entries = QHash<QString,EntriesFile>::value(key, defaultValue);
	if (entries.fileDate == QFileInfo(key).lastModified())
		return entries;
	else
	{
		(*this)[ key ] = EntriesFile(key);
		return QHash<QString,EntriesFile>::value(key, defaultValue);
	}
}

const EntriesLine EntriesList::status(const QString & filename)
{
	Q_ASSERT(QFileInfo(filename).isAbsolute());

	QString path = QFileInfo(filename).absolutePath();
	QString cvsEntries = QDir(path).absoluteFilePath("CVS/Entries");
	EntriesFile file;
	if (contains(cvsEntries))
	{
		file = value(cvsEntries);
	}
	else
	{
		file = EntriesFile(cvsEntries);
		insert(cvsEntries, file);
	}
	return file.status(filename);
}
