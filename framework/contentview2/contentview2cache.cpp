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

// Xinx header
#include "contentview2cache.h"
#include "project/xinxproject.h"
#include "plugins/xinxpluginsloader.h"
#include "core/exceptions.h"
#include "contentview2/contentview2manager.h"
#include "contentview2/contentview2project.h"
#include "contentview2/contentview2file.h"
#include "contentview2/contentview2node.h"
#include "contentview2/contentview2parserfactory.h"

// Qt header
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QTextCodec>

namespace ContentView2
{

/* Cache */

Cache::Cache()
{
	qRegisterMetaType<ContentView2::File>("ContentView2::File");
	m_watcher = new QFileSystemWatcher(this);
	connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(refreshCache(QString)));
}

Cache::~Cache()
{

}

QStringList Cache::contentsViewLoaded() const
{
	return m_watcher->files();
}

void Cache::initializeCache()
{
	if (m_watcher->files().size())
		m_watcher->removePaths(m_watcher->files());

	try
	{
		Project project( Manager::self()->database(), XINXProjectManager::self()->project() );

		QSqlQuery cacheQuery("SELECT path, type, selection FROM cv_file WHERE project_id=:project and cached = :cached", Manager::self()->database());
		cacheQuery.bindValue(":project", project.projectId());
		cacheQuery.bindValue(":cached", true);
		bool result = cacheQuery.exec();
		Q_ASSERT_X(result, "Cache::initializeCache", qPrintable(cacheQuery.lastError().text()));

		while (cacheQuery.next())
		{
			const QString path         = cacheQuery.value(0).toString();
			const QString type         = cacheQuery.value(1).toString();
			const QString selection    = cacheQuery.value(2).toString();

			struct_cache fileCache;
			fileCache.project      = XINXProjectManager::self()->project();
			fileCache.selection    = selection;
			fileCache.cached       = true;
			fileCache.path         = path;
			fileCache.type         = type;
			fileCache.parser       = 0;

			m_parsers.append(fileCache);
			if(QFileInfo(path).exists())
				m_watcher->addPath(path);
		}
	}
	catch(...)
	{
		// In case of error, make nothing
	}

	startTimer(300);
}

void Cache::addToCache(struct_cache p)
{
	m_parsers.append(p);
	if (QFileInfo(p.path).exists() && ! m_watcher->files().contains(p.path))
		m_watcher->addPath(p.path);
}

void Cache::addToCache(XinxProject * project, const QString & path, const QString & selection, Parser * parser)
{
	struct_cache fileCache;
	fileCache.project      = project;
	fileCache.cached       = parser == 0;
	fileCache.selection    = selection;
	fileCache.path         = path;
	fileCache.type         = QString();
	fileCache.parser       = parser;

	m_parsers.append(fileCache);
	if (QFileInfo(path).exists() && ! m_watcher->files().contains(path))
		m_watcher->addPath(path);

	startTimer(500);
}

void Cache::addToCache(XinxProject * project, const QString & path, const QString & type, const QString & selection, Parser * parser)
{
	struct_cache fileCache;
	fileCache.project      = project;
	fileCache.cached       = parser == 0;
	fileCache.selection    = selection;
	fileCache.path         = path;
	fileCache.type         = type;
	fileCache.parser       = parser;
	if (QFileInfo(path).exists() && ! m_watcher->files().contains(path))
		m_watcher->addPath(path);

	m_parsers.append(fileCache);

	startTimer(500);
}

void Cache::deleteFromCache(XinxProject * project, const QString & path, bool isCached)
{
	struct_cache fileCache;
	fileCache.project      = project;
	fileCache.cached       = isCached;
	fileCache.path         = path;
	if (QFileInfo(path).exists() && m_watcher->files().contains(path))
		m_watcher->removePath(path);

	m_toDelete.append(fileCache);

	startTimer(500);
}

void Cache::registerPath(const QString & path)
{
	if(! m_registeredFile.contains(path))
		m_registeredFile.append(path);
}

void Cache::unregisterPath(const QString & path)
{
	m_registeredFile.removeAll(path);
	startTimer(500);
}

void Cache::timerEvent(QTimerEvent * event)
{
	killTimer(event->timerId());
	if (m_parsers.size())
		start(QThread::IdlePriority);
}

