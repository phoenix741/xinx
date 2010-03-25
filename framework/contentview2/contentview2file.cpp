/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include "contentview2file.h"
#include "contentview2node.h"
#include "contentview2project.h"
#include "project/xinxproject.h"

// Qt header
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

namespace ContentView2
{

/* FileException */

FileException::FileException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message)
	: XinxException(assertion, locationFile, locationLine, locationMethod, message)
{
}

/* PrivateFileContainer */

class PrivateFileContainer : public QSharedData
{
public:
	QPointer<XinxProject> m_project;
	QString m_path;
	bool m_cached;

	File m_file;

	void load(QSqlDatabase db);

	PrivateFileContainer();
	PrivateFileContainer(const PrivateFileContainer &other);
	~PrivateFileContainer();
};

PrivateFileContainer::PrivateFileContainer() : m_project(0), m_cached(false)
{
}

PrivateFileContainer::PrivateFileContainer(const PrivateFileContainer &other) : QSharedData(other), m_project(other.m_project), m_path(other.m_path), m_cached(other.m_cached)
{
}

PrivateFileContainer::~PrivateFileContainer()
{

}

void PrivateFileContainer::load(QSqlDatabase db)
{
	if (! m_file.isValid())
	{
		try
		{
			m_file = File(db, m_project, m_path, m_cached);
		}
		catch (FileException e)
		{

		}
	}
}

/* PrivateFile */

class PrivateFile : public QSharedData
{
public:
	long m_id, m_projectId, m_rootId;
	QString m_path, m_type, m_selection, m_encoding;
	QDateTime m_datmod;
	bool m_cached, m_loaded;

	PrivateFile();
	PrivateFile(const PrivateFile &other);
	~PrivateFile();
};

PrivateFile::PrivateFile() : m_id(-1), m_projectId(-1), m_rootId(-1), m_cached(false), m_loaded(false)
{
}

PrivateFile::PrivateFile(const PrivateFile &other) : QSharedData(other), m_id(other.m_id), m_projectId(other.m_projectId), m_rootId(other.m_rootId),
		m_path(other.m_path), m_type(other.m_type), m_selection(other.m_selection), m_encoding(other.m_encoding), m_datmod(other.m_datmod),
		m_cached(other.m_cached), m_loaded(other.m_loaded)
{
}

PrivateFile::~PrivateFile()
{

}

/* FileContainer */

FileContainer::FileContainer()
{
	d = new PrivateFileContainer;
}

FileContainer::FileContainer(const FileContainer & other) : d(other.d)
{
}

FileContainer::FileContainer(QPointer<XinxProject> project, const QString & path, bool isCached)
{
	d = new PrivateFileContainer;
	d->m_project = project;
	d->m_path    = path;
	d->m_cached  = isCached;
}

FileContainer::~FileContainer()
{
}

File FileContainer::file(QSqlDatabase db) const
{
	d->load(db);
	return d->m_file;
}

void FileContainer::reload(QSqlDatabase db)
{
	if (isValid(db))
	{
		d->m_file.reload(db);
	}
}

bool FileContainer::isValid(QSqlDatabase db) const
{
	d->load(db);
	return d->m_file.isValid();
}

FileContainer & FileContainer::operator=(const FileContainer& file)
{
	this->d = file.d;

	return *this;
}

/* File */

File::File()
{
	d = new PrivateFile;
}

File::File(const File & other) : d(other.d)
{
}

File::File(QSqlDatabase db, uint id)
{
	d = new PrivateFile;
	load(db, id);
}

File::File(QSqlDatabase db, XinxProject * project, const QString & path, bool isCached)
{
	d = new PrivateFile;
	load(db, project, path, isCached);
}

File::~File()
{
}

void File::load(QSqlDatabase db, uint id)
{
	QSqlQuery selectQuery("SELECT project_id, path, cached, type, datmod, loaded, root_id, selection, encoding "
	                      "FROM cv_file WHERE id=:id", db);
	selectQuery.bindValue(":id", QVariant::fromValue(id));
	bool result = selectQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::load", selectQuery.lastError().text());
	EXCEPT_ELSE(selectQuery.first(), FileException, "File::load", tr("Can't find the node %1").arg(id));

	d->m_id        = id;
	d->m_projectId = selectQuery.value(0).toInt();
	d->m_rootId    = selectQuery.value(6).toInt();
	d->m_path      = selectQuery.value(1).toString();
	d->m_type      = selectQuery.value(3).toString();
	d->m_selection = selectQuery.value(7).toString();
	d->m_encoding  = selectQuery.value(8).toString();
	d->m_datmod    = QDateTime::fromString(selectQuery.value(4).toString(), Qt::ISODate);
	d->m_cached    = selectQuery.value(2).toBool();
	d->m_loaded    = selectQuery.value(5).toBool();
}

