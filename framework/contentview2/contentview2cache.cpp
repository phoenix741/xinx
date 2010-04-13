/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

/*!
 * \defgroup ContentView2 Gestion de la completion
 */

/*!
 * \ingroup ContentView2
 * \since 0.9.0.0
 *
 * \brief This namespace contains all classes used to access to the new
 * Content View system.
 *
 * The new ContentView system is based on a Sqlite database. All the classes
 * of this namespace is used to simplify and manage the access of this class.
 *
 * You can use classes ContentView2::Node, ContentView2::File, and ContentView2::Project
 * to access to the content of the table without any query.
 *
 * Classes ContentView2::Parser and ContentView2::Cache are used to generate
 * the content of the database for using it.
 *
 * Classes ContentView2::TreeModel and ContentView2::CompletionModel are used
 * to show the content of the base to the user.
 */
namespace ContentView2
{

/* Cache */

/*!
 * \ingroup ContentView2
 * \class ContentView2::Cache
 * \brief This class is used to cache file used by the content view system.
 *
 * The goal of this class is to manage the loading of the content view system in the sqlite database.
 * At the initialisation of a project, each file are checked to update the database. If a file is
 * modified since the last
 *
 *
 * \attention The cache is independent of the project and created in a single sqlite database.
 * If the project isn't use, the sqlite database is not cleaned. It's possible, when
 * \b XINX is stopped to delete the session file. It will be re-created at start.
 *
 * \attention Only one cache at a time should be opened in the application. If more that one
 * cache is opened, the application can be unstable.
 *
 * A file in the database have mark, who said the file as cached or not. Only cached file
 * are check at start. Other file will automatically removed if there couldn't be find in
 * registered path (see registerPath() and unregisterPath()).
 */

/*!
 * \fn void Cache::cacheLoaded(const ContentView2::File & file)
 * \brief Signal emited when the cache for the file \e file is loaded.
 */

/*!
 * \fn void Cache::cacheLoaded()
 * \brief Signal emited when the process is finished.
 */

/*!
 * \fn void Cache::progressRangeChanged(int min, int max)
 * \brief Signal emited when the progress value change (a new file is added).
 */

/*!
 * \fn void Cache::progressValueChanged(int value)
 * \brief Signal emited each time a file is process to indicate the progress.
 */


//! Create a new instance.
Cache::Cache()
{
	qRegisterMetaType<ContentView2::File>("ContentView2::File");
	m_watcher = new QFileSystemWatcher(this);
	connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(refreshCache(QString)));
	m_timer.setSingleShot(true);
	m_timer.setInterval(500);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerEvent()));
}

//! Destroy this instance
Cache::~Cache()
{

}

/*!
 * \brief Return the list of current contents view loaded.
 *
 * This method return the list of currently file loaded. The method return only
 * file mark as cached.
 */
QStringList Cache::contentsViewLoaded() const
{
	return m_watcher->files();
}

/*!
 * \brief Intialize a cache from the content of preloaded files.
 *
 * Load all file in the cache mark as cached. Use each predefined parser to reload the
 * file. The file will be test later to know if the file is already loaded or not.
 *
 * Each path is added in a QFileSystemWatcher to watch file modified and update
 * the cache if the file is modified.
 */
void Cache::initializeCache()
{
	m_timer.stop();

	if (m_watcher->files().size())
		m_watcher->removePaths(m_watcher->files());

	try
	{
		Project project(Manager::self()->database(), XINXProjectManager::self()->project());

		QSqlQuery cacheQuery = Manager::self()->getSqlQuery("SELECT path, type, selection FROM cv_file WHERE project_id=:project and cached = :cached", Manager::self()->database());
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
			if (QFileInfo(path).exists())
				m_watcher->addPath(path);
		}
	}
	catch (...)
	{
		// In case of error, make nothing
	}

	m_timer.start();
}

void Cache::addToCache(struct_cache p)
{
	m_parsers.append(p);
	if (QFileInfo(p.path).exists() && ! m_watcher->files().contains(p.path))
		m_watcher->addPath(p.path);
}

/*!
 * \brief Add the path \p path to the cache.
 *
 * If the file is a valid file, the file will be watched. The file will be
 * added in the cache for the project \p project.
 *
 * \param project Project for wich the file must be added. If NULL, the file
 * will be added for all project.
 * \param path The path to add to the cache. If the path is valid, the file
 * will be watched too.
 * \param selection If selection is equals to '*', this file can be used in all
 * type of file for completion. Else the file is exclude of the completion unless
 * it will be explicitely added.
 * \param parser The parser to use for parsing the file. If the parser is NULL,
 * the default parser for this type of file will be used and the file is considered as
 * cached.
 *
 * \overload
 */
