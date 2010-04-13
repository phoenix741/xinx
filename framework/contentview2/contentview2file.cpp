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
/*!
 * \ingroup ContentView2
 * \class FileException
 * \since 0.9.0.0
 *
 * \brief Exception throw when a SQL error occur on the File class.
 *
 * This class can be used by EXCEPT_ELSE() macro
 */

/*!
 * Create an exception
 * \param assertion The condition who failed
 * \param locationFile The file wich failed (this file)
 * \param locationLine The line where the exception is throw
 * \param locationMethod The method where the exception is throw.
 * \param message Error of the exception.
 */
FileException::FileException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message)
		: DatabaseException(assertion, locationFile, locationLine, locationMethod, message)
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

/*!
 * \ingroup ContentView2
 * \class FileContainer
 * \since 0.9.0.0
 *
 * \brief This class is used to wrap a File and control the creation of the file.
 *
 * A file is represented by the tuple (XinxProject, path, isCached). This object
 * is used to pass this three argumentes to the differents object, and facilitate
 * the control of existance of the file in the table cv_file.
 *
 * Before use this class, check the method isValid() to determine if the file is
 * created.
 */

//! Create an empty file container
FileContainer::FileContainer()
{
	d = new PrivateFileContainer;
}

//! Create a new file container as a copy of \p other
FileContainer::FileContainer(const FileContainer & other) : d(other.d)
{
}

//! Create a container for the project \p project, file \p path, and the parameter \p isCached
FileContainer::FileContainer(QPointer<XinxProject> project, const QString & path, bool isCached)
{
	d = new PrivateFileContainer;
	d->m_project = project;
	d->m_path    = path;
	d->m_cached  = isCached;
}

//! Destroy the file container
FileContainer::~FileContainer()
{
}

//! Return the project passed to the constructor
XinxProject * FileContainer::project() const
{
	return d->m_project;
}

//! Return the path passed to the constructor
const QString & FileContainer::path() const
{
	return d->m_path;
}

//! Return the indicator isCached passed to the constructor
bool FileContainer::isCached() const
{
	return d->m_cached;
}

/*!
 * \brief Return the file defined in this container if exist in cv_file.
 *
 * If exist in cv_file this class return the File for value passed in the
 * constructor. else this method return an empty file.
 */
File FileContainer::file(QSqlDatabase db) const
{
	d->load(db);
	return d->m_file;
}

//! Reload the file defined in the FileContainer
void FileContainer::reload(QSqlDatabase db)
{
	if (isValid(db))
	{
		d->m_file.reload(db);
	}
}

/*!
 * \brief Return the validity of the file.
 *
 * If the file exist in cv_file, this method return true. Else
 * this method return false.
 */
bool FileContainer::isValid(QSqlDatabase db) const
{
	d->load(db);
	return d->m_file.isValid();
}

//! Copy the content from the FileContainer \p file.
FileContainer & FileContainer::operator=(const FileContainer& file)
{
	this->d = file.d;

	return *this;
}

/* File */

/*!
 * \ingroup ContentView2
 * \class File
 * \since 0.9.0.0
 *
 * \brief The class file is usefull to simplify some operations on cv_file table.
 * This class is also used to be propaged from object to another.
 */

//! Create an empty file
File::File()
{
	d = new PrivateFile;
}

//! Copy the empty file from \p other
File::File(const File & other) : d(other.d)
{
}

//! Get the file where the id are equals to \p id
File::File(QSqlDatabase db, uint id)
{
	d = new PrivateFile;
	load(db, id);
}

//! Find the file where the project is \p project, the path \p path, and the cache indicator is \p isCached
File::File(QSqlDatabase db, XinxProject * project, const QString & path, bool isCached)
{
	d = new PrivateFile;
	load(db, project, path, isCached);
}

//! Destroy this file
File::~File()
{
}

/*!
 * \brief Load the file with the id \p id
 */
