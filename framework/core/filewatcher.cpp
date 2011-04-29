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

// Qt header
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QDebug>

// xinx header
#include "core/filewatcher.h"
#include "core/p_filewatcher.h"

/*! \cond private */

/* Variables */

FileWatcherManager * FileWatcherManager::s_self = NULL;

/* FileWatched */

bool FileWatched::isFileChanged()
{
	m_file.refresh();
	QDateTime date = m_file.lastModified();
	if (date == m_date)
		return false;
	m_date = date;
	return true;
}

/* FileWatcherManager */

FileWatcherManager::FileWatcherManager()
{
	XINXStaticDeleter::self()->addObject(this);
}

FileWatcherManager::~FileWatcherManager()
{
	if (s_self == this)
	{
		s_self = 0;
	}

	if (isRunning())
	{
		quit();
		wait();
	}

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
	qDebug() << "Add file " << filename << " to the watcher";
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
	qDebug() << "Remove file " << filename << " to the watcher";
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
	if (! s_self)
		s_self = new FileWatcherManager();
	return s_self;
}

FileWatched * FileWatcherManager::watchedFileAt(int index)
{
	return m_watchedfiles.at(index);
}

/* PrivateFileWatcher */

PrivateFileWatcher::PrivateFileWatcher(FileWatcher * parent) : m_activatedDate(QDateTime::currentDateTime()), m_isActivated(true), m_parent(parent)
{
}

PrivateFileWatcher::~PrivateFileWatcher()
{
	FileWatcherManager::instance()->removeFile(m_filename);
}

void PrivateFileWatcher::fileChanged(QString filename)
{
	int index = FileWatcherManager::instance()->indexOfWatchedFile(filename);

	if (m_isActivated && (m_activatedDate < FileWatcherManager::instance()->watchedFileAt(index)->date()) && (m_filename == filename))
		emit m_parent->fileChanged();
}

/* PrivateFilesWatcher */

PrivateFilesWatcher::PrivateFilesWatcher(FilesWatcher * parent) : _parent(parent)
{
}

PrivateFilesWatcher::~PrivateFilesWatcher()
{
	removePaths(_filenames);
}

void PrivateFilesWatcher::fileChanged(QString filename)
{
	if (_filenames.contains(filename))
		emit _parent->fileChanged(filename);
}

void PrivateFilesWatcher::removePaths(const QStringList & paths)
{
	foreach(QString filename, paths)
	{
		_filenames.removeAll(filename);
		FileWatcherManager::instance()->removeFile(filename);
	}
}


/*! \endcond */

/* FileWatcher */

/*!
 * \class FileWatcher
 * \brief Class used to watch the modification of a file.
 *
 * The file is periodically watched and if the date of the file is modified, the user is alerted by a signal.
 *
 * Behind the FileWatcher, a thread watch modifications of all files referenced by a
 * FileWatcher. The watcher can be desactivate and reactivate without delete the object.
 * When the object is reactivated, the file date, in memory, is updated.
 */

/*!
 * \fn void FileWatcher::fileChanged()
 * \brief The signal is emited when the watched file is modified.
 */

/*!
 * Create a FileWatcher with a file name
 * \param filename The file name to watch.
 */
FileWatcher::FileWatcher(const QString & filename) : d(new PrivateFileWatcher(this))
{
	d->m_filename = filename;

	connect(FileWatcherManager::instance(), SIGNAL(fileChanged(QString)), d.data(), SLOT(fileChanged(QString)), Qt::QueuedConnection);
	FileWatcherManager::instance()->addFile(filename);
}

/*! Destroy the FileWatcher */
FileWatcher::~FileWatcher()
{

}

/*!
 * Desactivate the watcher. You must called the \e activate method to reactivate
 * the watcher.
 */
void FileWatcher::desactivate()
{
	d->m_isActivated = false;
}

/*!
 * Activate the watcher. The watcher is created activated. Use \e desactivate to
 * desactivate.
 */
void FileWatcher::activate()
{
	d->m_activatedDate = QDateTime::currentDateTime();
	d->m_isActivated = true;
}


/*!
 * \class FilesWatcher
 * \brief Class used to watch the modification of multiple file.
 *
 * All files is periodically watched and if the date of the file is modified, the user is alerted by a signal.
 *
 * Behind the FileWatcher, a thread watch modifications of all files referenced by a
 * FileWatcher. The watcher can be desactivate and reactivate without delete the object.
 * When the object is reactivated, the file date, in memory, is updated.
 */

/*!
 * \fn void FilesWatcher::fileChanged(const QString & path)
 * \brief The signal is emited when the watched file is modified.
 * \param path The file that have been changed.
 */

/*!
 * Create a FilesWatcher with a file name
 */
FilesWatcher::FilesWatcher(QObject* parent) : QObject(parent), d(new PrivateFilesWatcher(this))
{
	connect(FileWatcherManager::instance(), SIGNAL(fileChanged(QString)), d.data(), SLOT(fileChanged(QString)), Qt::QueuedConnection);
}

/*! Destroy the FilesWatcher */
FilesWatcher::~FilesWatcher()
{

}

void FilesWatcher::addPath(const QString& path)
{
	if (! d->_filenames.contains(path))
	{
		d->_filenames.append(path);
		FileWatcherManager::instance()->addFile(path);
	}
}

void FilesWatcher::removePath(const QString & path)
{
	if (d->_filenames.contains(path))
	{
		d->_filenames.removeAll(path);
		FileWatcherManager::instance()->removeFile(path);
	}
}

void FilesWatcher::removePaths(const QStringList& paths)
{
	d->removePaths(paths);
}

const QStringList& FilesWatcher::files() const
{
	return d->_filenames;
}
