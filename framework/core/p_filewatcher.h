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

#pragma once
#ifndef _P_FILEWATCHER_H_
#define _P_FILEWATCHER_H_

// kmdalert watcher
#include <core/filewatcher.h>
#include <core/xinxthread.h>

// Qt header
#include <QString>
#include <QStringList>
#include <QThread>
#include <QObject>
#include <QMutex>
#include <QList>
#include <QDateTime>
#include <QTimer>

class FileWatched : public QObject
{
	Q_OBJECT
public:
	FileWatched() {}
	FileWatched(const FileWatched & o) : QObject(), m_name(o.m_name), m_date(o.m_date) {}
	explicit FileWatched(const QString & name) : m_name(name)
	{
		initializeDate();
	}
	virtual ~FileWatched() {}

	const QString & name() const
	{
		return m_name;
	}
	bool isFileChanged();
	void initializeDate();

	inline int ref()
	{
		return ++count;
	}
	inline int deref()
	{
		return --count;
	}
private:
	int count;
	QString m_name;
	QDateTime m_date;
};

class FileWatcherManager : public XinxThread
{
	Q_OBJECT
public:
	FileWatcherManager();
	virtual ~FileWatcherManager();

	virtual void threadrun();

	void addFile(const QString & filename);
	void removeFile(const QString & filename);

	int indexOfWatchedFile(const QString & filename);
	FileWatched * watchedFileAt(int index);

	static FileWatcherManager * instance();
	static void deleteIfPossible();
public slots:
	void watch();
signals:
	void fileChanged(QString filename);
	void directoryChanged(QString directory);
private:
	QMutex m_watchedFilesMutex;
	QList<FileWatched*> m_watchedfiles;
};

class PrivateFileWatcher : public QObject
{
	Q_OBJECT
public:
	PrivateFileWatcher(FileWatcher * parent);
	~PrivateFileWatcher();

	QString m_filename;
	bool m_isActivated;
public slots:
	void fileChanged(QString filename);
private:
	FileWatcher * m_parent;
};

class PrivateFilesWatcher : public QObject
{
	Q_OBJECT
public:
    PrivateFilesWatcher(FilesWatcher * parent);
    ~PrivateFilesWatcher();

	void removePaths(const QStringList & paths);

	QStringList _filenames;
public slots:
	void fileChanged(QString filename);
private:
	FilesWatcher * _parent;
};

extern FileWatcherManager * fileWatcherManager;

#endif // _P_FILEWATCHER_H_
