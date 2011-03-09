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

// Xinx header
#include "xinxprojectmanager.h"
#include "xinxprojectmanager_p.h"

#include <core/xinxconfig.h>
#include <plugins/xinxpluginsloader.h>
#include "xinxprojectprojectexception.h"
#include <session/session.h>
#include <editors/editormanager.h>
#include <application/welcomdlgimpl.h>
#include <project/newprojectwizard.h>
#include "projectpropertyimpl.h"
#include <plugins/xinxpluginelement.h>
#include <actions/actionmanager.h>
#include <plugins/interfaces/gui.h>
#include <plugins/interfaces/files.h>
#include <session/sessionmanager.h>

// Qt header
#include <QApplication>
#include <QProgressDialog>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

/*!
 * \namespace Project
 * \brief Classes to manage project in XINX
 *
 * This namespace group all classes used in project in XINX
 */
namespace XinxProject
{

/* PrivateManager */

PrivateManager::PrivateManager(Manager* parent) : QObject(parent), _new_project_action(0), _recent_project_action(0), _close_project_action(0), _customize_project_action(0), _manager(parent), _recent_separator(0)
{

}

PrivateManager::~PrivateManager()
{
}

void PrivateManager::updateRecentProjects()
{
	int numRecentFiles = qMin(XINXConfig::self()->config().project.recentProjectFiles.size(), MAXRECENTPROJECTS);

	for (int i = 0; i < numRecentFiles; i++)
	{
		QString filename, projectName;
		filename = XINXConfig::self()->config().project.recentProjectFiles[i];
		try
		{
			XinxProject::ProjectPtr project = XinxProject::Project::create(filename);
			projectName = QString("%1 (%2)").arg(project->projectName()).arg(QFileInfo(filename).fileName());
		}
		catch (XinxProject::ProjectException e)
		{
			projectName = QFileInfo(filename).fileName();
		}

		QString text = tr("&%1 %2").arg(i + 1).arg(projectName);
		_recent_actions[i]->setText(text);
		_recent_actions[i]->setData(XINXConfig::self()->config().project.recentProjectFiles[i]);
		_recent_actions[i]->setVisible(true);
	}

	for (int j = numRecentFiles; j < MAXRECENTPROJECTS; j++)
		_recent_actions[j]->setVisible(false);

	_recent_separator->setVisible(numRecentFiles > 0);
}

void PrivateManager::openProjectTriggered()
{
	QString directory = QFileDialog::getExistingDirectory(qApp->activeWindow(), tr("Open a project"), XINXConfig::self()->config().project.defaultPath);
	if (! directory.isEmpty())
	{
		try
		{
			_manager->openProject(directory);
		}
		catch (XinxProject::ProjectException e)
		{
			_manager->newProject(directory);
		}
	}
}

void PrivateManager::openRecentProject()
{
	try
	{
		QAction * action = qobject_cast<QAction*>(sender());
		if (action)
		{
			_manager->openProject(action->data().toString());
		}
	}
	catch (ProjectException e)
	{
		QMessageBox::critical(qApp->activeWindow(), tr("Open project"), tr("Can't open project : %1").arg(e.getMessage()));
	}
}

void PrivateManager::closeProjectTriggered()
{
	_manager->closeProject(_selected_project.toStrongRef());
}

void PrivateManager::customizeProjectTriggered()
{
	_manager->customizeProject(_selected_project.toStrongRef());
}

void PrivateManager::createOpentRecentAction()
{
	QAction * openProjectAct = new QAction(QIcon(":/images/project_open.png"), tr("&Choose file ..."), this);
	openProjectAct->setToolTip(tr("Choose a project file"));
	openProjectAct->setStatusTip(tr("Choose a project file on the disk"));
	connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProjectTriggered()));

	// Recent Project
	QMenu * recentProjectMenu = new QMenu;
	recentProjectMenu->addAction(openProjectAct);
	_recent_project_action->setMenu(recentProjectMenu);

	_recent_separator = recentProjectMenu->addSeparator();
	_recent_separator->setVisible(false);
	for (int i = 0; i < MAXRECENTFILES; i++)
	{
		_recent_actions[i] = new QAction(this);
		_recent_actions[i]->setVisible(false);
		recentProjectMenu->addAction(_recent_actions[i]);
		connect(_recent_actions[i], SIGNAL(triggered()), this, SLOT(openRecentProject()));
	}
}

void PrivateManager::updateActions()
{
	_close_project_action->setEnabled(! _selected_project.isNull());
	_customize_project_action->setEnabled(! _selected_project.isNull());
}

/* Manager */

/*!
 * \class Manager
 * \brief Singleton to manage all the projects
 *
 * The XINX Project Manager is used to manage \e XinxProject file. One project
 * only by process is authorized.
 */

/*!
 * \fn void Manager::changed()
 * \brief Signal emited when the project is changed or when the project emit \e changed()
 */

Manager::Manager()
{
	d = new PrivateManager(this);
}

