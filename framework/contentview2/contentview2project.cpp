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
#include "contentview2project.h"
#include "contentview2file.h"
#include "project/xinxproject.h"

// Qt header
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

namespace ContentView2
{

/* ProjectException */
/*!
 * \class ProjectException
 * \brief Exception throw when a SQL error occur
 */

/*!
 * Create the exception with a message and a line.
 * \param assertion The condition who failed
 * \param locationFile The file wich failed (this file)
 * \param locationLine The line where the exception is throw
 * \param locationMethod The method where the exception is throw.
 * \param message Error of the exception.
 * \param message Error of the exception.
 */
ProjectException::ProjectException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message)
	: XinxException(assertion, locationFile, locationLine, locationMethod, message)
{
}

/* PrivateProject */

class PrivateProject : public QSharedData
{
public:
	long m_id;
	QString m_path, m_name;

	PrivateProject();
	PrivateProject(const PrivateProject &other);
	~PrivateProject();
};

PrivateProject::PrivateProject() : m_id(-1)
{
}

PrivateProject::PrivateProject(const PrivateProject &other) : QSharedData(other), m_id(other.m_id), m_path(other.m_path), m_name(other.m_name)
{
}

PrivateProject::~PrivateProject()
{

}

/* Project */

/*!
 * \class Project
 * \brief This class represent a project in the session database.
 *
 * The class Project is usefull to simplify some operations on cv_project table.
 * This class is also used to be propaged from object to another.
 */

Project::Project()
{
	d = new PrivateProject;
}

Project::Project(const Project & other) : d(other.d)
{
}

Project::Project(QSqlDatabase db, uint id)
{
	d = new PrivateProject;
	load(db, id);
}

Project::Project(QSqlDatabase db, XinxProject * project)
{
	d = new PrivateProject;
	load(db, project);
}

Project::~Project()
{
}

void Project::load(QSqlDatabase db, uint id)
{
	QSqlQuery selectQuery("SELECT path, name FROM cv_project WHERE id=:id", db);
	selectQuery.bindValue(":id", QVariant::fromValue(id));
	bool result = selectQuery.exec();
	EXCEPT_ELSE(result, ProjectException, "Project::load", selectQuery.lastError().text());
	EXCEPT_ELSE(selectQuery.first(), ProjectException, "Project::Load", tr("Can't find the node %1").arg(id));

	d->m_id   = id;
	d->m_path = selectQuery.value(0).toString();
	d->m_name = selectQuery.value(1).toString();
}

void Project::load(QSqlDatabase db, XinxProject * project)
{
	d->m_path = project ? project->fileName() : "/";

	QSqlQuery selectQuery("SELECT id, name FROM cv_project WHERE path=:path", db);
	selectQuery.bindValue(":path", QVariant::fromValue(d->m_path));
	bool result = selectQuery.exec();
	EXCEPT_ELSE(result, ProjectException, "Project::load", selectQuery.lastError().text());
	EXCEPT_ELSE(selectQuery.first(), ProjectException, "Project::load", tr("Can't find the project %1").arg(d->m_path));

	d->m_id   = selectQuery.value(0).toInt();
	d->m_name = selectQuery.value(1).toString();
}

void Project::reload(QSqlDatabase db)
{
	if (d->m_id >= 0)
		load(db, d->m_id);
}

int Project::create(QSqlDatabase db)
{
	QSqlQuery insertQuery(db);
	insertQuery.prepare("INSERT INTO cv_project(path, name) VALUES(:path, :name)");
	insertQuery.bindValue(":path",       d->m_path);
	insertQuery.bindValue(":name",       d->m_name);

	bool result = insertQuery.exec();
	EXCEPT_ELSE(result, ProjectException, "Project::create", qPrintable(insertQuery.lastError().text()));

	uint newId = insertQuery.lastInsertId().toInt();
	d->m_id = newId;
	return newId;
}

void Project::update(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "Project::update", "The project must be initialized");

	QSqlQuery updateQuery("UPDATE cv_project SET name=:name WHERE id=:id", db);
	updateQuery.bindValue(":name", QVariant::fromValue(d->m_name));
	updateQuery.bindValue(":id",   QVariant::fromValue(d->m_id));

	bool result = updateQuery.exec();
	EXCEPT_ELSE(result, ProjectException, "Project::update", qPrintable(updateQuery.lastError().text()));
}

void Project::destroy(QSqlDatabase db)
{
	Q_ASSERT_X(d->m_id >= 0, "Project::destroy", "The project must be initialized");

	destroyFiles(db);

	QSqlQuery deleteQuery1("DELETE FROM cv_project WHERE id=:id", db);
	deleteQuery1.bindValue(":id", QVariant::fromValue(d->m_id));
	bool result = deleteQuery1.exec();
	EXCEPT_ELSE(result, ProjectException, "Project::destroy", qPrintable(deleteQuery1.lastError().text()));
}

void Project::destroyFiles(QSqlDatabase db)
{
	if (d->m_id == -1) return ;

	foreach(int id, files(db))
	{
		File file(db, id);
		file.destroy(db);
	}
}

uint Project::projectId() const
{
	return d->m_id;
}

bool Project::isValid() const
{
	return d->m_id >= 0;
}

void Project::clear()
{
	d->m_path     = -1;
	d->m_name     = -1;
	d->m_id       = -1;
}

void Project::setProjectName(const QString & value)
{
	d->m_name = value;
}

const QString & Project::projectName() const
{
	return d->m_name;
}

void Project::setProjectPath(const QString & value)
{
	Q_ASSERT_X(d->m_id == -1, "Project::setProjectPath", "The path can't be modified after node creation");
	d->m_path = value;
}

const QString & Project::projectPath() const
{
	return d->m_path;
}

QList<int> Project::files(QSqlDatabase db) const
{
	QList<int> result;
	if (d->m_id == -1) return result;

	QSqlQuery select("SELECT id FROM cv_file WHERE project_id=:project_id", db);
	select.bindValue(":project_id", QVariant::fromValue(d->m_id));
	bool r = select.exec();
	Q_ASSERT_X(r, "Project::files", qPrintable(select.lastError().text()));

	while (select.next())
	{
		result += select.value(0).toInt();
	}

	return result;
}

Project & Project::operator=(const Project & project)
{
	this->d = project.d;

	return *this;
}

} // namespace ContentView2
