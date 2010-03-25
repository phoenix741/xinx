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

#ifndef CONTENTVIEW2PROJECT_H
#define CONTENTVIEW2PROJECT_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>

// Qt header
#include <QApplication>
#include <QSqlDatabase>
#include <QExplicitlySharedDataPointer>

class XinxProject;

namespace ContentView2
{

/*!
 * \class FileException
 * \brief Exception throw when a SQL error occur
 */
class LIBEXPORT ProjectException : public XinxException
{
public:
	/*!
	 * Create the exception with a message and a line.
	 * \param message Error of the exception.
	 * \param line Line where the error is.
	 */
	ProjectException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message);
private:
};

class PrivateProject;

/*!
 * The class Project is usefull to simplify some operations on cv_project table.
 * This class is also used to be propaged from object to another.
 */
class LIBEXPORT Project
{
	Q_DECLARE_TR_FUNCTIONS(Project)
public:
	Project();
	Project(const Project & other);
	Project(QSqlDatabase db, uint id);
	Project(QSqlDatabase db, XinxProject * project);
	~Project();

	void load(QSqlDatabase db, uint id);
	void load(QSqlDatabase db, XinxProject * project);
	void reload(QSqlDatabase db);
	int create(QSqlDatabase db);
	void update(QSqlDatabase db);
	void destroy(QSqlDatabase db);
	void destroyFiles(QSqlDatabase db);

	uint projectId() const;
	bool isValid() const;
	void clear();

	void setProjectName(const QString & value);
	const QString & projectName() const;

	void setProjectPath(const QString & value);
	const QString & projectPath() const;

	QList<int> files(QSqlDatabase db) const;

	Project & operator=(const Project & project);
private:
	QExplicitlySharedDataPointer<PrivateProject> d;
};

} // namespace ContentView2

#endif // CONTENTVIEW2PROJECT_H
