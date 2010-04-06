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

#pragma once
#ifndef ACTIONINTERFACE_H
#define ACTIONINTERFACE_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QObject>
#include <QAction>
#include <QKeySequence>
#include <QIcon>

namespace XinxAction
{

class LIBEXPORT MenuItem
{
public:
	virtual ~MenuItem();
};

class LIBEXPORT Separator : public MenuItem
{
public:
	virtual ~Separator();
};

class LIBEXPORT Action : public QObject, public MenuItem
{
	Q_OBJECT
public:
	Action(QAction * a, QObject * parent);
	Action(const QString & text, const QKeySequence & shortcut, QObject * parent);
	Action(const QIcon & icon, const QString & text, const QKeySequence & shortcut, QObject * parent);
	virtual ~Action();

	QAction * action() const;

	virtual bool isActionVisible() const;
	virtual bool isActionEnabled() const;
	virtual bool isInToolBar() const;
	virtual bool isInPopupMenu() const;

public slots:
	void updateActionState();
protected slots:
	virtual void actionTriggered();
private:
	QAction * m_action;
};

class LIBEXPORT ActionList : public QList<MenuItem*>
{
public:
	ActionList(const QString & menu);
	ActionList(const QString & menu, const QString & id);

	const QString & menu() const;
	const QString & menuId() const;

	void updateActionsState();
private:
	QString m_menu, m_menuId;
};

class LIBEXPORT MenuList : public QList<ActionList>
{
public:
	MenuList();

	void updateMenuState();
};

}

#endif // ACTIONINTERFACE_H