void Manager::initialisation()
{
	d->m_defaultProject = Project::create();

	// New Project
	d->_new_project_action = new QAction(QIcon(":/images/window_new.png"), tr("&New project ..."), this);
	d->_new_project_action->setToolTip(tr("Create a new project"));
	connect(d->_new_project_action, SIGNAL(triggered()), this, SLOT(newProject()));

	// Open Project
	d->_recent_project_action = new QAction(QIcon(":/images/project_open.png"), tr("&Recent Project ..."), this);
	d->_recent_project_action->setToolTip(tr("Open a project file"));
	connect(d->_recent_project_action, SIGNAL(triggered()), d, SLOT(openProjectTriggered()));
	d->createOpentRecentAction();

	// Close Project
	d->_close_project_action = new QAction(QIcon(":/images/project_close.png"), tr("Close project"), this);
	d->_close_project_action->setToolTip(tr("Close the project file"));
	connect(d->_close_project_action, SIGNAL(triggered()), d, SLOT(closeProjectTriggered()));

	// Close Project
	d->_customize_project_action = new QAction(tr("Project &property"), this);
	d->_customize_project_action->setToolTip(tr("Show/Modify the property of the project"));
	connect(d->_customize_project_action, SIGNAL(triggered()), d, SLOT(customizeProjectTriggered()));

	d->updateActions();
	d->updateRecentProjects();

	connect(this, SIGNAL(projectOpened(XinxProject::ProjectPtr)), d, SLOT(updateRecentProjects()));
}

/*!
 * \brief Destroy the manager
 */
Manager::~Manager()
{
	d->m_defaultProject.clear();;
	d->m_projects.clear();;
}

/*!
 * \brief List of project opened in XINX
 *
 * This method list all project opened by XINX. When user open a project
 * it will be added to the list.
 *
 * If the project mode is in single mode, other project are closed.
 */
const QList<ProjectPtr> & Manager::projects() const
{
	return d->m_projects;
}

ProjectPtr Manager::defaultProject()
{
	return d->m_defaultProject;
}

/*!
 * \brief Search and return the project in which we can find \e filename.
 *
 * Return the project where the file \e filename can be found. If the file isn't in a
 * project of the project list, the default project is returned.
 */
ProjectPtr Manager::projectOfFile(const QString & filename)
{
	const QString cleanFileName = QDir::cleanPath(filename);
	foreach(XinxProject::ProjectPtr project, d->m_projects)
	{
		const QString cleanDirName = QDir::cleanPath(project->projectPath());
		if (cleanFileName.contains(cleanDirName))
			return project;
	}

	return defaultProject();
}

/*!
 * \brief Search and return the project in the path, if the project is present in the path.
 */
ProjectPtr Manager::projectOfPath(const QString & path)
{
	const QString cleanPath = QDir::cleanPath(path);

	foreach(XinxProject::ProjectPtr project, d->m_projects)
	{
		const QString cleanDirName = QDir::cleanPath(project->projectPath());
		if (cleanPath == cleanDirName)
			return project;
	}

	return defaultProject();
}

void Manager::newProject(const QString & directory)
{
	NewProjectWizard wizard(directory);
	if (wizard.exec())
	{
		openProject(wizard.createProject());
	}
}

/*!
 * \brief Open the file directory \e directory as a project
 * \throw XinxProject::ProjectException
 */
void Manager::openProject(const QString & directory)
{
	XINXConfig::self()->config().project.recentProjectFiles.removeAll(directory);

	ProjectPtr project = Project::create(directory);

	openProject(project);
}

ProjectPtr PrivateManager::projectContains(const QString & directory)
{
	foreach(ProjectPtr project, m_projects)
	{
		if (project->projectPath() == QFileInfo(directory).canonicalFilePath())
		{
			return project;
		}
	}

	return ProjectPtr();
}

/*!
 * \brief Open the project \e project
 */