void File::load(QSqlDatabase db, XinxProject * project, const QString & path, bool isCached)
{
	QSqlQuery projectQuery("SELECT id, name, path FROM cv_project WHERE path=:project_path", db);
	projectQuery.bindValue(":project_path", project ? project->fileName() : "/");
	bool result = projectQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::load", projectQuery.lastError().text());
	EXCEPT_ELSE(projectQuery.first(), FileException, "File::load", tr("Can't find the project %1").arg(project->fileName()));

	d->m_projectId = projectQuery.value(0).toInt();

	QSqlQuery selectQuery("SELECT id, type, datmod, loaded, root_id, selection, encoding "
	                      "FROM cv_file WHERE project_id=:project_id and path=:path and cached=:cached", db);
	selectQuery.bindValue(":project_id", QVariant::fromValue(d->m_projectId));
	selectQuery.bindValue(":path", QVariant::fromValue(path));
	selectQuery.bindValue(":cached", QVariant::fromValue(isCached));
	result = selectQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::load", qPrintable(selectQuery.lastError().text()));
	EXCEPT_ELSE(selectQuery.first(), FileException, "File::load", tr("Can't find the node for path %1").arg(path));

	d->m_path      = path;
	d->m_cached    = isCached;
	d->m_id        = selectQuery.value(0).toInt();
	d->m_rootId    = selectQuery.value(4).toInt();
	d->m_type      = selectQuery.value(1).toString();
	d->m_selection = selectQuery.value(5).toString();
	d->m_encoding  = selectQuery.value(6).toString();
	d->m_datmod    = QDateTime::fromString(selectQuery.value(2).toString(), Qt::ISODate);
	d->m_loaded    = selectQuery.value(3).toBool();
}

void File::reload(QSqlDatabase db)
{
	if (d->m_id >= 0)
		load(db, d->m_id);
}

uint File::create(QSqlDatabase db)
{
	QSqlQuery selectQuery("SELECT 1 FROM cv_project WHERE id=:project_id", db);
	selectQuery.bindValue(":project_id", QVariant::fromValue(d->m_projectId));

	bool result = selectQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::create", qPrintable(selectQuery.lastError().text()));
	EXCEPT_ELSE(selectQuery.first(), FileException, "File::create", tr("Can't find the project %1").arg(d->m_path));

	QSqlQuery insertQuery(db);
	insertQuery.prepare("INSERT INTO cv_file(project_id, path, cached, type, datmod, loaded, root_id, selection, encoding) "
	                    "VALUES(:project_id, :path, :cached, :type, :datmod, :loaded, :root_id, :selection, :encoding)");

	insertQuery.bindValue(":project_id", QVariant::fromValue(d->m_projectId));
	insertQuery.bindValue(":path",       QVariant::fromValue(d->m_path));
	insertQuery.bindValue(":cached",     QVariant::fromValue(d->m_cached));
	insertQuery.bindValue(":type",       QVariant::fromValue(d->m_type));
	insertQuery.bindValue(":datmod",     QVariant::fromValue(d->m_datmod));
	insertQuery.bindValue(":loaded",     QVariant::fromValue(d->m_loaded));
	insertQuery.bindValue(":root_id",    QVariant::fromValue(d->m_rootId));
	insertQuery.bindValue(":selection",  QVariant::fromValue(d->m_selection));
	insertQuery.bindValue(":encoding",   QVariant::fromValue(d->m_encoding));

	result = insertQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::create", qPrintable(insertQuery.lastError().text()));

	uint newId = insertQuery.lastInsertId().toInt();
	d->m_id = newId;
	return newId;
}

void File::update(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "File::update", "The file must be initialized");

	QSqlQuery updateQuery("UPDATE cv_file "
	                      "SET	datmod=:datmod , "
	                      "		loaded=:loaded , "
	                      "		root_id=:root_id , "
	                      "		encoding=:encoding "
	                      "WHERE id=:id", db);
	updateQuery.bindValue(":datmod",   QVariant::fromValue(d->m_datmod));
	updateQuery.bindValue(":loaded",   QVariant::fromValue(d->m_loaded));
	updateQuery.bindValue(":root_id",  QVariant::fromValue(d->m_rootId));
	updateQuery.bindValue(":encoding", QVariant::fromValue(d->m_encoding));
	updateQuery.bindValue(":id",       QVariant::fromValue(d->m_id));

	bool result = updateQuery.exec();
	EXCEPT_ELSE(result, FileException, "Node::update", qPrintable(updateQuery.lastError().text()));
}