Project Cache::getProject(QSqlDatabase db, XinxProject * p)
{
	Project project;
	try
	{
		project = Project( db, p );
	}
	catch(ProjectException e)
	{
		if( p )
		{
			project.setProjectName( p->projectName() );
			project.setProjectPath( p->fileName() );
		}
		else
		{
			project.setProjectName( "/" );
			project.setProjectPath( "/" );
		}
		project.create(db);
	}
	return project;
}

File Cache::getFile(QSqlDatabase db, struct_cache p)
{
	File file;
	try
	{
		/* Lecture du fichier */
		file = File( db, p.project, p.path, p.cached );
	}
	catch(FileException e)
	{
		/* Lecture/Création du projet */
		Project project = getProject(db, p.project);

		/* Creation du fichier */
		file.setProject(project);
		file.setPath(p.path);
		file.setIsCached(p.cached);
		file.setIsLoaded(false);
		file.setSelection(p.selection);
		if (p.type.isEmpty())
		{
			file.setType(ParserFactory::getParserTypeByFilename(p.path));
		}
		else
		{
			file.setType(p.type);
		}

		file.create(db);

		/* Creation de la racine */
		Node node;
		node.setFile(file);
		node.setData(p.path, Node::NODE_DISPLAY_NAME);
		node.setData(p.path, Node::NODE_NAME);
		node.setData("RootFilename", Node::NODE_TYPE);

		node.create(db);

		/* Indication de la racine sur le fichier */
		file.setRoot(node);
		file.update(db);
	}

	return file;
}

void Cache::regenerateImport(QSqlDatabase db)
{
	QMultiHash<int,int> imports;

	bool result = db.transaction();
	Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(db.lastError().text()));

	try
	{
		QSqlQuery deleteAllImportsQuery("DELETE FROM cv_import WHERE automatic_import=:automatic", db);
		deleteAllImportsQuery.bindValue(":automatic", QVariant::fromValue(true));
		bool result = deleteAllImportsQuery.exec();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(deleteAllImportsQuery.lastError().text()));

		QSqlQuery selectImportQuery("SELECT parent_id, child_id FROM cv_import WHERE automatic_import=:automatic", db);
		selectImportQuery.bindValue(":automatic", QVariant::fromValue(false));
		result = selectImportQuery.exec();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(selectImportQuery.lastError().text()));

		while(selectImportQuery.next())
		{
			imports.insert(selectImportQuery.value(0).toInt(), selectImportQuery.value(1).toInt());
		}

		foreach(int key, imports.keys())
		{
			foreach(int value, imports.values(key))
			{
				foreach(int node, imports.keys(key))
				{
					imports.insert(node, value);
					QSqlQuery insertImportQuery("INSERT INTO cv_import(parent_id, child_id, automatic_import) VALUES(:parent_id, :child_id, :automatic)", db);
					insertImportQuery.bindValue(":parent_id", node);
					insertImportQuery.bindValue(":child_id",value);
					insertImportQuery.bindValue(":automatic", true);
					result = insertImportQuery.exec();
					Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(insertImportQuery.lastError().text()));
				}
			}
		}

		result = db.commit();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(db.lastError().text()));
	}
	catch(FileException e)
	{
		result = db.rollback();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(db.lastError().text()));
	}
}

