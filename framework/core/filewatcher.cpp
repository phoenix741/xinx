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

// Qt header
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QDebug>

// xinx header
#include "core/filewatcher.h"
#include "core/p_filewatcher.h"

/* Variables */

FileWatcherManager * fileWatcherManager = NULL;

/* FileWatched */

void FileWatched::initializeDate()
{
	m_date = QFileInfo(m_name).lastModified();
}

bool FileWatched::isFileChanged()
{
	QDateTime date = QFileInfo(m_name).lastModified();
	if (date == m_date)
		return false;
	m_date = date;
	return true;
}

/* FileWatcherManager */

FileWatcherManager::FileWatcherManager()
{
}

FileWatcherManager::~FileWatcherManager()
{
	for (int i = 0 ; i < m_watchedfiles.count() ; i++)
	{
		delete m_watchedfiles.at(i);
	}
	m_watchedfiles.clear();
}

void FileWatcherManager::watch()
{
	// File to watch
	m_watchedFilesMutex.lock();
	for (int i = 0 ; i < m_watchedfiles.count() ; i++)
	{
		if (m_watchedfiles.at(i)->isFileChanged())
		{
			emit fileChanged(m_watchedfiles.at(i)->name());
		}
	}
	m_watchedFilesMutex.unlock();
}

void FileWatcherManager::threadrun()
{
	QTimer timer;
	connect(&timer, SIGNAL(timeout()), this, SLOT(watch()));
	timer.start(1000);
	exec();
}

int FileWatcherManager::indexOfWatchedFile(const QString & filename)
{
	for (int i = 0 ; i < m_watchedfiles.count() ; i++)
	{
		if (m_watchedfiles.at(i)->name() == filename)
			return i;
	}
	return -1;
}

void FileWatcherManager::addFile(const QString & filename)
{
	m_watchedFilesMutex.lock();
	int index;
	if ((index = indexOfWatchedFile(filename)) == -1)
	{
		FileWatched * file = new FileWatched(filename);
		m_watchedfiles.append(file);
	}
	else
	{
		m_watchedfiles.at(index)->ref();
		qDebug() << QString("Duplicate entry for file %1").arg(filename);
	}
	m_watchedFilesMutex.unlock();
	if ((m_watchedfiles.count() > 0) && !isRunning())
		start();
}

void FileWatcherManager::removeFile(const QString & filename)
{
	m_watchedFilesMutex.lock();
	int index = indexOfWatchedFile(filename);
	if (m_watchedfiles.at(index)->deref() == 0)
	{
		m_watchedfiles.removeAt(index);
	}
	m_watchedFilesMutex.unlock();
}

FileWatcherManager * FileWatcherManager::instance()
{
	if (! fileWatcherManager)
		fileWatcherManager = new FileWatcherManager();
	return fileWatcherManager;
}

void FileWatcherManager::deleteIfPossible()
{
	if (fileWatcherManager && (fileWatcherManager->m_watchedfiles.count() == 0))
	{
		FileWatcherManager * manager = fileWatcherManager;
		fileWatcherManager = NULL;
		if (manager->isRunning())
		{
			manager->quit();
			manager->wait();
		}
		delete manager;
	}
}

FileWatched * FileWatcherManager::watchedFileAt(int index)
{
	return m_watchedfiles.at(index);
}


/* PrivateFileWatcher */

PrivateWatcher::PrivateWatcher(FileWatcher * parent) : m_isActivated(true), m_parent(parent)
{
}

PrivateWatcher::~PrivateWatcher()
{
	FileWatcherManager::instance()->removeFile(m_filename);
	FileWatcherManager::deleteIfPossible();
}

void PrivateWatcher::fileChanged(QString filename)
{
	if (m_isActivated && (m_filename == filename))
		emit m_parent->fileChanged();
}

/* FileWatcher */

FileWatcher::FileWatcher(const QString & filename)
{
	d = new PrivateWatcher(this);
	d->m_filename = filename;

	connect(FileWatcherManager::instance(), SIGNAL(fileChanged(QString)), d, SLOT(fileChanged(QString)), Qt::QueuedConnection);
	FileWatcherManager::instance()->addFile(filename);
}

FileWatcher::~FileWatcher()
{
	delete d;
}

void FileWatcher::desactivate()
{
	d->m_isActivated = false;
}

void FileWatcher::activate()
{
	int index = FileWatcherManager::instance()->indexOfWatchedFile(d->m_filename);
	FileWatcherManager::instance()->watchedFileAt(index)->initializeDate();
	d->m_isActivated = true;
}