void Cache::addToCache(XinxProject * project, const QString & path, const QString & selection, Parser * parser)
{
	m_timer.stop();

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

	m_timer.start();
}

/*!
 * \brief Add the path \p path to the cache.
 *
 * If the file is a valid file, the file will be watched. The file will be
 * added in the cache for the project \p project.
 *
 * \param project Project for wich the file must be added. If NULL, the file
 * will be added for all project.
 * \param path The path to add to the cache. If the path is valid, the file
 * will be watched too.
 * \param type The type of parser to use. Given if the default parser isn't adapted.
 * \param selection If selection is equals to '*', this file can be used in all
 * type of file for completion. Else the file is exclude of the completion unless
 * it will be explicitely added.
 * \param parser The parser to use for parsing the file. If the parser is NULL,
 * the default parser for this type of file will be used and the file is considered as
 * cached.
 */
void Cache::addToCache(XinxProject * project, const QString & path, const QString & type, const QString & selection, Parser * parser)
{
	m_timer.stop();

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

	m_timer.start();
}

/*!
 * \brief Indicate to removed the given file from the cache.
 *
 * The file will be not watched after call this method.
 */
void Cache::deleteFromCache(XinxProject * project, const QString & path, bool isCached)
{
	m_timer.stop();

	struct_cache fileCache;
	fileCache.project      = project;
	fileCache.cached       = isCached;
	fileCache.path         = path;
	if (QFileInfo(path).exists() && m_watcher->files().contains(path))
		m_watcher->removePath(path);

	m_toDelete.append(fileCache);

	m_timer.start();
}

/*!
 * \brief Register an uncached file.
 *
 * This method is used to register a file add as not cached. If the file
 * isn't registered, it will be automatically deleted at the next call of
 * the thread.
 */
void Cache::registerPath(const QString & path)
{
	if (! m_registeredFile.contains(path))
		m_registeredFile.append(path);
}

/*!
 * \brief Unregister an uncached file.
 *
 * This method remove the file from the registered file and launch the
 * thread to remove it from the base.
 */
void Cache::unregisterPath(const QString & path)
{
	m_timer.stop();
	m_registeredFile.removeAll(path);
	m_timer.start();
}

void Cache::timerEvent()
{
	m_timer.stop();
	if (m_parsers.size() && (!isRunning()))
		start(QThread::IdlePriority);
}

Project Cache::getProject(QSqlDatabase db, XinxProject * p)
{
	Project project;
	try
	{
		project = Project(db, p);
	}
	catch (ProjectException e)
	{
		if (p)
		{
			project.setProjectName(p->projectName());
			project.setProjectPath(p->fileName());
		}
		else
		{
			project.setProjectName("/");
			project.setProjectPath("/");
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
		file = File(db, p.project, p.path, p.cached);
	}
	catch (FileException e)
	{
		/* Lecture/Creation du projet */
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
		QSqlQuery deleteAllImportsQuery = Manager::self()->getSqlQuery("DELETE FROM cv_import WHERE automatic_import=:automatic", db);
		deleteAllImportsQuery.bindValue(":automatic", QVariant::fromValue(true));
		bool result = deleteAllImportsQuery.exec();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(deleteAllImportsQuery.lastError().text()));

		QSqlQuery selectImportQuery = Manager::self()->getSqlQuery("SELECT parent_id, child_id FROM cv_import WHERE automatic_import=:automatic", db);
		selectImportQuery.bindValue(":automatic", QVariant::fromValue(false));
		result = selectImportQuery.exec();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(selectImportQuery.lastError().text()));

		while (selectImportQuery.next())
		{
			imports.insert(selectImportQuery.value(0).toInt(), selectImportQuery.value(1).toInt());
		}

		foreach(int key, imports.keys())
		{
			foreach(int value, imports.values(key))
			{
				foreach(int node, imports.keys(key))
				{
					if (!imports.contains(node, value))
					{
						imports.insert(node, value);
						QSqlQuery insertImportQuery = Manager::self()->getSqlQuery("INSERT INTO cv_import(parent_id, child_id, automatic_import) VALUES(:parent_id, :child_id, :automatic)", db);
						insertImportQuery.bindValue(":parent_id", node);
						insertImportQuery.bindValue(":child_id",value);
						insertImportQuery.bindValue(":automatic", true);
						result = insertImportQuery.exec();
						Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(insertImportQuery.lastError().text()));
					}
				}
			}
		}

		result = db.commit();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(db.lastError().text()));
	}
	catch (FileException e)
	{
		result = db.rollback();
		Q_ASSERT_X(result, "Cache::regenerateImport", qPrintable(db.lastError().text()));
	}
}

