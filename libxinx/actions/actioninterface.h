/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
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

#ifndef ACTIONINTERFACE_H
#define ACTIONINTERFACE_H

// Qt header
#include <QObject>
#include <QAction>
#include <QKeySequence>
#include <QIcon>

namespace XinxAction {

class MenuItem {
public:
	virtual ~MenuItem() {};
};

class Separator : public MenuItem {
public:
	virtual ~Separator() {};
};

class Action : public QObject, public MenuItem {
	Q_OBJECT
public:
	//! Create an action with \a a
	Action( QAction * a, QObject * parent );
	Action( const QString & text, const QKeySequence & shortcut, QObject * parent );
	Action( const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent );
	virtual ~Action();

	//! The Action to add
	QAction * action() const;

	//! Return true if the action is visible to user
	virtual bool isActionVisible() const;
	//! Return true if the action can be called
	virtual bool isActionEnabled() const;
	//! Return true if the action can be add to the toolbar
	virtual bool isInToolBar() const;

	//! Use methode isActionVisible() and isActionEnabled() to update the status of the action.
	void updateActionState();
protected slots:
	//! Get the current editor (or not) and trigger the action
	virtual void actionTriggered();
private:
	QAction * m_action;
};

class ActionList : public QList<MenuItem*> {
public:
	ActionList( const QString & menu );

	const QString & menu() const;

	void updateActionsState();
protected:
private:
	QString m_menu;
};

class MenuList : public QList<ActionList> {
public:
	MenuList();

	void updateMenuState();
protected:
private:
};

}

#endif // ACTIONINTERFACE_H
