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
#ifndef XINXPROJECTMANAGER_H
#define XINXPROJECTMANAGER_H

// Xinx header
#include <core/lib-config.h>
#include <core/xinxsingleton.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QPointer>

class QAction;

namespace XinxProject
{

class PrivateManager;

class LIBEXPORT Manager : public XinxLibSingleton<Manager>
{
	Q_OBJECT
public:
	virtual ~Manager();

	const QList<ProjectPtr> & projects() const;
	ProjectPtr defaultProject();
	ProjectPtr projectOfFile(const QString & filename);
	ProjectPtr projectOfPath(const QString & path);

	void setSelectedProject(const QString & projectPath);
	void setSelectedProject(XinxProject::ProjectPtrWeak project);
	ProjectPtrWeak selectedProject() const;

	QAction * newProjectAction() const;
	QAction * openProjectAction() const;
	QAction * closeProjectAction() const;
	QAction * customizeProjectAction() const;
public slots:
	void newProject(const QString & directory = QString());
	void openProject(const QString & directory);
	void openProject(ProjectPtr project);
	void customizeProject(ProjectPtr project);
	bool closeProject(ProjectPtr project, bool showWelcome = true);
	bool closeAllProject();
	void removeProject(const QString & directory);

	void openWelcomDialog();
signals:
	void projectOpened(XinxProject::ProjectPtr project);
	void projectClosing(XinxProject::ProjectPtr project);
	void projectClosed();
	void projectCustomized(XinxProject::ProjectPtr project);
	void changed();
	void selectionChanged(XinxProject::ProjectPtr project);
protected:
	virtual void initialisation();
private:
	Manager();

	PrivateManager * d;
	friend class PrivateManager;
	friend class XinxLibSingleton<Manager>;
};

} // namespace XinxProject

#endif // XINXPROJECTMANAGER_H
