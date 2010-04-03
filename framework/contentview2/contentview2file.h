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

#ifndef CONTENTVIEW2FILE_H
#define CONTENTVIEW2FILE_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>

// Qt header
#include <QApplication>
#include <QSqlDatabase>
#include <QExplicitlySharedDataPointer>
#include <QDateTime>
#include <QPointer>

class XinxProject;

namespace ContentView2
{

class LIBEXPORT FileException : public XinxException
{
public:
	FileException(const QString & assertion, const QString & locationFile, int locationLine, const QString & locationMethod, QString message);
private:
};

class PrivateFile;
class PrivateFileContainer;
class Node;
class Project;
class File;


class LIBEXPORT FileContainer
{
public:
	FileContainer();
	FileContainer(const FileContainer & other);
	FileContainer(QPointer<XinxProject> project, const QString & path, bool isCached);
	~FileContainer();

	XinxProject * project() const;
	const QString & path() const;
	bool isCached() const;

	File file(QSqlDatabase db) const;
	void reload(QSqlDatabase db);
	bool isValid(QSqlDatabase db) const;

	FileContainer & operator=(const FileContainer& file);
private:
	QExplicitlySharedDataPointer<PrivateFileContainer> d;
};

/*!
 * The class file is usefull to simplify some operations on cv_file table.
 * This class is also used to be propaged from object to another.
 */
class LIBEXPORT File
{
	Q_DECLARE_TR_FUNCTIONS(File)
public:
	File();
	File(const File & other);
	File(QSqlDatabase db, uint id);
	File(QSqlDatabase db, XinxProject * project, const QString & path, bool isCached);
	~File();

	void load(QSqlDatabase db, uint id);
	void load(QSqlDatabase db, XinxProject * project, const QString & path, bool isCached);
	void reload(QSqlDatabase db);
	uint create(QSqlDatabase db);
	void update(QSqlDatabase db);
	void destroy(QSqlDatabase db);
	void destroyNodes(QSqlDatabase db);

	uint fileId() const;
	bool isValid() const;
	void clear();

	uint projectId() const;
	void setProjectId(uint value);
	void setProject(const Project & project);

	void addImport(QSqlDatabase db, const File & file, bool automatic);
	QList<int> imports(QSqlDatabase db);
	void destroyImports(QSqlDatabase db);
	void destroyAutomaticImports(QSqlDatabase db);

	const QString & path() const;
	void setPath(const QString & value);

	bool isCached() const;
	void setIsCached(bool value);

	const QString & type() const;
	void setType(const QString & value);

	const QDateTime & datmod() const;
	void setDatmod(const QDateTime & value);

	bool isLoaded() const;
	void setIsLoaded(bool value);

	uint rootId() const;
	void setRootId(uint value);
	void setRoot(const Node & node);

	const QString & selection() const;
	void setSelection(const QString & value);

	const QString & encoding() const;
	void setEncoding(const QString & value);

	QList<int> nodes(QSqlDatabase db) const;

	File & operator=(const File & file);
private:
	QExplicitlySharedDataPointer<PrivateFile> d;
};

} // namespace ContentView2

#endif // CONTENTVIEW2FILE_H
