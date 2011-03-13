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
#ifndef XINXPROJECTPROJECT_H
#define XINXPROJECTPROJECT_H

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>
#include <session/session.h>
#include <project/externalfileresolver.h>
#include <versioncontrol/rcsproxy.h>
#include <contentview3/cache.h>

// Qt header
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSharedPointer>

class ExternalFileResolver;
namespace VersionControl
{
class RCSProxy;
}

namespace ContentView3
{
class Cache;
}

namespace XinxProject
{

class PrivateXinxProject;
class Project;

typedef QSharedPointer<Project> ProjectPtr;
typedef QWeakPointer<Project> ProjectPtrWeak;

/* Constante */

#define XINX_PROJECT_VERSION_0 0
#define XINX_PROJECT_VERSION 0

class LIBEXPORT Project : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString projectPath READ projectPath)
	Q_PROPERTY(VersionControl::RCSProxy* rcs READ rcsProxy)

	Q_PROPERTY(QString projectName READ projectName WRITE setProjectName)
	Q_PROPERTY(QString projectRCS READ projectRCS WRITE setProjectRCS)
	Q_PROPERTY(QStringList activatedPlugin READ activatedPlugin WRITE setActivatedPlugin)
public:
	static XinxProject::ProjectPtr create();
	static XinxProject::ProjectPtr create(const QString & path, bool creation = false);
	~Project();

	const QString & projectPath() const;

	QString projectName() const;
	void setProjectName(const QString & value);

	const QString & projectRCS() const;
	void setProjectRCS(const QString & value);

	QStringList activatedPlugin() const;
	void setActivatedPlugin(const QStringList & value);

	QStringList excludedPath() const;
	void setExcludedPath(const QStringList & value);

	QStringList linkedPath() const;
	void setLinkedPath(const QStringList & value);

	Q_INVOKABLE void writeProperty(const QString & key, QVariant value);
	Q_INVOKABLE QVariant readProperty(const QString & key) const;

	ContentView3::Cache * cache();
	ExternalFileResolver * resolver();

	VersionControl::RCSProxy * rcsProxy();

	void addObject(const QString & name, QObject * object);
	void removeObject(const QString & name);
	bool containsObject(const QString & name) const;
	QObject * getObject(const QString & name) const;
signals:
	void changed();
private:
	Project();
	explicit Project(const QString& path);

	PrivateXinxProject * d;
	friend class PrivateXinxProject;
};

} // namespace XinxProject

Q_DECLARE_METATYPE(XinxProject::ProjectPtrWeak);

#endif // XINXPROJECTPROJECT_H