/*!
 * \brief Process the cache refresh.
 *
 * This internal method process the cache refresh. This method remove all the
 * unregistered files, remove the requested file and add all added file (at the
 * condition that the file isn't alread cached).
 *
 * If the file is already cached, and the file isn't modified, the file isn't process.
 */
void Cache::run()
{
	{
		QSqlDatabase db = QSqlDatabase::cloneDatabase(Manager::self()->database(), "CONTENTVIEW_SESSION_THREAD");
		db.open();

		bool deleteNotRegistered = true;
		while ((m_parsers.size() || m_toDelete.size() || (deleteNotRegistered)))
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
				if (! result)
				{
					sleep(500);
					i--;
				}

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
							emit cacheLoaded(file);
							continue;
						}
					}

					if (! parser)
						parser = ParserFactory::getParserByType(file.type());
					if (! parser)
					{
						result = db.commit();
						Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
						emit progressValueChanged(progress++);
						emit cacheLoaded(file);
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
						ErrorManager::self()->clearMessages(file.path());
						parser->load();
						if (parser->codec())
							file.setEncoding(parser->codec()->name());

						file.destroyImports(db);
						foreach(const QString & import, parser->imports())
						{
							if (! import.isEmpty())
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
						}

						file.setIsLoaded(true);
						if (lastModified.isValid())
						{
							file.setDatmod(lastModified);
						}
						file.update(db);

						result = db.commit();
						while (! result)
						{
							result = db.commit();
							sleep(500);
						}

						emit cacheLoaded(file);
					}
					catch (ParserException e)
					{
						ErrorManager::self()->addMessage(file.path(), e.getLine(), ErrorManager::MessageError, e);
						result = db.rollback();
						Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
					}
				}
				catch (ParserException e)
				{
					result = db.rollback();
					Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));

					if (file.isValid())
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
			if (deleteNotRegistered)
			{
				deleteNotRegistered = false;
				try
				{
					Project project(db, XINXProjectManager::self()->project());

					QSqlQuery query = Manager::self()->getSqlQuery("SELECT path, id FROM cv_file WHERE project_id=:project_id and cached=:cached", db);
					query.bindValue(":project_id", project.projectId());
					query.bindValue(":cached",false);
					bool result = query.exec();
					Q_ASSERT_X(result, "Cache::run", qPrintable(query.lastError().text()));

					while (query.next())
					{
						const QString path = query.value(0).toString();
						const int     id   = query.value(1).toInt();

						if (! m_registeredFile.contains(path))
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
							catch (FileException e)
							{
								result = db.rollback();
								Q_ASSERT_X(result, "Cache::run", qPrintable(db.lastError().text()));
							}
						}
					}
				}
				catch (ProjectException e)
				{
				}

				emit progressValueChanged(progress++);
			}
		}

		/* Regenerate automatic import */
		regenerateImport(db);

		/* Clear QSqlQueries */
		Manager::self()->clearPool(db);

		/* End of thread */
		emit cacheLoaded();
	}

	QSqlDatabase::database("CONTENTVIEW_SESSION_THREAD").close();
	QSqlDatabase::removeDatabase("CONTENTVIEW_SESSION_THREAD");
}

/*!
 * \brief Call this method if you want refresh the cache manuelly for a given file.
 *
 * This method is automatically called when a watched file is modified.
 */
void Cache::refreshCache(const QString & filename)
{
	if (QFileInfo(filename).exists())
	{
		m_timer.stop();
		struct_cache fileCache;
		fileCache.project      = XINXProjectManager::self()->project();
		fileCache.path         = filename;
		fileCache.cached       = true;
		fileCache.selection    = "M";
		fileCache.type         = QString();
		fileCache.parser       = 0;

		addToCache(fileCache);
		m_timer.start();
	}
	else
	{
		deleteFromCache(XINXProjectManager::self()->project(), filename, true);
	}
}

}
