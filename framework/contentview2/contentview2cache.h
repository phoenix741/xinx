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

// Qt header
#include <QThread>
#include <QQueue>
#include <QFileSystemWatcher>
#include <QDateTime>

class XinxProject;
class TestContentView2;

namespace ContentView2 {

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
class LIBEXPORT Cache : public QThread {
	Q_OBJECT
public:
	//! Create a content view cache and launch the indexation.
	Cache( XinxProject * project );
	//! Destroy the content view cache
	virtual ~Cache();

	//! Return the list of current contents view loaded.
	QStringList contentsViewLoaded() const;

	//! Intialize a cache from the content of preloaded files.
	void initializeCache();
	//! Load file and import to cache
	void loadCache( const QStringList & filenames );
	//! Load and add the parser to cache
	void addToCache( Parser * parser );
	//! Create the root id
	int createRootId( const QString & filename, bool get = true, bool cached = true );
	//! Top the file as loaded
	void markAsLoaded( QSqlDatabase db, uint rootId );

	//! Method to destroy the cache for a file \e filename
	void destroyCache( const QString & filename );
public slots:
	//! Call this method if you want refresh the cache for a given file
	void refreshCache( const QString & filename );
signals:
	void cacheLoaded( uint file_id, const QString & filename );
	void cacheLoaded();
	void progressRangeChanged( int min, int max );
	void progressValueChanged( int value );
protected:
	virtual void run();
private:
	void changeDatmod( QSqlDatabase db, uint fileId, const QDateTime & datmod );
	Parser * createParser( const QString & filename, bool persistent );

	QFileSystemWatcher * m_watcher;
	QQueue< Parser* > m_parsers;
	XinxProject * m_project;
};

}

#endif // CONTENTVIEWCACHE_H