void Cache::run()
{
	{
		QSqlDatabase db = QSqlDatabase::cloneDatabase(Manager::self()->database(), "CONTENTVIEW_SESSION_THREAD");
		db.open();

		bool deleteNotRegistered = true;
		while (m_parsers.size() || m_toDelete.size() || (deleteNotRegistered))
		{
			QList<struct_cache> parsers;
			while (m_parsers.size())
				parsers.append(m_parsers.dequeue());

			QList<struct_cache> toDelete;
			while (m_toDelete.size())
				toDelete.append(m_toDelete.dequeue());

			emit progressRangeChanged(0, parsers.size() + toDelete.size() + (deleteNotRegistered ? 1 : 0));
			emit progressValueChanged(0);

			int progress = 0;

			/* Cache to create */
			for (int i = 0 ; i < parsers.size() ; i++)
			{
				bool result = db.transaction();
				Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));

				Parser * parser = parsers.at(i).parser;
				File file;
				try
				{
					QDateTime lastModified;
					file = getFile(db, parsers.at(i));

					bool manageDatmod = (file.isCached() && QFileInfo(file.path()).exists());
					if (manageDatmod)
					{
						lastModified = QDateTime::fromString(QFileInfo(file.path()).lastModified().toString(Qt::ISODate), Qt::ISODate);
						if ((lastModified <= file.datmod()) && file.isLoaded())
						{
							result = db.commit();
							Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
							emit progressValueChanged(progress++);
							continue;
						}
					}

					if( ! parser )
						parser = ParserFactory::getParserByType(file.type());
					if( ! parser )
					{
						result = db.commit();
						Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
						emit progressValueChanged(progress++);
						continue;
					}

					if (file.isCached() && QFileInfo(file.path()).exists())
					{
						parser->setFilename(file.path());
					}

					parser->setRootNode(Node(db, file.rootId()));
					parser->setDatabase(db);
					try
					{
						parser->load();
						if(parser->codec())
							file.setEncoding(parser->codec()->name());

						file.destroyImports(db);
						foreach(const QString & import, parser->imports())
						{
							struct_cache fileCache;
							fileCache.project      = XINXProjectManager::self()->project();
							fileCache.path         = import;
							fileCache.cached       = true;
							fileCache.selection    = "M";
							fileCache.type         = QString();
							fileCache.parser       = 0;

							File importFile = getFile(db, fileCache);
							file.addImport(db, importFile, false);
							addToCache(fileCache);
						}

						file.setIsLoaded(true);
						if (lastModified.isValid())
						{
							file.setDatmod (lastModified);
						}
						file.update(db);

						result = db.commit();
						Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));

						emit cacheLoaded(file);
					}
					catch(ParserException e)
					{
						result = db.rollback();
						Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
					}
				}
				catch (ParserException e)
				{
					result = db.rollback();
					Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));

					if( file.isValid() )
					{
						struct_cache fileCache;
						fileCache.project      = XINXProjectManager::self()->project();
						fileCache.path         = file.path();
						fileCache.cached       = file.isCached();
						m_toDelete.append(fileCache);
					}
				}
				catch (...)
				{
					result = db.rollback();
					Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
				}

				delete parser;

				emit progressValueChanged(progress++);
			}

			/* Cached to deletes */
			for (int i = 0 ; i < toDelete.size() ; i++)
			{
				bool result = db.transaction();
				Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));

				File file;
				try
				{
					file = File(db, toDelete.at(i).project, toDelete.at(i).path, toDelete.at(i).cached);
					file.destroy(db);

					result = db.commit();
					Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
				}
				catch (...)
				{
					result = db.rollback();
					Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
				}

				emit progressValueChanged(progress++);
			}

			/* Delete file not registered */
			if(deleteNotRegistered)
			{
				deleteNotRegistered = false;
				try
				{
					Project project(db, XINXProjectManager::self()->project());

					QSqlQuery query("SELECT path, id FROM cv_file WHERE project_id=:project_id and cached=:cached", db);
					query.bindValue(":project_id", project.projectId());
					query.bindValue(":cached",false);
					bool result = query.exec();
					Q_ASSERT_X(result, "Cache::run", qPrintable(query.lastError().text()));

					while(query.next())
					{
						const QString path = query.value(0).toString();
						const int     id   = query.value(1).toInt();

						if(! m_registeredFile.contains(path))
						{
							bool result = db.transaction();
							Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));

							try
							{
								File file(db, id);
								file.destroy(db);

								result = db.commit();
								Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
							}
							catch(FileException e)
							{
								result = db.rollback();
								Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
							}
						}
					}
				}
				catch(ProjectException e)
				{
				}

				emit progressValueChanged(progress++);
			}
		}

		/* Regenerate automatic import */
		regenerateImport(db);

		/* End of thread */
		emit cacheLoaded();
	}

	QSqlDatabase::database("CONTENTVIEW_SESSION_THREAD").close();
	QSqlDatabase::removeDatabase("CONTENTVIEW_SESSION_THREAD");
}

void Cache::refreshCache(const QString & filename)
{
	if (QFileInfo(filename).exists())
	{
		struct_cache fileCache;
		fileCache.project      = XINXProjectManager::self()->project();
		fileCache.path         = filename;
		fileCache.cached       = true;
		fileCache.selection    = "M";
		fileCache.type         = QString();
		fileCache.parser       = 0;

		addToCache(fileCache);
	}
	else
	{
		deleteFromCache(XINXProjectManager::self()->project(), filename, true);
	}
}

}
