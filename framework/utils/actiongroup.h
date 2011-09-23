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

#pragma once
#ifndef ACTIONGROUP_H
#define ACTIONGROUP_H

#include <core/lib-config.h>
#include <QActionGroup>
#include <QScopedPointer>

class ActionGroupPrivate;

class LIBEXPORT ActionGroup : public QActionGroup
{
	Q_OBJECT
public:
	explicit ActionGroup(QObject *parent = 0);
	virtual ~ActionGroup();

	QAction * addAction(QAction * action);
	QAction * addAction(const QString & text);
	QAction * addAction(const QIcon & icon, const QString & text);
	void removeAction(QAction * action);

	void setMenu(QMenu * menu);
	QMenu * menu() const;

	void setMenuSeparator(QAction * action);
	QAction * menuSeparator() const;
signals:
	void actionAdded(QAction * action);
	void actionRemoved(QAction * action);
private:
	QScopedPointer<ActionGroupPrivate> d;
};

#endif // ACTIONGROUP_H
