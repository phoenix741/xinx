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

#ifndef __CONTENTVIEWCLASS_H__
#define __CONTENTVIEWCLASS_H__
#pragma once

// Qt header
#include <QHash>
#include <QFutureWatcher>
#include <QQueue>

class XinxProject;
class ContentViewNode;
class ContentViewParser;
class ContentViewNode;
class QTimerEvent;

/*!
 * \class ContentViewCache
 * \brief This class containt a list of pre-loaded file that can be used in content view obect.
 *
 * This class is creating by a project. All file defined in the project is pre-loaded. Next when
 * a content view object want to access to a file, he look to know if the file isn't already
 * pre-loaded.
 *
 * If the content of the file is found, he is returned ; else he is created. In this last case,
 * we launch a thread to fill the content of the node.
 *
 * The goal is to down the size in the memory, and speed up the loading of file and completion.
 */
class ContentViewCache : public QObject {
	Q_OBJECT
public:
	//! Create a content view cache and preloads project.
	ContentViewCache( XinxProject * project );
	//! Destroy the content view cache
	virtual ~ContentViewCache();

	/*!
	 * Return the content view for the given file name. Look in the cache if the
	 * node exist. If not, use the XinxPluginLoader to create the content file name.
	 * \param filename The file name of the content view to create.
	 * \return Return the content view of the file name given in parameters.
	 */
	ContentViewNode * contentOfFileName( const QString & filename );

	//! Return the list of current contents view loaded.
	QStringList contentsViewLoaded() const;

	//! Intialize a cache from the content of preloaded files.
	void initializeCache( QWidget * parent = 0 );
	//! Load file and import to cache
	void loadCache( QStringList filenames, QWidget * parent = 0 );
protected:
	virtual void timerEvent( QTimerEvent * event );
private slots:
	void resultReadyAt( int index );
private:
	ContentViewParser * createParser( const QString & filename, ContentViewNode* node = 0 );
	ContentViewParser * createParserAndNode( const QString & filename, ContentViewNode** refNode = 0 );

	QHash<QString,ContentViewNode*> m_nodes;
	QFutureWatcher<ContentViewParser*> * m_watcher;
	XinxProject * m_project;
	QQueue< ContentViewParser* > m_parsers;
	int m_timerId;
};

#endif /* __CONTENTVIEWCLASS_H__ */
