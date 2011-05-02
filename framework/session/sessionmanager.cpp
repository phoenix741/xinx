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
#include "sessionmanager_p.h"
#include "session.h"
#include <project/xinxprojectmanager.h>
#include <project/xinxprojectprojectexception.h>
#include <editors/editormanager.h>
#include <QInputDialog>
#include <QMessageBox>
#include <core/xinxconfig.h>

namespace XinxSession
{

/* PrivateSessionManager */

PrivateSessionManager::PrivateSessionManager(SessionManager * session_manager) : _session_manager(session_manager)
{
	_session_actions = new ActionGroup(_session_manager);
	_is_application_closing = false;
	_signal_mapper = new QSignalMapper(this);

	connect(_signal_mapper, SIGNAL(mapped(QString)), session_manager, SLOT(restoreSession(QString)));
}

void PrivateSessionManager::updateSessions()
{
	QStringList keys = Session::sessionsNames();

	foreach(QAction * action, _session_actions->actions())
	{
		_session_actions->removeAction(action);
		action->deleteLater();
	}

	foreach(const QString & key, keys)
	{
		QAction * action = _session_actions->addAction(key);
		action->setCheckable(true);
		if (key == _current_session->sessionName())
		{
			action->setChecked(true);
		}

		connect(action, SIGNAL(triggered()), _signal_mapper, SLOT(map()));
		_signal_mapper->setMapping(action, key);
	}
}

void PrivateSessionManager::newSessionTrigger()
{
	QString session_name = QInputDialog::getText(qApp->activeWindow(), tr("Create a new session"), tr("Name of the new session"));
	if (! session_name.isEmpty())
	{
		if (Session::sessionsNames().contains(session_name) || (session_name == RECOVER_SESSION))
		{
			QMessageBox::critical(qApp->activeWindow(), tr("Create a new session"), tr("The name %1 is already used by another session").arg(session_name));
			newSessionTrigger();
			return;
		}

		_session_manager->restoreSession(session_name);
	}
}

void PrivateSessionManager::renameSessionTrigger()
{
	QString session_name = QInputDialog::getText(qApp->activeWindow(), tr("Rename session"), tr("New name of the session"), QLineEdit::Normal, _current_session->sessionName());
	if (! session_name.isEmpty())
	{
		if (Session::sessionsNames().contains(session_name) || (session_name == RECOVER_SESSION))
		{
			QMessageBox::critical(qApp->activeWindow(), tr("Rename session"), tr("The name %1 is already used by another session").arg(session_name));
			renameSessionTrigger();
			return;
		}

		_session_manager->renameSession(session_name);
	}
}

void PrivateSessionManager::removeSessionTrigger()
{
	QMessageBox::StandardButton result = QMessageBox::question(qApp->activeWindow(), tr("Delete a session"), tr("Are you sure to delete the session %1 ?").arg(_current_session->sessionName()), QMessageBox::Yes | QMessageBox::No);

	if (result == QMessageBox::Yes)
	{
		_session_manager->deleteSession();
	}
}

/* Session */

/*!
 * \ingroup XinxSession
 * \class SessionManager
 * \brief This class is used to store tempory information.
 * \since 0.9.1.0
 *
 * A session represent a file with the opened project, opened files,
 * last opened project and last opened files.
 *
 * This session will be stored in XINX config file (so a file or in the registery)
 */


/*!
 * \brief Create an empty session
 * \throw XinxSession::ProjectException When the application can't read the session file.
 */
SessionManager::SessionManager() : d(new PrivateSessionManager(this))
{

}

/*!
 * \brief Destroy the session
 */
SessionManager::~SessionManager()
{

}

void SessionManager::initialisation()
{
	// Create default session
	d->_current_session = new Session(this);
	d->updateSessions();

	// New Session
	d->_new_session_action = new QAction(QIcon(":/images/window_new.png"), tr("&New session ..."), this);
	d->_new_session_action->setToolTip(tr("Create a new session (list of project)"));
	connect(d->_new_session_action, SIGNAL(triggered()), d.data(), SLOT(newSessionTrigger()));

	// Remove session
	d->_remove_session_action = new QAction(QIcon(":/images/editdelete.png"), tr("&Remove session ..."), this);
	d->_remove_session_action->setToolTip(tr("Remove the current session"));
	connect(d->_remove_session_action, SIGNAL(triggered()), d.data(), SLOT(removeSessionTrigger()));

	// Rename the session
	d->_rename_session_action = new QAction(tr("Rename session ..."), this);
	d->_rename_session_action->setToolTip(tr("Change the name of the current session"));
	connect(d->_rename_session_action, SIGNAL(triggered()), d.data(), SLOT(renameSessionTrigger()));
}

Session * SessionManager::currentSession() const
{
	return d->_current_session;
}

QAction * SessionManager::newSessionAction() const
{
	return d->_new_session_action;
}

QAction * SessionManager::removeSessionAction() const
{
	return d->_remove_session_action;
}

QAction * SessionManager::renameSessionAction() const
{
	return d->_rename_session_action;
}

ActionGroup * SessionManager::sessionActions() const
{
	return d->_session_actions;
}

void SessionManager::setApplicationStopping(bool value)
{
	d->_is_application_closing = value;
	d->_current_session->setSessionUpdatable(!value);
}

bool SessionManager::isApplicationStopping() const
{
	return d->_is_application_closing;
}

void SessionManager::restoreSession(const QString & sessionName)
{
	/* Backup the current session */
	if (d->_current_session->sessionName() != sessionName)
	{
		saveSession();
	}

	/* Close All */
	d->_current_session->setSessionUpdatable(false);
	if (! EditorManager::self()->closeAllFile())
	{
		return;
	}

	XinxProject::Manager::self()->closeAllProject();
	d->_current_session->setSessionUpdatable(true);

	Session * current_session = d->_current_session;
	d->_current_session = new Session(this, sessionName);
	delete current_session;
	d->_current_session->loadSession();
	d->_current_session->saveSession();
	XINXConfig::self()->config().project.lastOpenedSession = d->_current_session->sessionName();
	d->updateSessions();

	/* Restore */
	const QString currentProjectPath = d->_current_session->currentProjectPath();
	foreach(const QString & projectPath, d->_current_session->openedProject())
	{
		try
		{
			XinxProject::Manager::self()->openProject(projectPath);
		}
		catch (XinxProject::ProjectException e)
		{
			qWarning() << e.getMessage() << " : " << projectPath;
		}
	}

	if (! currentProjectPath.isEmpty())
	{
		XinxProject::Manager::self()->setSelectedProject(currentProjectPath);
	}

	EditorManager::self()->deserializeEditors(d->_current_session->serializedEditors());
}

void SessionManager::saveSession(const QString & sessionName, bool recover)
{
	EditorManager::self()->serializeEditors(recover);
	d->_current_session->saveSession(sessionName);
}

void SessionManager::renameSession(const QString & sessionName)
{
	d->_current_session->saveSession(sessionName);
	XINXConfig::self()->config().project.lastOpenedSession = d->_current_session->sessionName();
	d->updateSessions();
}

void SessionManager::deleteSession()
{
	Session::removeSession(d->_current_session->sessionName());
	restoreSession(DEFAULT_SESSION);
	d->updateSessions();
}

void SessionManager::createRecoverSession()
{
	saveSession(RECOVER_SESSION, true);
	d->updateSessions();
}

void SessionManager::deleteRecoverSession()
{
	Session::removeSession(RECOVER_SESSION);
	d->updateSessions();
}

} // namespace XinxSession
