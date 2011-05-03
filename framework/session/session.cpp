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
#include <core/xinxcore.h>
#include "session_p.h"
#include <project/xinxprojectproject.h>
#include <project/xinxprojectprojectexception.h>
#include <session/sessioneditor.h>
#include <session/sessionmanager.h>

// Qt header
#include <QSettings>

namespace XinxSession
{

/* PrivateSession */

PrivateSession::PrivateSession(Session * session) : _session(session), _is_session_updatable(true)
{
	_current_session_name = DEFAULT_SESSION;
}

void PrivateSession::save(const QString & saveName)
{
	Q_ASSERT_X(! _current_session_name.isEmpty(), "PrivateSession::load", "The current session name is empty");
	Q_ASSERT_X(! saveName.isEmpty(), "PrivateSession::load", "The current session name is empty");

	QSettings settings;
	settings.beginGroup("sessions");
	settings.remove(saveName);
	settings.beginGroup(saveName);

	settings.setValue("name", _current_session_name);
	settings.setValue("currentProject", _current_project_path);

	_project_path_opened.removeDuplicates();
	settings.beginWriteArray("projects");
	for (int i = 0; i < _project_path_opened.size(); i++)
	{
		settings.setArrayIndex(i);
		const QString & path = _project_path_opened.at(i);
		settings.setValue("path", path);
	}
	settings.endArray();

	settings.beginWriteArray("files");
	for (int i = 0; i < _last_opened_file.size(); i++)
	{
		settings.setArrayIndex(i);
		const QString & filename = _last_opened_file.at(i);
		settings.setValue("filename", filename);
	}
	settings.endArray();

	settings.beginWriteArray("editors");
	for (int i = 0; i < _opened_editor.size(); i++)
	{
		settings.setArrayIndex(i);
		SessionEditor * editor = _opened_editor.at(i);

		foreach(const QString & key, editor->propertieKeys())
		{
			settings.setValue(key, editor->readProperty(key));
		}
	}
	settings.endArray();

	settings.endGroup();
	settings.endGroup();
}

void PrivateSession::load()
{
	Q_ASSERT_X(! _current_session_name.isEmpty(), "PrivateSession::load", "The current session name is empty");
	Q_ASSERT_X(_is_session_updatable, "PrivateSession::load", "The session is not updatable");

	qDeleteAll(_opened_editor);
	_opened_editor.clear();
	_last_opened_file.clear();
	_project_path_opened.clear();

	QSettings settings;
	settings.beginGroup("sessions");
	settings.beginGroup(_current_session_name);

	if (!settings.value("name").toString().isEmpty())
	{
		_current_session_name = settings.value("name").toString();
	}
	if (!settings.value("currentProject").toString().isEmpty())
	{
		_current_project_path = settings.value("currentProject").toString();
	}

	int size = settings.beginReadArray("projects");
	for (int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);
		const QString path = settings.value("path").toString();
		_project_path_opened.append(path);
	}
	settings.endArray();

	size = settings.beginReadArray("files");
	for (int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);
		const QString & filename = settings.value("filename").toString();
		_last_opened_file.append(filename);
	}
	settings.endArray();

	size = settings.beginReadArray("editors");
	for (int i = 0; i < size; i++)
	{
		settings.setArrayIndex(i);
		SessionEditor * editor = new SessionEditor(_session);

		foreach(const QString & key, settings.allKeys())
		{
			editor->writeProperty(key, settings.value(key));
		}

		_opened_editor.append(editor);
	}
	settings.endArray();

	settings.endGroup();
	settings.endGroup();
}

/* Session */

/*!
 * \ingroup XinxSession
 * \class Session
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
Session::Session(SessionManager * manager, const QString & sessionName) : QObject(manager), d(new PrivateSession(this))
{
	d->_current_session_name = sessionName;
	// Pas de lecture automatique, elle doit être explicite (ce qui permettrai d'écraiser la session default).
	//loadSession (sessionName);
}

/*!
 * \brief Destroy the session
 */
Session::~Session()
{
	qDeleteAll(d->_opened_editor);
}

const QStringList & Session::openedProject() const
{
	return d->_project_path_opened;
}

