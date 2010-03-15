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

/*!
 * \file contentviewcache.h
 * \brief Class to allow caching of content view object and access it.
 */

#ifndef CONTENTVIEWCACHE_H
#define CONTENTVIEWCACHE_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <contentview2/contentview2node.h>
#include <contentview2/contentview2parser.h>
#include <contentview2/contentview2file.h>

// Qt header
#include <QThread>
#include <QQueue>
#include <QFileSystemWatcher>
#include <QDateTime>

class XinxProject;
class TestContentView2;

namespace ContentView2
{

/*!
 * \class Cache
 * \brief This class is used to cache file used by the content view system.
 *
 * The goal of this class is to manage the loading of the content view system.
 *
 *
 * ATTENTION : The cache must be independent of the project. If no project the
 * content must be created in memory ... This because of need to have a content view even
 * if no project is opened. In the near future, the session file must be independ of the project:
 * in memory or in file...
 */
class LIBEXPORT Cache : public QThread
{
	Q_OBJECT
public:
	Cache();
	//! Destroy the content view cache
	virtual ~Cache();

	//! Return the list of current contents view loaded.
	QStringList contentsViewLoaded() const;

	//! Intialize a cache from the content of preloaded files.
	void initializeCache();
	//! Load and add the parser to cache
	void addToCache(XinxProject * project, const QString & path, const QString & selection, Parser * parser = 0);
	void addToCache(XinxProject * project, const QString & path, const QString & type, const QString & selection, Parser * parser = 0);
	void deleteFromCache(XinxProject * project, const QString & path, bool isCached = true);

	void registerPath(const QString & path);
	void unregisterPath(const QString & path);
public slots:
	//! Call this method if you want refresh the cache for a given file
	void refreshCache(const QString & filename);
signals:
	void cacheLoaded(const ContentView2::File & file);
	void cacheLoaded();
	void progressRangeChanged(int min, int max);
	void progressValueChanged(int value);
protected:
	virtual void run();
	virtual void timerEvent(QTimerEvent * event);
private:
	struct struct_cache
	{
		Parser * parser;
		XinxProject * project;
		bool cached;
		QString path, selection, type;
	};

	void regenerateImport(QSqlDatabase db);

	void addToCache(struct_cache p);
	Project getProject(QSqlDatabase db, XinxProject * project);
	File getFile(QSqlDatabase db, struct_cache p);

	QFileSystemWatcher * m_watcher;
	QQueue< struct_cache > m_parsers;
	QQueue< struct_cache > m_toDelete;
	QStringList m_registeredFile;
};

}

#endif // CONTENTVIEWCACHE_H