void Manager::openProject(ProjectPtr project)
{
	XINXConfig::self()->config().project.recentProjectFiles.removeAll(project->projectPath());

	// If the project is already opend, we don't repoen it, but we made it the default.
	ProjectPtr oldProject = d->projectContains(project->projectPath());
	if (oldProject)
	{
		setSelectedProject(oldProject.toWeakRef());

		XINXConfig::self()->config().project.recentProjectFiles.prepend(oldProject->projectPath());
		while (XINXConfig::self()->config().project.recentProjectFiles.size() > MAXRECENTPROJECTS)
			XINXConfig::self()->config().project.recentProjectFiles.removeLast();

		return;
	}

	QList<IProjectInitialisationStep*> steps;
	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		if (e->isActivated() && qobject_cast<IXinxInputOutputPlugin*>(e->plugin()))
		{
			steps << qobject_cast<IXinxInputOutputPlugin*>(e->plugin())->loadProjectStep(project);
		}
	}

	QProgressDialog progressDlg(qApp->activeWindow());
	progressDlg.setCancelButton(0);
	progressDlg.setModal(true);
	progressDlg.setWindowTitle(tr("Open the project ..."));
	progressDlg.setMinimumWidth(300);
	progressDlg.setMaximum(steps.size() + 1);
	progressDlg.setValue(0);
	//progressDlg.show();

	if (XINXConfig::self()->config().project.singleProjectMode)
	{
		progressDlg.setMaximum(progressDlg.maximum() + 1);
		progressDlg.setLabelText(tr("Close previous project"));
		qApp->processEvents();

		foreach(ProjectPtr project, d->m_projects)
		{
			closeProject(project, false);
		}

		progressDlg.setValue(progressDlg.value() + 1);
		qApp->processEvents();
	}

	foreach(IProjectInitialisationStep* step, steps)
	{
		progressDlg.setLabelText(step->name());
		qApp->processEvents();
		if (! step->process())
		{
			qWarning() << tr("Can't start a project at the step \"%1\"").arg(step->name());
		}
		progressDlg.setValue(progressDlg.value() + 1);
		qApp->processEvents();

		delete step;
	}

	d->m_projects.append(project);

	// Add the project in recent project
	XINXConfig::self()->config().project.recentProjectFiles.prepend(project->projectPath());
	while (XINXConfig::self()->config().project.recentProjectFiles.size() > MAXRECENTPROJECTS)
		XINXConfig::self()->config().project.recentProjectFiles.removeLast();

	XinxSession::SessionManager::self()->currentSession()->addOpenedProject(project->projectPath());

	progressDlg.setLabelText(tr("Initialisation of the parser job"));
	qApp->processEvents();
	progressDlg.setValue(progressDlg.value() + 1);
	qApp->processEvents();

	connect(project.data(), SIGNAL(changed()), this, SIGNAL(changed()));

	emit changed();
	emit projectOpened(project);

	if (selectedProject() == 0)
	{
		setSelectedProject(project.toWeakRef());
	}
}

void Manager::customizeProject(XinxProject::ProjectPtr project)
{
	ProjectPropertyImpl property(qApp->activeWindow());
	property.loadFromProject(project);

	if (property.exec())
	{
		property.saveToProject(project);
	}

	emit projectCustomized(project);
}

/*!
 * \brief Close the project \e project
 */
bool Manager::closeProject(XinxProject::ProjectPtr project, bool showWelcome)
{
	emit projectClosing(project);

	if (! EditorManager::self()->closeAllFile(project))
	{
		return false;
	}

	if (! XinxSession::SessionManager::self()->isApplicationStopping())
	{
		XinxSession::SessionManager::self()->currentSession()->deleteOpenedProject(project->projectPath());
	}

	d->m_projects.removeAll(project);
	if (project.data() == selectedProject().data())
	{
		setSelectedProject(d->m_projects.size() ? d->m_projects.first().toWeakRef() : ProjectPtrWeak());
	}

	// Update the state of action
	XinxAction::ActionManager::self()->updateMenuItemState();

	emit changed();
	emit projectClosed();

	if (showWelcome && (d->m_projects.size() == 0))
		openWelcomDialog();

	return true;
}

bool Manager::closeAllProject()
{
	foreach(ProjectPtr project, d->m_projects)
	{
		if (! closeProject(project, false))
		{
			return false;
		}
	}
	return true;
}

void Manager::setSelectedProject(XinxProject::ProjectPtrWeak project)
{
	Q_ASSERT_X(! project || d->m_projects.contains(project), "Manager::setSelectedProject", "Project must be opended");

	if (project.data() != d->_selected_project.data())
	{
		d->_selected_project = project;
		d->updateActions();

		emit selectionChanged(d->_selected_project.toStrongRef());
	}
}

ProjectPtrWeak Manager::selectedProject() const
{
	return d->_selected_project;
}

QAction* Manager::newProjectAction() const
{
	return d->_new_project_action;
}

QAction* Manager::openProjectAction() const
{
	return d->_recent_project_action;
}

QAction* Manager::closeProjectAction() const
{
	return d->_close_project_action;
}

QAction * Manager::customizeProjectAction() const
{
	return d->_customize_project_action;
}

void Manager::openWelcomDialog()
{
	WelcomDialogImpl dlg(qApp->activeWindow());

	int numRecentFiles = qMin(XINXConfig::self()->config().project.recentProjectFiles.size(), MAXRECENTFILES);
	for (int i = 0; i < numRecentFiles; i++)
	{
		dlg.addProjectFile(XINXConfig::self()->config().project.recentProjectFiles[i]);
	}

	foreach(const QString & session, XinxSession::Session::sessionsNames())
	{
		dlg.addSession(session);
	}

	connect(&dlg, SIGNAL(createNewProject()), SLOT(newProject()));
	connect(&dlg, SIGNAL(openExistingProject()), d, SLOT(openProjectTriggered()));
	connect(&dlg, SIGNAL(requestProject(QString)), SLOT(openProject(QString)));
	connect(&dlg, SIGNAL(requestSession(QString)), XinxSession::SessionManager::self(), SLOT(restoreSession(QString)));

	dlg.exec();
}

} // namespace XinxProject