void Session::addOpenedProject(const QString & projectPath)
{
	if (!d->_is_session_updatable) return;

	if (! d->_project_path_opened.contains(QDir::cleanPath(projectPath)))
	{
		d->_project_path_opened.append(QDir::cleanPath(projectPath));
		d->save(d->_current_session_name);
		emit changed();
	}
}

void Session::deleteOpenedProject(const QString & projectPath)
{
	if (!d->_is_session_updatable) return;

	d->_project_path_opened.removeAll(projectPath);
	d->save(d->_current_session_name);
	emit changed();
}

/*!
 * \brief List all the last opend files in the project.
 * \return the list of the last opend file.
 */
const QStringList & Session::lastOpenedFile() const
{
	return d->_last_opened_file;
}

/*!
 * \brief Add a new opened file to the list.
 * \param filename filename to add to the list
 *
 * This methode keep the last MAX_RECENT_FILES element. If a new element is added
 * and the list have already MAX_RECENT_FILES elements, the last element will be deleted.
 */
void Session::addOpenedFile(const QString & filename)
{
	if (!d->_is_session_updatable) return;

	d->_last_opened_file.removeAll(filename);
	d->_last_opened_file.prepend(filename);

	while (d->_last_opened_file.size() > MAXRECENTFILES)
	{
		d->_last_opened_file.removeLast();
	}
	d->save(d->_current_session_name);
	emit changed();
}

void Session::setCurrentProjectPath(const QString& path)
{
	if (!d->_is_session_updatable) return;

	if (d->_current_project_path != path)
	{
		d->_current_project_path = path;
		d->save(d->_current_session_name);
		emit changed();
	}
}

const QString& Session::currentProjectPath() const
{
	return d->_current_project_path;
}

/*!
 * \brief Clear the list of serialized editors.
 */
void Session::clearSerializedEditors()
{
	if (!d->_is_session_updatable) return;

	qDeleteAll(d->_opened_editor);
	d->_opened_editor.clear();
	d->save(d->_current_session_name);
	emit changed();
}

/*!
 * \brief List of serialized editor.
 * \return Return the list of serialized editor stored in the session file.
 */
const QList<SessionEditor*> & Session::serializedEditors() const
{
	return d->_opened_editor;
}

/*!
 * \brief Add a new session file in the editor in the list
 * \param editor the editor to add.
 */
void Session::addSerializedEditor(SessionEditor * editor)
{
	if (!d->_is_session_updatable) return;

	d->_opened_editor.append(editor);
	d->save(d->_current_session_name);
	emit changed();
}

void Session::removeSerializedEditor(SessionEditor * editor)
{
	if (!d->_is_session_updatable) return;

	d->_opened_editor.removeAll(editor);
	d->save(d->_current_session_name);
	emit changed();
}

const QString & Session::sessionName() const
{
	return d->_current_session_name;
}

void Session::loadSession(const QString& sessionName)
{
	if (! sessionName.isEmpty())
	{
		d->_current_session_name = sessionName;
	}
	d->load();
	emit changed();
}

void Session::saveSession(const QString& sessionName)
{
	if (! sessionName.isEmpty() && (sessionName != RECOVER_SESSION))
	{
		removeSession(d->_current_session_name);
		d->_current_session_name = sessionName;
	}
	d->save(sessionName.isEmpty() ? d->_current_session_name : sessionName);
	emit changed();
}

void Session::removeSession(const QString & name)
{
	Q_ASSERT_X(!name.isEmpty(), "Session::removeSession", "Session name can't be empty");
	QSettings settings;
	settings.beginGroup("sessions");
	settings.remove(name);
	settings.endGroup();
}

QStringList Session::sessionsNames()
{
	QStringList keys;
	QSettings settings;
	settings.beginGroup("sessions");
	keys = settings.childGroups();
	settings.endGroup();

	// On ajoute toujours la session par défaut, même s'elle n'est pas sauvegardé.
	if (!keys.contains(DEFAULT_SESSION))
	{
		keys.append(DEFAULT_SESSION);
	}

	return keys;
}

void Session::setSessionUpdatable(bool value)
{
	d->_is_session_updatable = value;
}

bool Session::isSessionUpdatable() const
{
	return d->_is_session_updatable;
}

} // namespace XinxSession
