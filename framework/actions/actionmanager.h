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

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <core/lib-config.h>
#include <core/xinxsingleton.h>
#include <actions/actioninterface.h>

class QStringList;

namespace XinxAction
{

class MenuItem;
class ActionManagerPrivate;

class LIBEXPORT ActionManager : public XinxLibSingleton<ActionManager>
{
	Q_OBJECT
public:
	virtual ~ActionManager();

	const QString nameOfMenu(const QString & menu) const;
	void insertNameOfMenu(const QString & menu, const QString & name);

	QStringList menus() const;
	QList<MenuItem*> menu(const QString & menuId) const;
	void addMenuItem(const QString & menuId, MenuItem * item);
	void addMenuSeparator(const QString & menuId);

	QStringList toolBars() const;
	QList<MenuItem*> toolBar(const QString & toolbarId) const;
	void addToolBarItem(const QString & toolbarId, MenuItem * item);
	void addToolBarSeparator(const QString & toolbarId);

	const QList<MenuItem*> & popup() const;
	void addPopupItem(MenuItem * item);
	void addPopupSeparator();

	const QList<MenuItem*> projectDirectoryPopup() const;
	void addProjectDirectoryPopupItem(MenuItem * item);
	void addProjectDirectoryPopupSeparator();

public slots:
	void updateProjectSelection(QList<XinxAction::ProjectAction::RowInfo> rows);
	void generateMenu();
	void updateMenuItemState();
signals:
	void changed();
private:
	ActionManager();

	QScopedPointer<ActionManagerPrivate> d;
	friend class ActionManagerPrivate;
	friend class XinxLibSingleton<ActionManager>;
};

}

#endif // ACTIONMANAGER_H