void File::destroy(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "File::destroy", "The file must be initialized");
	destroyNodes(db);

	QSqlQuery deleteQuery1("DELETE FROM cv_import WHERE parent_id = :id", db);
	deleteQuery1.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = deleteQuery1.exec();
	EXCEPT_ELSE(result, FileException, "File::destroye", qPrintable(deleteQuery1.lastError().text()));

	QSqlQuery deleteQuery2("DELETE FROM cv_file WHERE id=:id", db);
	deleteQuery2.bindValue(":id", QVariant::fromValue(d->m_id));
	result = deleteQuery2.exec();
	EXCEPT_ELSE(result, FileException, "File::destroy", qPrintable(deleteQuery2.lastError().text()));

	d->m_id = -1;
}

void File::destroyNodes(QSqlDatabase db)
{
	if ((d->m_id == -1) || (d->m_rootId == -1)) return ;

	Node node(db, d->m_rootId);
	node.destroy(db);
	d->m_rootId = -1;
}

uint File::fileId() const
{
	return d->m_id;
}

bool File::isValid() const
{
	return d->m_id >= 0;
}

void File::clear()
{
	d->m_id        = -1;
	d->m_projectId = -1;
	d->m_rootId    = -1;
	d->m_path      = QString();
	d->m_type      = QString();
	d->m_selection = QString();
	d->m_encoding  = QString();
	d->m_datmod    = QDateTime();
	d->m_cached    = false;
	d->m_loaded    = false;
}

void File::addImport(QSqlDatabase db, const File & file, bool automatic)
{
	Q_ASSERT_X(d->m_id >= 0, "File::addImport", "Can't add import on file not created");

	QSqlQuery createQuery("INSERT INTO cv_import( parent_id, child_id, automatic_import ) VALUES( :parentId, :childId, :automatic )", db);
	createQuery.bindValue(":parentId",  QVariant::fromValue(d->m_id));
	createQuery.bindValue(":childId",   QVariant::fromValue(file.fileId()));
	createQuery.bindValue(":automatic", QVariant::fromValue(automatic));
	bool result = createQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::addImport", qPrintable(createQuery.lastError().text()));
}

QList<int> File::imports(QSqlDatabase db)
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select("SELECT id FROM cv_import WHERE parent_id=:id", db);
	select.bindValue(":id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "File::imports", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	return result;
}

void File::destroyImports(QSqlDatabase db)
{
	QSqlQuery q("DELETE FROM cv_import WHERE parent_id=:id", db);
	q.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = q.exec();
	EXCEPT_ELSE(result, FileException, "File::destroyImports", qPrintable(q.lastError().text()));
}

void File::destroyAutomaticImports(QSqlDatabase db)
{
	QSqlQuery q("DELETE FROM cv_import WHERE parent_id=:id and automatic_import=:automatic", db);
	q.bindValue(":id", QVariant::fromValue(d->m_id));
	q.bindValue(":automatic", QVariant::fromValue(true));
	bool result = q.exec();
	EXCEPT_ELSE(result, FileException, "File::destroyAutomaticImports", qPrintable(q.lastError().text()));
}

uint File::projectId() const
{
	return d->m_projectId;
}

void File::setProjectId(uint value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setProjectId", "The project can't be modified after node creation");
	d->m_projectId = value;
}

void File::setProject(const Project & project)
{
	Q_ASSERT_X(d->m_id == -1, "File::setProject", "The project can't be modified after node creation");
	d->m_projectId = project.projectId();
}

const QString & File::path() const
{
	return d->m_path;
}

void File::setPath(const QString & value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setPath", "The path can't be modified after node creation");
	d->m_path = value;
}

bool File::isCached() const
{
	return d->m_cached;
}

void File::setIsCached(bool value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setIsCached", "The cache value can't be modified after node creation");
	d->m_cached = value;
}

const QString & File::type() const
{
	return d->m_type;
}

void File::setType(const QString & value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setProjectId", "The type can't be modified after node creation");
	d->m_type = value;
}

const QDateTime & File::datmod() const
{
	return d->m_datmod;
}

void File::setDatmod(const QDateTime & value)
{
	d->m_datmod = value;
}

bool File::isLoaded() const
{
	return d->m_loaded;
}

void File::setIsLoaded(bool value)
{
	d->m_loaded = value;
}

uint File::rootId() const
{
	return d->m_rootId;
}

void File::setRootId(uint value)
{
	d->m_rootId = value;
}

void File::setRoot(const Node & node)
{
	d->m_rootId = node.nodeId();
}

const QString & File::selection() const
{
	return d->m_selection;
}

void File::setSelection(const QString & value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setProjectId", "The selection can't be modified after node creation");
	d->m_selection = value;
}

const QString & File::encoding() const
{
	return d->m_encoding;
}

void File::setEncoding(const QString & value)
{
	d->m_encoding = value;
}

QList<int> File::nodes(QSqlDatabase db) const
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select("SELECT id FROM cv_node WHERE file_id=:file_id", db);
	select.bindValue(":file_id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "File::nodes", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	return result;
}

File & File::operator=(const File & file)
{
	this->d = file.d;

	return *this;
}


} // namespace ContentView2
