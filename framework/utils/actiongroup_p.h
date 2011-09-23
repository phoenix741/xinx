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

#ifndef _ACTIONGROUPPRIVATE_
#define _ACTIONGROUPPRIVATE_

#include <utils/actiongroup.h>
#include <QMenu>
#include <QAction>
#include <QDebug>

/* PrivateActionGroup */

class ActionGroupPrivate
{
public:
	ActionGroupPrivate();

	ActionGroup * _group;
	QMenu * _menu;
	QAction * _separator;

	void addActionToMenu(QAction * action);
	void removeActionOfMenu(QAction * action);

	void cleanMenu();
	void createMenu();
};

#endif /* _ACTIONGROUPPRIVATE_ */