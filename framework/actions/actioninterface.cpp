/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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
#include "actioninterface.h"
#include <core/xinxcore.h>

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

//! Destroy the menu item
MenuItem::~MenuItem() {
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

//! Destroy the separator
Separator::~Separator() {
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
Action::Action(QAction * a, QObject * parent) : QObject(parent), m_action(a)
{
	updateActionState();
	connect(m_action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

/*!
 * \brief Create an action with the text \e text and the shortcut \e shortcut.
 *
 * This methode create a QAction object with the different information from parameters and
 * use him self as parent of the QAction.
 */
Action::Action(const QString & text, const QKeySequence & shortcut, QObject * parent) : QObject(parent)
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
Action::Action(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent) : QObject(parent)
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

//! This method return the QAction object
QAction * Action::action() const
{
	return m_action;
}

/*!
 * \brief This method must return true if the action is visible to user
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool Action::isActionVisible() const
{
	return true;
}

/*!
 * \brief This method return true if the action can be called
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool Action::isActionEnabled() const
{
	return true;
}

/*!
 * \brief This method must return true if the action can be add to the toolbar
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool Action::isInToolBar() const
{
	return true;
}

/*!
 * \brief This method must return true if the action can be add to the popup menu
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool Action::isInPopupMenu() const
{
	return true;
}

//! Use methode isActionVisible() and isActionEnabled() to update the status of the action.
void Action::updateActionState()
{
	m_action->setVisible(isActionVisible());
	m_action->setEnabled(isActionEnabled());
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

/* ActionList */

/*!
 * \class XinxAction::ActionList
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief Class to manage a list of action (a Menu)
 *
 * This class is a list of action. This also the synonym for a menu. This list as
 * also a name and a method to refresh the state of each action.
 */


//! Create the list with the name \e menu and the id \e menu
ActionList::ActionList(const QString & menu) : m_menu(menu), m_menuId(menu)
{
}

//! Create the list with the name \e menu and the id \e id
ActionList::ActionList(const QString & menu, const QString & id) : m_menu(menu), m_menuId(id)
{
}

//! Return the name of the list
const QString & ActionList::menu() const
{
	return m_menu;
}

//! Return the id of the menu
const QString & ActionList::menuId() const
{
	return m_menuId;
}

//! Update the state (visible, and enable) of all actions in the menu
void ActionList::updateActionsState()
{
	foreach(MenuItem * item, *this)
	{
		Action * action = dynamic_cast<Action*>(item);
		if (action)
			action->updateActionState();
	}
}

/* MenuList */

/*!
 * \class XinxAction::MenuList
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief Class to manage a list of ActionList (Menu)
 *
 * This list is a list of menu used in the plugin interface. A method
 * to refresh all menu of a plugin exists.
 */

//! Create the menu list
MenuList::MenuList()
{
}

//! Update the state (visible, and enable) of all menu.
void MenuList::updateMenuState()
{
	foreach(ActionList menu, *this)
	{
		menu.updateActionsState();
	}
}

