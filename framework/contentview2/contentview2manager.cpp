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
#include "contentview2manager.h"
#include "core/xinxcore.h"
#include "project/xinxproject.h"
#include "contentview2/contentview2cache.h"
#include "snipets/snipetcompletionparser.h"

// Qt header
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace ContentView2
{

/* DatabaseException */

DatabaseException::DatabaseException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message)
		: XinxException(assertion, locationFile, locationLine, locationMethod, message)
{
}

/* Static member */

Manager * Manager::s_self = 0;

/* Manager */

Manager::Manager()
{
	openDatabase();
	m_cache = new Cache();
}

Manager::~Manager()
{
	if (s_self == this)
	{
		s_self = 0;
	}

	closeDatabase();
	delete m_cache;
}

QSqlQuery Manager::getSqlQuery(const QString & query, QSqlDatabase db)
{
	QSqlDatabase dbUse = db.isValid() ? db :QSqlDatabase::addDatabase("QSQLITE", "CONTENTVIEW_SESSION");
	QSqlQuery q;
	if (! m_queries[dbUse.connectionName()].contains(query))
	{
		//qDebug() << "Insert query in cache : " << query;
		q = QSqlQuery(query, dbUse);
		m_queries[dbUse.connectionName()].insert(query, q);
	}
	else
	{
		q = m_queries[dbUse.connectionName()].value(query);
	}

	return q;
}

void Manager::clearPool(const QSqlDatabase & db)
{
	m_queries.remove(db.connectionName());
}

void Manager::addInitializationParser(bool isGlobal, const QString & path, const QString & type, const QString & selection)
{
	ParserTuple t;
	t.isGlobal   = isGlobal;
	t.selection  = selection;
	t.path       = path;
	t.type       = type;
	m_persistentParser.append(t);
}

void Manager::initializeDatabase()
{
	foreach(ParserTuple t, m_persistentParser)
	{
		m_cache->addToCache(t.isGlobal ? 0 : XINXProjectManager::self()->project(), t.path, t.type, t.selection);
	}

	m_cache->initializeCache();
}

QString Manager::fileName() const
{
	QFileInfo fileInfo("datas:contentview.db");
	QString databaseFileName = fileInfo.absoluteFilePath();
	if (! fileInfo.exists())
	{
		databaseFileName = QDir(QDir::searchPaths("datas").at(0)).absoluteFilePath("contentview.db");
	}
	return databaseFileName;
}

void Manager::openDatabase()
{
	// Create the db object
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "CONTENTVIEW_SESSION");
	db.setDatabaseName(fileName());
	bool result = db.open();
	Q_ASSERT_X(result, "Manager::openDatabase", qPrintable(db.lastError().text()));
	db.exec("PRAGMA synchronous=OFF");
	db.exec("PRAGMA temp_store=MEMORY");

	// Check the content
	QStringList tables = db.tables();
	if (! tables.contains("cv_file"))
		createDatabase(db);
}

void Manager::compactDatabase()
{
	database().exec("VACUUM");
}

void Manager::rebuildIndex()
{
	destroyIndex();
	createIndex();
}

void Manager::destroyIndex()
{
	QSqlQuery q(database());
	bool result;

	/* Destroy Index */
	result = q.exec("DROP INDEX IF EXISTS cv_project_idx1");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_file_idx1");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_file_idx3");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_import_idx1");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_node_idx1");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_node_idx2");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_node_idx3");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_node_idx4");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_node_idx5");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
	result = q.exec("DROP INDEX IF EXISTS cv_node_property_idx1");
	Q_ASSERT_X(result, "Manager::destroyIndex", qPrintable(q.lastError().text()));
}

void Manager::createIndex()
{
	QSqlQuery q(database());
	bool result;
	/* Create Index */
	result = q.exec("CREATE INDEX cv_project_idx1 on cv_project (path ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE UNIQUE INDEX cv_file_idx1 on cv_file (path ASC, project_id ASC, cached ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_file_idx3 on cv_file (root_id ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_import_idx1 on cv_import (parent_id ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_node_idx1 on cv_node (parent_id ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_node_idx2 on cv_node (name ASC, type ASC, file_id ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_node_idx3 on cv_node (parent_id ASC, hash ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_node_idx4 on cv_node (type ASC, display_name ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_node_idx5 on cv_node (parent_id ASC, type ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
	result = q.exec("CREATE INDEX cv_node_property_idx1 on cv_node_property (node_id ASC, ord ASC)");
	Q_ASSERT_X(result, "Manager::createIndex", qPrintable(q.lastError().text()));
}

void Manager::createDatabase(QSqlDatabase db)
{
	QSqlQuery q(db);
	bool result;
	/* Create tables */
	result = q.exec("CREATE TABLE cv_project (id INTEGER PRIMARY KEY, name TEXT, path TEXT)");
	Q_ASSERT_X(result, "Manager::createDatabase", qPrintable(q.lastError().text()));
	result = q.exec("CREATE TABLE cv_file (id INTEGER PRIMARY KEY, project_id INTEGER NOT NULL, path TEXT NOT NULL, cached TEXT, type TEXT, datmod TEXT, loaded TEXT, root_id INTEGER, selection TEXT, encoding TEXT)");
	Q_ASSERT_X(result, "Manager::createDatabase", qPrintable(q.lastError().text()));
	result = q.exec("CREATE TABLE cv_import (parent_id INTEGER, child_id INTEGER, automatic_import INTEGER)");
	Q_ASSERT_X(result, "Manager::createDatabase", qPrintable(q.lastError().text()));
	result = q.exec("CREATE TABLE cv_node (id INTEGER PRIMARY KEY, parent_id INTEGER, name TEXT, type TEXT NOT NULL, icon TEXT, display_name TEXT, tips TEXT, completion_value TEXT, line INTEGER, file_id INTEGER NOT NULL, hash INTEGER NOT NULL, property1 TEXT, property2 TEXT, property3 TEXT, property4 TEXT, property5 TEXT, property6 TEXT, property7 TEXT, property8 TEXT, property9 TEXT, property10 TEXT)");
	Q_ASSERT_X(result, "Manager::createDatabase", qPrintable(q.lastError().text()));
	result = q.exec("CREATE TABLE cv_node_property (node_id INTEGER NOT NULL, ord INTEGER NOT NULL, value TEXT)");
	Q_ASSERT_X(result, "Manager::createDatabase", qPrintable(q.lastError().text()));
	/* Insert */
	result = q.exec(QString("INSERT INTO cv_project(id, name, path) VALUES (0, '%1', '/')").arg(tr("General")));
	Q_ASSERT_X(result, "Manager::createDatabase", qPrintable(q.lastError().text()));

	createIndex();
}

void Manager::closeDatabase()
{
	m_cache->wait();
	{
		QSqlDatabase db = QSqlDatabase::database("CONTENTVIEW_SESSION", false);
		clearPool(db);
		if (db.isOpen())
			db.close();
	}
	QSqlDatabase::removeDatabase("CONTENTVIEW_SESSION");
}

QSqlDatabase Manager::database() const
{
	return QSqlDatabase::database("CONTENTVIEW_SESSION", true);
}

Cache * Manager::cache()
{
	return m_cache;
}

Manager * Manager::self()
{
	if (! s_self)
	{
		s_self = new Manager();
		XINXStaticDeleter::self()->addObject(s_self);
	}
	return s_self;
}

} // namespace ContentView2
