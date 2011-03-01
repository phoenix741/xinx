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
#include "manualfileresolver.h"
#include "project/xinxprojectproject.h"

// Qt header
#include <QDir>

/* ManualFileResolver */

ManualFileResolver::ManualFileResolver()
{
}

ManualFileResolver::~ManualFileResolver()
{
}

QString ManualFileResolver::staticId()
{
	return "manual";
}

QString ManualFileResolver::id()
{
	return staticId();
}

QString ManualFileResolver::name()
{
	return tr("Manual Resolver");
}

QStringList ManualFileResolver::searchPathList(XinxProject::ProjectPtr project) const
{
	QStringList searchList = project->readProperty("searchPathList").toString().split(";;");
	QMutableStringListIterator it(searchList);
	while (it.hasNext())
	{
		QString path = QDir(project->projectPath()).absoluteFilePath(it.next());
		it.setValue(path);
	}
	return searchList;
}

QStringList ManualFileResolver::resolvePath(const QString& path, const QString& currentPath, XinxProject::ProjectPtr project)
{
	QStringList searchList;

	if (! currentPath.isEmpty())
		searchList << currentPath;

	if (project)
	{
		searchList += searchPathList(project);
	}

	QStringList absPaths;

	foreach(const QString & p, searchList)
	{
		absPaths << QDir(p).absoluteFilePath(path);
	}

	return absPaths;
}

bool ManualFileResolver::resolveFileName(const QString& nameToResolve, QString& resolvedName, const QString& currentPath, XinxProject::ProjectPtr project)
{
	QStringList searchList;

	if (! currentPath.isEmpty())
		searchList << currentPath;

	if (project)
	{
		searchList += searchPathList(project);
	}

	QString absPath = QString();
	bool finded = false;
	foreach(const QString & path, searchList)
	{
		absPath = QDir(path).absoluteFilePath(nameToResolve);
		if (QFile::exists(absPath))
		{
			finded = true;
			break;
		}
	}

	if (finded)
	{
		resolvedName = absPath;
	}

	return true;
}
