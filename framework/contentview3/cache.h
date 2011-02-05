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

#ifndef CONTENTVIEW3_CACHE_H
#define CONTENTVIEW3_CACHE_H

// Xinx header
#include <contentview3/file.h>
#include <core/exceptions.h>

// Qt header
#include <QString>
#include <QAtomicInt>
#include <QFileSystemWatcher>
#include <QHash>

namespace XinxProject
{
	class Project;
}
class QThreadPool;

namespace ContentView3 {

class Parser;
class PrivateCache;


class LIBEXPORT CacheParserNotFoundException : public XinxException
{
public:
	CacheParserNotFoundException(const QString& assertion, const QString& locationFile, int locationLine, const QString& locationMethod, const QString& message, const QString& filename);
	virtual ~CacheParserNotFoundException() throw();
};

class LIBEXPORT Cache : public QObject
{
	Q_OBJECT
public:
	enum CacheVisibility
	{
		NONE    = 0,
		IMPORT  = 5,
		PROJECT = 10
	};

	explicit Cache(XinxProject::Project * project);
	~Cache();

	QStringList cachedFiles() const;
	void addFileToCache(const QString & filename, bool force = false, CacheVisibility visibility = Cache::IMPORT);
	void addFileToCache(ContentView3::Parser * parser, bool force = false, CacheVisibility visibility = Cache::IMPORT);

	QList<FilePtr> importOf(ContentView3::FilePtr file) const;
	FilePtr cachedFile(const QString & filename);
signals:
	void updated(FilePtr file);
public slots:
	void refreshCache(const QString & filename);
private slots:
	void parsed(FilePtr file);
private:
	PrivateCache * d;
	friend class ContentView3::Parser;
};

}

#endif // CONTENTVIEW3_CACHE_H
