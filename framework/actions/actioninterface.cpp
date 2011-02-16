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
#include "actioninterface.h"
#include <core/xinxcore.h>
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

// Qt header
#include <QTimer>

/*!
 * \namespace XinxAction
 * \since 0.8.1.0
 *
 * \brief Interfaces to manage action in XINX
 */

/*!
 * \defgroup XinxAction Actions of the XINX framework
 */

using namespace XinxAction;

/* MenuItem */

/*!
 * \class XinxAction::MenuItem
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief This class provide a menu item for ActionList
 *
 * Class represent a menu item. A menu item can be an action or a separator. This used by XINX throw plugin,
 * to define the type of the element in the menu.
 */


MenuItem::MenuItem(QAction * a) : m_action(a)
{

}

//! Destroy the menu item
MenuItem::~MenuItem()
{
}

//! This method return the QAction object
QAction * MenuItem::action() const
{
	return m_action;
}

/*!
 * \brief This method return true if the action can be called
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool MenuItem::isEnabled() const
{
	return true;
}

//! Use methode isVisible() and isEnabled() to update the status of the action.
void MenuItem::updateActionState()
{
	m_action->setVisible(isVisible());
	m_action->setEnabled(isEnabled());
}

/*!
 * \brief This method must return true if the action is visible to user
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool MenuItem::isVisible() const
{
	return true;
}

/* Separator */

/*!
 * \class XinxAction::Separator
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief This class provide a separator item for ActionList
 *
 * This class represent a separator. For a separator there is no data expected.
 */

Separator::Separator() : _visible(true)
{
	m_action = new QAction(this);
	m_action->setSeparator(true);
}

//! Destroy the separator
Separator::~Separator()
{
}

bool Separator::isVisible() const
{
	return _visible;
}

void Separator::setVisible(bool value)
{
	_visible = value;
}

/* Action */

/*!
 * \class XinxAction::Action
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief This class provide an action for ActionList
 *
 * This class is a wrapper of a QAction. This wrapper is used to facilite
 * the enable, and visible feature of the action. This class inherits from
 * MenuItem too, so we can use it in ActionList.
 */

/*!
 * \brief Create an action with the QAction \a a
 *
 * This method set enabled and visible to true.
 */
Action::Action(QAction * a) : MenuItem(a), _editor(0)
{
	updateActionState();
	connect(m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

/*!
* \brief Create an action with the text \e text.
*
* This methode create a QAction object with the different information from parameters and
* use him self as parent of the QAction.
*/
Action::Action(const QString& text): _editor(0)
{
	m_action = new QAction(text, this);
	updateActionState();
	connect(m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

/*!
 * \brief Create an action with the text \e text and the shortcut \e shortcut.
 *
 * This methode create a QAction object with the different information from parameters and
 * use him self as parent of the QAction.
 */
Action::Action(const QString & text, const QKeySequence & shortcut) : _editor(0)
{
	m_action = new QAction(text, this);
	m_action->setShortcut(shortcut);
	updateActionState();
	connect(m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

/*!
 * \brief Create an actiion with the icon \e icon, text \e text, and the shortcut \e shortcut
 *
 * This methode create a QAction object with the different information from parameters and
 * use him self as parent of the QAction.
 */
Action::Action(const QIcon & icon, const QString & text, const QKeySequence & shortcut) : _editor(0)
{
	m_action = new QAction(icon, text, this);
	m_action->setShortcut(shortcut);
	updateActionState();
	connect(m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

//! Destroy the action
Action::~Action()
{
}

/*!
 * \brief Get the current editor (or not) and trigger the action.
 *
 * \note This method should be redefine in child class.
 *
 * By default this method do nothing.
 */
void Action::actionTriggered()
{
}

void Action::addEditorSignals(const char* signal)
{
	_signals.append(signal);
}

AbstractEditor* Action::currentEditor() const
{
	return _editor;
}

void Action::setCurrentEditor(AbstractEditor * editor)
{
	if (_editor && _signals.count())
	{
		foreach(const char * signal, _signals)
		{
			disconnect(_editor, signal, this, SLOT(updateActionState()));
		}
	}
	_editor = editor;
	if (_editor && _signals.count())
	{
		foreach(const char * signal, _signals)
		{
			connect(_editor, signal, this, SLOT(updateActionState()));
		}
	}
}

