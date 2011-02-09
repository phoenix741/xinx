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

#include "cache.h"
#include "parser.h"
#include "parserfactory.h"
#include <jobs/xinxjobmanager.h>
#include <core/filewatcher.h>

// Qt header
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>
#include <QMutex>
#include <QMetaType>

namespace ContentView3
{

/* CacheParserNotFoundException */

CacheParserNotFoundException::CacheParserNotFoundException(const QString& assertion, const QString& locationFile, int locationLine, const QString& locationMethod, const QString& message, const QString & filename) : XinxException(assertion, locationFile, locationLine, locationMethod, message.arg(filename))
{

}

CacheParserNotFoundException::~CacheParserNotFoundException() throw()
{

}

/* PrivateCache */

class PrivateCache
{
public:
	PrivateCache();

	struct CacheInformation
	{
		QString _parser_name;
		Cache::CacheVisibility _visibility;
		FilePtr _file;
	};

	QHash<QString,CacheInformation> _files;

	FilesWatcher * _watcher;
	XinxProject::Project * _project;
	QMutex _cache_mutex;

	void importOf(QList< FilePtr > & result, FilePtr file);
};

PrivateCache::PrivateCache() : _cache_mutex(QMutex::NonRecursive)
{

}

void PrivateCache::importOf(QList< ContentView3::FilePtr >& result, ContentView3::FilePtr file)
{
	foreach(const QString & import, file->imports())
	{
		const QString absoluteImport = QFileInfo(import).absoluteFilePath ();

		if (_files.contains(absoluteImport))
		{
			CacheInformation information = _files.value(absoluteImport);
			if ((information._visibility == Cache::IMPORT) && (!result.contains(information._file)))
			{
				result += information._file;

				importOf(result, information._file);
			}
		}
	}
}


/* Cache */

Cache::Cache(XinxProject::Project* project)
{
	qRegisterMetaType<ContentView3::FilePtr>("ContentView3::FilePtr");

	d                   = new PrivateCache;
	d->_project         = project;
	d->_watcher         = new FilesWatcher(this);

	connect(d->_watcher, SIGNAL(fileChanged(QString)), this, SLOT(refreshCache(QString)));
}

Cache::~Cache()
{
	delete d;
}

QStringList Cache::cachedFiles() const
{
	QStringList result;

	foreach(PrivateCache::CacheInformation information, d->_files)
	{
		if (information._visibility >= Cache::IMPORT)
		{
			result += information._file->filename();
		}
	}

	return result;
}

void Cache::addFileToCache(const QString& filename, bool force, ContentView3::Cache::CacheVisibility visibility)
{
	const QString absoluteFilename = QFileInfo(filename).absoluteFilePath ();

	// If the file is already in the cache, don't reload it.
	if (! force && d->_files.contains (absoluteFilename)) return;

	QFile * device = new QFile(absoluteFilename);
	if (device->open(QFile::ReadOnly))
	{
		FilePtr file = cachedFile (absoluteFilename);

		Parser * parser = 0;
		if (! d->_files.value (absoluteFilename)._parser_name.isEmpty())
		{
			parser = ParserFactory::getParserByType (d->_files.value (absoluteFilename)._parser_name);
		}
		else
		{
			parser = ParserFactory::getParserByFilename(absoluteFilename);
		}
		EXCEPT_ELSE(parser, CacheParserNotFoundException, "Cache::addFileToCache", "Can't find a parser for the file %1", absoluteFilename);

		parser->setFile(file);
		parser->setDevice(device);

		addFileToCache(parser, force, visibility);
	}
}

void Cache::addFileToCache(ContentView3::Parser* parser, bool force, ContentView3::Cache::CacheVisibility visibility)
{
	Q_ASSERT_X(parser, "Cache::addFileToCache", "Parser must be initialized");

	FilePtr file = parser->file();
	if (file && !file->filename().isEmpty())
	{
		if (visibility > Cache::NONE)
		{
			// If the file is already in the cache, don't reload it.
			if (! force && d->_files.contains (file->filename ())) return;

			QMutexLocker locker(&d->_cache_mutex);

			PrivateCache::CacheInformation information;
			information._file = file;
			information._visibility = visibility;
			information._parser_name = parser->name ();
			d->_files.insert(file->filename(), information);

			QFileInfo fileInformation(file->filename());
			if (fileInformation.exists() && !file->filename().startsWith("datas:") && ! d->_watcher->files().contains(file->filename()))
				d->_watcher->addPath(file->filename());
		}
	}

	XinxJobManager::self ()->addJob(parser);
}

void Cache::parsed(FilePtr file)
{
	emit updated (file);
}

void Cache::refreshCache(const QString& filename)
{
	if (QFileInfo(filename).exists())
	{
		addFileToCache(filename, true, d->_files.value(filename)._visibility);
	}
	else
	{
		QMutexLocker locker(&d->_cache_mutex);

		d->_files.remove(filename);
	}
}

QList< FilePtr > Cache::importOf(ContentView3::FilePtr file) const
{
	QMutexLocker locker(&d->_cache_mutex);

	QList< FilePtr > result;

	foreach(PrivateCache::CacheInformation information, d->_files)
	{
		if (information._visibility == Cache::PROJECT)
		{
			result += information._file;
		}
	}

	d->importOf(result, file);

	return result;
}

FilePtr Cache::cachedFile(const QString & filename)
{
	const QString absoluteFilename = QFileInfo(filename).absoluteFilePath ();

	FilePtr file;

	if (d->_files.contains(absoluteFilename))
	{
		QMutexLocker locker(&d->_cache_mutex);

		file = d->_files.value(absoluteFilename)._file;
	}
	else
	{
		file = File::create (absoluteFilename, d->_project);
	}

	return file;
}

}

