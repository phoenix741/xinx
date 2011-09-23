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

#include "actiongroup_p.h"

/* ActionGroupPrivate */

ActionGroupPrivate::ActionGroupPrivate() : _group(0), _menu(0), _separator(0)
{

}

void ActionGroupPrivate::cleanMenu()
{
	foreach(QAction * action, _group->actions())
	{
		removeActionOfMenu(action);
	}
}

void ActionGroupPrivate::createMenu()
{
	foreach(QAction * action, _group->actions())
	{
		addActionToMenu(action);
	}
}

void ActionGroupPrivate::addActionToMenu(QAction * action)
{
	if (_menu && _separator)
	{
		_menu->insertAction(_separator, action);
		_separator->setVisible(true);
	}
}

void ActionGroupPrivate::removeActionOfMenu(QAction * action)
{
	if (_menu && _separator)
	{
		_menu->removeAction(action);
		if (_group->actions().size() == 0)
		{
			_separator->setVisible(false);
		}
	}
}

/* ActionGroup */

/*!
 * \class ActionGroup
 * \ingroup UtilsClass
 * \since 0.10.1
 *
 * \brief Group of action used for session
 *
 * In a QActionGroup, all actions have a radio button. When a action is selected
 * another action is unselected.
 *
 * We can use triggered() signal of QActionGroup, to receive an unique action from the group and
 * not X signals from X actions.
 * We can use the exclusive property of QActionGroup to have checkbox and not radiobutton.
 *
 * The ActionGroup propose signals actionAdded(), actionRemoved(). and manage automatically a QMenu.
 * In this class QActions are automatically added after the separator defined with menuSeparator().
 */

/*!
 * \fn void ActionGroup::actionAdded(QAction * action)
 * \brief Signal emitted when an action is added to the menu.
 */

/*!
 * \fn void ActionGroup::actionRemoved(QAction * action)
 * \brief Signal emitted when an action is removed from the menu.
 */

//! Create an action group
ActionGroup::ActionGroup(QObject *parent) : QActionGroup(parent), d(new ActionGroupPrivate)
{
	d->_group = this;
}

//! Destroy an action group
ActionGroup::~ActionGroup()
{

}

/*!
 * \brief Adds the \p action to this group, and returns it.
 *
 * This method add the action after the menuSeparator() in the menu().
 *
 * \sa removeAction(), addAction(const QString&), addAction(const QIcon&, const QString&)
 */
QAction * ActionGroup::addAction(QAction * action)
{
	QAction * a = QActionGroup::addAction(action);
	d->addActionToMenu(a);
	emit actionAdded(a);
	return a;
}

/*!
 * \brief Creates and return an action with \p text. The newly created action is a child
 * of this action group.
 *
 * This method add the action after the menuSeparator() in the menu().
 *
 * \sa removeAction(), addAction(QAction*), addAction(const QIcon&, const QString&)
 */
QAction * ActionGroup::addAction(const QString & text)
{
	QAction * a = QActionGroup::addAction(text);
	d->addActionToMenu(a);
	emit actionAdded(a);
	return a;
}

/*!
 * \brief Creates and returns an action with \p text and an \p icon. The newly created action is a child
 * of this action group.
 *
 * This method add the action after the menuSeparator() in the menu().
 *
 * \sa removeAction(), addAction(QAction*), addAction(const QString&)
 */
QAction * ActionGroup::addAction(const QIcon & icon, const QString & text)
{
	QAction * a = QActionGroup::addAction(icon, text);
	d->addActionToMenu(a);
	emit actionAdded(a);
	return a;
}

/*!
 * \brief Removes the \p action from this group. The action will have no parent as a result.
 *
 * This method remove the action from the menu().
 *
 * \sa addAction(QAction*), addAction(const QString&), addAction(const QIcon&, const QString&)
 */
void ActionGroup::removeAction(QAction * action)
{
	emit actionRemoved(action);

	d->removeActionOfMenu(action);
	QActionGroup::removeAction(action);
}

/*!
 * \brief Set the \p menu where to add the action
 *
 * \sa menu()
 */
void ActionGroup::setMenu(QMenu * menu)
{
	d->cleanMenu();
	d->_menu = menu;
	d->createMenu();
}

/*!
 * \brief Return the \p menu where we add the action
 *
 * \sa setMenu()
 */
QMenu * ActionGroup::menu() const
{
	return d->_menu;
}

/*!
 * \brief Set the \p action as menu separator
 *
 * Actions are added after this menu separator.
 *
 * \sa setMenuSeparator()
 */
void ActionGroup::setMenuSeparator(QAction * action)
{
	d->cleanMenu();
	d->_separator = action;
	d->createMenu();
}

/*!
 * \brief Return the menuSeparator() used to add actions.
 *
 * \sa setMenuSeparator()
 */
QAction * ActionGroup::menuSeparator() const
{
	return d->_separator;
}
