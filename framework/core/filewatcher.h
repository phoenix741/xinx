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

#ifndef _FILEWATCHER_H_
#define _FILEWATCHER_H_
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <qobject.h>

class PrivateFileWatcher;
class PrivateFilesWatcher;

class LIBEXPORT FileWatcher : public QObject
{
	Q_OBJECT
public:
	explicit FileWatcher(const QString & filename);
	virtual ~FileWatcher();

	void desactivate();
	void activate();
signals:
	void fileChanged();
protected:
	PrivateFileWatcher * d;
	friend class PrivateFileWatcher;
};

class LIBEXPORT FilesWatcher : public QObject
{
	Q_OBJECT
public:
	explicit FilesWatcher(QObject * parent = 0);
	virtual ~FilesWatcher();

	void addPath(const QString & path);
	void removePath(const QString & path);
	void removePaths(const QStringList & paths);
	const QStringList & files() const;
signals:
	void fileChanged(const QString & path);
private:
	PrivateFilesWatcher * d;
	friend class PrivateFilesWatcher;
};

#endif // _FILEWATCHER_H_
