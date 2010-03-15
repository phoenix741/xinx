/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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

/*!
 * \file actioninterface.h
 * \brief Contains class used for plugins's action
 */

#ifndef ACTIONINTERFACE_H
#define ACTIONINTERFACE_H
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>
#include <QAction>
#include <QKeySequence>
#include <QIcon>

/*!
 * \namespace XinxAction
 * \brief Interfaces to manage action in XINX
 */
namespace XinxAction
{

/*!
 * \class MenuItem
 * \brief This class menu item used in a menu list.
 * Class represent a menu item. A menu item can be an action or a separator. This used by plugin, for define
 * which type of item the element is.
 */
class LIBEXPORT MenuItem
{
public:
	//! Destroy the menu item
	virtual ~MenuItem() {};
};

/*!
 * \class Separator
 * \brief Represent a separator
 * Class represent a separator.
 */
class LIBEXPORT Separator : public MenuItem
{
public:
	//! Destroy the separator
	virtual ~Separator() {};
};

/*!
 * \class Action
 * \brief This class represent an action in XINX
 * This class is a wrapper of a Qt action QAction. This wrapper is used to facilite
 * the enable, and visible feature of the action.
 */
class LIBEXPORT Action : public QObject, public MenuItem
{
	Q_OBJECT
public:
	//! Create an action with \a a
	Action(QAction * a, QObject * parent);
	//! Create an action with the text \e text and the shortcut \e shortcut.
	Action(const QString & text, const QKeySequence & shortcut, QObject * parent);
	//! Create an actiion with the icon \e icon, text \e text, and the shortcut \e shortcut
	Action(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent);
	virtual ~Action();

	//! The Action to add
	QAction * action() const;

	//! Return true if the action is visible to user
	virtual bool isActionVisible() const;
	//! Return true if the action can be called
	virtual bool isActionEnabled() const;
	//! Return true if the action can be add to the toolbar
	virtual bool isInToolBar() const;
	//! Return true if the action can be add to the popup menu
	virtual bool isInPopupMenu() const;

public slots:
	//! Use methode isActionVisible() and isActionEnabled() to update the status of the action.
	void updateActionState();
protected slots:
	//! Get the current editor (or not) and trigger the action
	virtual void actionTriggered();
private:
	QAction * m_action;
};

/*!
 * \class ActionList
 * \brief Class to manage a list of action (a Menu)
 * This class is a list of action. This list of action is
 * a menu too.
 */
class LIBEXPORT ActionList : public QList<MenuItem*>
{
public:
	//! Create the list with the name \e menu and the id \e menu
	ActionList(const QString & menu);
	//! Create the list with the name \e menu and the id \e id
	ActionList(const QString & menu, const QString & id);

	//! Return the name of the list
	const QString & menu() const;

	//! Return the id of the menu
	const QString & menuId() const;

	//! Update the state (visible, and enable) of all actions in the menu
	void updateActionsState();
protected:
private:
	QString m_menu, m_menuId;
};

/*!
 * \class MenuList
 * \brief Class to manage a list of ActionList (Menu)
 * This list is a list of menu used in the plugin interface.
 */
class LIBEXPORT MenuList : public QList<ActionList>
{
public:
	//! Create the menu list
	MenuList();

	//! Update the state (visible, and enable) of all menu.
	void updateMenuState();
protected:
private:
};

}

#endif // ACTIONINTERFACE_H