void File::load(QSqlDatabase db, uint id)
{
	QSqlQuery selectQuery = Manager::self()->getSqlQuery(
						"SELECT project_id, path, cached, type, datmod, loaded, root_id, selection, encoding "
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

/*!
 * \brief Load The file with the project \p project, the path \p path, and the indicator \p isCached.
 */
void File::load(QSqlDatabase db, XinxProject * project, const QString & path, bool isCached)
{
	QSqlQuery projectQuery = Manager::self()->getSqlQuery("SELECT id, name, path FROM cv_project WHERE path=:project_path", db);
	projectQuery.bindValue(":project_path", project ? project->fileName() : "/");
	bool result = projectQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::load", projectQuery.lastError().text());
	EXCEPT_ELSE(projectQuery.first(), FileException, "File::load", tr("Can't find the project %1").arg(project->fileName()));

	d->m_projectId = projectQuery.value(0).toInt();

	QSqlQuery selectQuery = Manager::self()->getSqlQuery(
						"SELECT id, type, datmod, loaded, root_id, selection, encoding "
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

//! Reload the content of the file
void File::reload(QSqlDatabase db)
{
	if (d->m_id >= 0)
		load(db, d->m_id);
}

/*!
 * \brief Create the file in the table cv_file with information
 * set in this object.
 *
 * After the creation in the database, the id will be stored and
 * returned
 */
uint File::create(QSqlDatabase db)
{
	QSqlQuery selectQuery = Manager::self()->getSqlQuery("SELECT 1 FROM cv_project WHERE id=:project_id", db);
	selectQuery.bindValue(":project_id", QVariant::fromValue(d->m_projectId));

	bool result = selectQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::create", qPrintable(selectQuery.lastError().text()));
	EXCEPT_ELSE(selectQuery.first(), FileException, "File::create", tr("Can't find the project %1").arg(d->m_path));

	QSqlQuery insertQuery = Manager::self()->getSqlQuery(
						"INSERT INTO cv_file(project_id, path, cached, type, datmod, loaded, root_id, selection, encoding) "
						"VALUES(:project_id, :path, :cached, :type, :datmod, :loaded, :root_id, :selection, :encoding)", db);

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

//! Update the database with the content of the object
void File::update(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "File::update", "The file must be initialized");

	QSqlQuery updateQuery = Manager::self()->getSqlQuery(
							"UPDATE cv_file "
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

//! Destroy the file in cv_file (and links in cv_file, cv_node, cv_link and cv_import)
void File::destroy(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "File::destroy", "The file must be initialized");
	destroyNodes(db);

	QSqlQuery deleteQuery1 = Manager::self()->getSqlQuery("DELETE FROM cv_import WHERE parent_id = :id", db);
	deleteQuery1.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = deleteQuery1.exec();
	EXCEPT_ELSE(result, FileException, "File::destroye", qPrintable(deleteQuery1.lastError().text()));

	QSqlQuery deleteQuery2 = Manager::self()->getSqlQuery("DELETE FROM cv_file WHERE id=:id", db);
	deleteQuery2.bindValue(":id", QVariant::fromValue(d->m_id));
	result = deleteQuery2.exec();
	EXCEPT_ELSE(result, FileException, "File::destroy", qPrintable(deleteQuery2.lastError().text()));

	d->m_id = -1;
}

//! Destroy attached nodes.
void File::destroyNodes(QSqlDatabase db)
{
	if ((d->m_id == -1) || (d->m_rootId == -1)) return ;

	Node node(db, d->m_rootId);
	node.destroy(db);
	d->m_rootId = -1;
}

//! Return the file id (-1 if the file isn't loaded or created)
uint File::fileId() const
{
	return d->m_id;
}

//! Return true is the id >= 0
bool File::isValid() const
{
	return d->m_id >= 0;
}

//! Clear the content of the object (don't destroy the file)
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

/*!
 * \brief Add the import \p file to this object
 *
 * Manual node are declared by the Parser. The automatic nodes
 * are created by the Cache.
 */
void File::addImport(QSqlDatabase db, const File & file, bool automatic)
{
	Q_ASSERT_X(d->m_id >= 0, "File::addImport", "Can't add import on file not created");

	QSqlQuery createQuery = Manager::self()->getSqlQuery("INSERT INTO cv_import( parent_id, child_id, automatic_import ) VALUES( :parentId, :childId, :automatic )", db);
	createQuery.bindValue(":parentId",  QVariant::fromValue(d->m_id));
	createQuery.bindValue(":childId",   QVariant::fromValue(file.fileId()));
	createQuery.bindValue(":automatic", QVariant::fromValue(automatic));
	bool result = createQuery.exec();
	EXCEPT_ELSE(result, FileException, "File::addImport", qPrintable(createQuery.lastError().text()));
}

//! List of imports attached to this node.
QList<int> File::imports(QSqlDatabase db)
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select = Manager::self()->getSqlQuery("SELECT id FROM cv_import WHERE parent_id=:id", db);
	select.bindValue(":id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "File::imports", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	return result;
}

/*!
 * \brief Destroy all imports attached to the node.
 * \sa destroyAutomaticImports()
 */
void File::destroyImports(QSqlDatabase db)
{
	QSqlQuery q = Manager::self()->getSqlQuery("DELETE FROM cv_import WHERE parent_id=:id", db);
	q.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = q.exec();
	EXCEPT_ELSE(result, FileException, "File::destroyImports", qPrintable(q.lastError().text()));
}

/*!
 * \brief Destroy all imports mark as automatic (added by the Parser class)
 * \sa destroyImports()
 */
void File::destroyAutomaticImports(QSqlDatabase db)
{
	QSqlQuery q = Manager::self()->getSqlQuery("DELETE FROM cv_import WHERE parent_id=:id and automatic_import=:automatic", db);
	q.bindValue(":id", QVariant::fromValue(d->m_id));
	q.bindValue(":automatic", QVariant::fromValue(true));
	bool result = q.exec();
	EXCEPT_ELSE(result, FileException, "File::destroyAutomaticImports", qPrintable(q.lastError().text()));
}

/*!
 * \brief Return the project id of the node.
 * \sa setProjectId(), setProject()
 */
uint File::projectId() const
{
	return d->m_projectId;
}

/*!
 * \brief Set the project id of the node to \p value.
 *
 * If the file is loaded or created, the value can't be modified.
 * \sa setProject(), projectId()
 */
void File::setProjectId(uint value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setProjectId", "The project can't be modified after node creation");
	d->m_projectId = value;
}

/*!
 * \brief Set the project to \p project
 *
 * If the file is loaded or created, the project can't be modified.
 * \sa setProjectId(), projectId()
 */
void File::setProject(const Project & project)
{
	Q_ASSERT_X(d->m_id == -1, "File::setProject", "The project can't be modified after node creation");
	d->m_projectId = project.projectId();
}

/*!
 * \brief Return the path of the file
 * \sa setPath()
 */
const QString & File::path() const
{
	return d->m_path;
}

/*!
 * \brief Set the path for the file
 *
 * If the file is loaded or created the value can't be modified.
 * \sa path()
 */
void File::setPath(const QString & value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setPath", "The path can't be modified after node creation");
	d->m_path = value;
}

/*!
 * \brief Return the isCached indicator.
 * \sa setIsCached()
 */
bool File::isCached() const
{
	return d->m_cached;
}

/*!
 * \brief Set the isCached indicator
 *
 * If the file is loaded or created the value can't be modified.
 * \sa isCached()
 */
void File::setIsCached(bool value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setIsCached", "The cache value can't be modified after node creation");
	d->m_cached = value;
}

/*!
 * \brief Return the type of the file (used to find the parser).
 * \sa setType()
 */
const QString & File::type() const
{
	return d->m_type;
}

/*!
 * \brief Set the type of the file (used to find the parser)
 *
 * If the file is loaded or created the value can't be modified.
 * \sa type()
 */
void File::setType(const QString & value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setType", "The type can't be modified after node creation");
	d->m_type = value;
}

/*!
 * \brief Return the last Modification date of the file
 * \sa setDatmod()
 */
const QDateTime & File::datmod() const
{
	return d->m_datmod;
}

/*!
 * \brief Set the last modification date to \p value
 * \sa datmod()
 */
void File::setDatmod(const QDateTime & value)
{
	d->m_datmod = value;
}

/*!
 * \brief Return the IsLoaded indicator.
 * \sa setIsLoaded()
 */
bool File::isLoaded() const
{
	return d->m_loaded;
}

/*!
 * \brief Set the isLoaded indicator to \p value
 * \sa isLoaded()
 */
void File::setIsLoaded(bool value)
{
	d->m_loaded = value;
}

/*!
 * \brief Return the first node of the file (root node).
 * \sa setRootId(), setRoot()
 */
uint File::rootId() const
{
	return d->m_rootId;
}

/*!
 * \brief Set the first node of the file (root node)
 * \sa rootId(), setRoot()
 */
void File::setRootId(uint value)
{
	d->m_rootId = value;
}

/*!
 * \brief Set the first node of the file.
 *
 * This method is provide for convinience
 * \sa setRootId(), rootId()
 */
void File::setRoot(const Node & node)
{
	d->m_rootId = node.nodeId();
}

/*!
 * \brief Set the selection mode of the file ('M' or '*')
 * \sa setSelection()
 */
const QString & File::selection() const
{
	return d->m_selection;
}

/*!
 * \brief Set the selection mode of the file.
 *
 * If the selection is 'M' the file must be added in completion manually.
 * If the selection is '*' the file is include automatically in the completion.
 *
 * This value can't be modified if the file is loaded or created
 * \sa selection()
 */
void File::setSelection(const QString & value)
{
	Q_ASSERT_X(d->m_id == -1, "File::setProjectId", "The selection can't be modified after node creation");
	d->m_selection = value;
}

/*!
 * \brief Return the encoding finded by the parser.
 * \sa setEncoding()
 */
const QString & File::encoding() const
{
	return d->m_encoding;
}

/*!
 * \brief Set the encoding finded by the parser
 * \sa encoding()
 */
void File::setEncoding(const QString & value)
{
	d->m_encoding = value;
}

//! Return all the attached nodes for the file
QList<int> File::nodes(QSqlDatabase db) const
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select = Manager::self()->getSqlQuery("SELECT id FROM cv_node WHERE file_id=:file_id", db);
	select.bindValue(":file_id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "File::nodes", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	return result;
}

//! Copy the File \p file to self.
File & File::operator=(const File & file)
{
	this->d = file.d;

	return *this;
}


} // namespace ContentView2
