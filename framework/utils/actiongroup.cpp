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

#include "actiongroup.h"
#include <QMenu>
#include <QAction>
#include <QDebug>

/* PrivateActionGroup */

class PrivateActionGroup
{
public:
	PrivateActionGroup();

	ActionGroup * _group;
	QMenu * _menu;
	QAction * _separator;

	void addActionToMenu(QAction * action);
	void removeActionOfMenu(QAction * action);

	void cleanMenu();
	void createMenu();
};

PrivateActionGroup::PrivateActionGroup() : _group(0), _menu(0), _separator(0)
{

}

void PrivateActionGroup::cleanMenu()
{
	foreach(QAction * action, _group->actions())
	{
		removeActionOfMenu(action);
	}
}

void PrivateActionGroup::createMenu()
{
	foreach(QAction * action, _group->actions())
	{
		addActionToMenu(action);
	}
}

void PrivateActionGroup::addActionToMenu(QAction * action)
{
	if (_menu && _separator)
	{
		_menu->insertAction(_separator, action);
		_separator->setVisible(true);
	}
}

void PrivateActionGroup::removeActionOfMenu(QAction * action)
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

ActionGroup::ActionGroup(QObject *parent) : QActionGroup(parent), d(new PrivateActionGroup)
{
	d->_group = this;
}

ActionGroup::~ActionGroup()
{

}

QAction * ActionGroup::addAction(QAction * action)
{
	QAction * a = QActionGroup::addAction(action);
	d->addActionToMenu(a);
	emit actionAdded(a);
	return a;
}

QAction * ActionGroup::addAction(const QString & text)
{
	QAction * a = QActionGroup::addAction(text);
	d->addActionToMenu(a);
	emit actionAdded(a);
	return a;
}

QAction * ActionGroup::addAction(const QIcon & icon, const QString & text)
{
	QAction * a = QActionGroup::addAction(icon, text);
	d->addActionToMenu(a);
	emit actionAdded(a);
	return a;
}

void ActionGroup::removeAction(QAction * action)
{
	emit actionRemoved(action);

	d->removeActionOfMenu(action);
	QActionGroup::removeAction(action);
}

void ActionGroup::setMenu(QMenu * menu)
{
	d->cleanMenu();
	d->_menu = menu;
	d->createMenu();
}

QMenu * ActionGroup::menu() const
{
	return d->_menu;
}

void ActionGroup::setMenuSeparator(QAction * action)
{
	d->cleanMenu();
	d->_separator = action;
	d->createMenu();
}

QAction * ActionGroup::menuSeparator() const
{
	return d->_separator;
}
