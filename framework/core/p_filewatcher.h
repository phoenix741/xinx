/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke   *
 *   ulrich.vdh@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _P_FILEWATCHER_H_
#define _P_FILEWATCHER_H_
#pragma once

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

class FileWatched : public QObject {
	Q_OBJECT
public:
	FileWatched() {}
	FileWatched( const FileWatched & o ) : QObject(), m_name( o.m_name ), m_date( o.m_date ) {}
	FileWatched( const QString & name ) : m_name( name ) { initializeDate(); }
	virtual ~FileWatched() {}

	const QString & name() const { return m_name; }
	bool isFileChanged();
	void initializeDate();

	inline int ref() { return ++count; }
	inline int deref() { return --count; }
private:
	int count;
	QString m_name;
	QDateTime m_date;
};

class FileWatcherManager : public XinxThread {
	Q_OBJECT
public:
	FileWatcherManager();
	virtual ~FileWatcherManager();

	virtual void threadrun();

	void addFile( const QString & filename );
	void removeFile( const QString & filename );

	int indexOfWatchedFile( const QString & filename );
	FileWatched * watchedFileAt( int index );

	static FileWatcherManager * instance();
	static void deleteIfPossible();
public slots:
	void watch();
signals:
	void fileChanged( QString filename );
	void directoryChanged( QString directory );
private:
	QMutex m_watchedFilesMutex;
	QList<FileWatched*> m_watchedfiles;
};

class PrivateWatcher : public QObject {
	Q_OBJECT
public:
	PrivateWatcher( FileWatcher * parent );
	~PrivateWatcher();

	QString m_filename;
	bool m_isActivated;
public slots:
	void fileChanged( QString filename );
private:
	FileWatcher * m_parent;
};

extern FileWatcherManager * fileWatcherManager;

#endif // _P_FILEWATCHER_H_
