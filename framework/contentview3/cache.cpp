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
#include "cacheexception.h"
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

/*!
 * \defgroup contentview Content View
 */

/*!
 * \brief This namespace contains all class used for content view.
 * \since 0.10.1
 *
 * This namespace contains all class and method used to generate a contentview. The contentview
 * can be used for the completion too.
 */
namespace ContentView3
{

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
	XinxProject::ProjectPtrWeak _project;
	QMutex _cache_mutex;

	void importOf(QList< FilePtr > & result, FilePtr file, const ResolverContextInformation & ctxt) const;
	QString locationOf(const QString& relativeFilename, const ResolverContextInformation & ctxt) const;
};

PrivateCache::PrivateCache() : _cache_mutex(QMutex::NonRecursive)
{

}

void PrivateCache::importOf(QList< ContentView3::FilePtr >& result, ContentView3::FilePtr file, const ResolverContextInformation & ctxt) const
{
	ResolverContextInformation ctxtChild = ctxt;
	if (! ctxtChild.isEmpty())
	{
		ctxtChild.setCurrentPath(QDir(file->filename()).absolutePath());
	}

	foreach(const QString & import, file->imports())
	{
		const QString absoluteImport = QFileInfo(locationOf(import, ctxt)).absoluteFilePath();

		if (_files.contains(absoluteImport))
		{
			CacheInformation information = _files.value(absoluteImport);
			if ((information._visibility == Cache::IMPORT) && (!result.contains(information._file)))
			{
				result += information._file;

				importOf(result, information._file, ctxtChild);
			}
		}
	}
}

QString PrivateCache::locationOf(const QString& relativeFilename, const ResolverContextInformation & ctxt) const
{
	XinxProject::ProjectPtr project = _project.toStrongRef();
	if (project)
	{
		return project->resolver()->resolveFileName(relativeFilename, ctxt);
	}
	else
	{
		return relativeFilename;
	}
}


/* Cache */

Cache::Cache(XinxProject::ProjectPtrWeak project)
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

ContentView3::Parser* Cache::addFileToCache(const QString& filename, const ResolverContextInformation & information, bool force, ContentView3::Cache::CacheVisibility visibility)
{
	Q_ASSERT_X(information.isEmpty() || (information.project()->projectPath() == d->_project.toStrongRef()->projectPath()), "Cache::addFileToCache", "Context must be in the same project that the cache");
	const QString absoluteFilename = QFileInfo(d->locationOf(filename, information)).absoluteFilePath();

	// If the file is already in the cache, don't reload it.
	if (! force && d->_files.contains(absoluteFilename)) return 0;

	if (QFile::exists(absoluteFilename))
	{
		FilePtr file = cachedFile(absoluteFilename);

		Parser * parser = 0;
		if (! d->_files.value(absoluteFilename)._parser_name.isEmpty())
		{
			parser = ParserFactory::getParserByType(d->_files.value(absoluteFilename)._parser_name);
		}
		else
		{
			parser = ParserFactory::getParserByFilename(absoluteFilename);
		}
		EXCEPT_ELSE(parser, CacheParserNotFoundException, "Cache::addFileToCache", "Can't find a parser for the file %1", absoluteFilename);

		parser->setFile(file);
		parser->setDevice(new QFile(absoluteFilename));

		ResolverContextInformation ctx = information;
		ctx.setCurrentPath(QFileInfo(absoluteFilename).absolutePath());
		parser->setContext(ctx);

		addFileToCache(parser, force, visibility);

		return parser;
	}

	return 0;
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
			if (! force && d->_files.contains(file->filename())) return;

			QMutexLocker locker(&d->_cache_mutex);

			PrivateCache::CacheInformation information;
			information._file = file;
			information._visibility = visibility;
			information._parser_name = parser->name();
			d->_files.insert(file->filename(), information);

			QFileInfo fileInformation(file->filename());
			if (fileInformation.exists() && !file->filename().startsWith("datas:") && ! d->_watcher->files().contains(file->filename()))
				d->_watcher->addPath(file->filename());
		}
	}

	XinxJobManager::self()->addJob(parser);
}

void Cache::parsed(ContentView3::FilePtr file)
{
	emit updated(file);
}

void Cache::refreshCache(const QString& filename)
{
	if (QFileInfo(filename).exists())
	{
		addFileToCache(filename, ResolverContextInformation(), true, d->_files.value(filename)._visibility);
	}
	else
	{
		QMutexLocker locker(&d->_cache_mutex);

		d->_files.remove(filename);
	}
}

QList< FilePtr > Cache::importOf(ContentView3::FilePtr file, const ResolverContextInformation & information) const
{
	Q_ASSERT_X(information.isEmpty() || (information.project()->projectPath() == d->_project.toStrongRef()->projectPath()), "Cache::addFileToCache", "Context must be in the same project that the cache");
	ResolverContextInformation ctx = information;
	if (information.isEmpty())
	{
		XinxProject::ProjectPtr project = d->_project.toStrongRef();
		if (project)
		{
			ctx = project->resolver()->createContextInformation(file->filename());
		}
	}

	QMutexLocker locker(&d->_cache_mutex);
	QList< FilePtr > result;

	foreach(PrivateCache::CacheInformation information, d->_files)
	{
		if (information._visibility == Cache::PROJECT)
		{
			result += information._file;
		}
	}

	d->importOf(result, file, ctx);

	return result;
}

FilePtr Cache::cachedFile(const QString & filename, const ResolverContextInformation & information)
{
	Q_ASSERT_X(information.isEmpty() || (information.project()->projectPath() == d->_project.toStrongRef()->projectPath()), "Cache::addFileToCache", "Context must be in the same project that the cache");
	const QString absoluteFilename = QFileInfo(d->locationOf(filename, information)).absoluteFilePath();

	FilePtr file;

	if (d->_files.contains(absoluteFilename))
	{
		QMutexLocker locker(&d->_cache_mutex);

		file = d->_files.value(absoluteFilename)._file;
	}
	else
	{
		// Doesn't come from the cache.
		file = File::create(absoluteFilename, d->_project.toStrongRef());
	}

	return file;
}

}

