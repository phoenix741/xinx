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

#pragma once
#ifndef CONTENTVIEW3_FILE_H
#define CONTENTVIEW3_FILE_H

// Xinx header
#include <core/lib-config.h>
#include <contentview3/definitions.h>

// Qt header
#include <QApplication>
#include <QScopedPointer>

namespace XinxProject {
	class Project;
}

namespace ContentView3
{

class PrivateFile;

class LIBEXPORT File
{
	Q_DECLARE_TR_FUNCTIONS(File)
public:
	class Symbole
	{
	public:
		QString type, name;
	};

	static FilePtr create(XinxProject::Project * project = 0);
	static FilePtr create(const QString & filename, XinxProject::Project * project = 0);
	~File();

	void setFilename(const QString & filename);
	const QString & filename() const;

	void setProject(XinxProject::Project * project);
	XinxProject::Project * project() const;

	const QStringList & imports() const;
	void clearImports();
	void addImport(const QString & import);

	const QList<Symbole> & symboles() const;
	void clearSymboles();
	void addSymbole(const Symbole & symbole);

	void setRootNode(FileNodePtr node);
	FileNodePtr rootNode() const;
private:
	explicit File(XinxProject::Project * project = 0);
	explicit File(const QString & filename, XinxProject::Project * project = 0);

	QScopedPointer<PrivateFile> d;
};

}

#endif // CONTENTVIEW3_FILE_H
