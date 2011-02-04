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
#ifndef XINXPROJECTMANAGER_P_H
#define XINXPROJECTMANAGER_P_H

#include <QObject>
#include <QAction>
#include <project/xinxprojectproject.h>

namespace XinxProject {

class Manager;

class LIBEXPORT PrivateManager : public QObject
{
	Q_OBJECT
public:
    PrivateManager(XinxProject::Manager* parent);
    virtual ~PrivateManager();

	QAction * _new_project_action, * _recent_project_action, * _close_project_action, * _customize_project_action;
	Project * _selected_project;
	QList<Project*> m_projects;
	Project * m_defaultProject;
    XinxProject::Manager* _manager;
    QAction * _recent_separator;
	QAction* _recent_actions[ MAXRECENTFILES ];

	void initialisation();
	void createOpentRecentAction();
	void updateActions();
	Project * projectContains(const QString & directory);

public slots:
	void openProjectTriggered();
	void closeProjectTriggered();
	void customizeProjectTriggered();
	void openRecentProject();
	void updateRecentProjects();
};

} // namespace XinxProject

#endif // XINXPROJECTMANAGER_P_H
