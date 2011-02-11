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

#include "sessioneditor.h"
#include "session.h"
#include <project/xinxprojectproject.h>

namespace XinxSession {

/* PrivateSessionEditor */

class PrivateSessionEditor
{
public:
	PrivateSessionEditor(Session * session, SessionEditor * session_editor);

	SessionEditor * _session_editor;
	QHash<QString,QVariant> _properties;
	Session * _parent;
	XinxProject::ProjectPtrWeak _project;
};

PrivateSessionEditor::PrivateSessionEditor(Session * session, SessionEditor * session_editor) : _session_editor(session_editor), _parent(session)
{

}

/* SessionEditor */

/*!
 * \ingroup XinxSession
 * \class SessionEditor
 * \brief Represent a session editor
 * \since 0.9.1.0
 *
 * A session editor is a map of key/value. An editor can store information as
 * file name, contents, state, and all other properties that can be used to
 * restore the editor as saving.
 *
 * The value is a QVariant but must be writtable in a text file. (QString, int, ...)
 */

/*!
 * \brief Create a session editor used to store editor's parameters
 *
 * A session éditor is used to store all parameters of an editor. It can be used to
 * restore this parameters.
 *
 * The editor must save all necessarry editor that can be used to restore the editor.
 *
 * \param parent The session file where to store the editor information
 */
SessionEditor::SessionEditor(Session * parent) : d(new PrivateSessionEditor(parent, this))
{
}

/*!
 * \brief Destroy the session editor
 */
SessionEditor::~SessionEditor()
{
}

void SessionEditor::setProject(XinxProject::ProjectPtr project)
{
	d->_project = project.toWeakRef();
}

XinxProject::ProjectPtr SessionEditor::project() const
{
	return d->_project.toStrongRef();
}

/*!
 * \brief Store the property \e key with the \e value in the session.
 * \param key The key used to store the property
 * \param value The value to store
 */
void SessionEditor::writeProperty(const QString & key, QVariant value)
{
	d->_properties[ key ] = value;
}

/*!
 * \brief Read the property \e key
 * \param key The key used to store the information
 * \return the value associate to the property
 */
QVariant SessionEditor::readProperty(const QString & key)
{
	return d->_properties.value(key);
}

QStringList SessionEditor::propertieKeys() const
{
	return d->_properties.keys ();
}

} // namespace XinxSession
