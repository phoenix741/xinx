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
class PrivateActionManager;

class LIBEXPORT ActionManager : public XinxLibSingleton<ActionManager>
{
	Q_OBJECT
public:
	virtual ~ActionManager();

	const QString nameOfMenu(const QString & menu) const;
	void insertNameOfMenu(const QString & menu, const QString & name);

	QStringList menus() const;
	QList<MenuItem*> menu(const QString & menu) const;
	void addMenuItem(const QString & menu, MenuItem * item);
	void addMenuSeparator(const QString & menu);

	QStringList toolBars() const;
	QList<MenuItem*> toolBar(const QString & toolbar) const;
	void addToolBarItem(const QString & toolbar, MenuItem * item);
	void addToolBarSeparator(const QString & toolbar);

	const QList<MenuItem*> & popup() const;
	void addPopupItem(MenuItem * item);
	void addPopupSeparator();

	const QList<MenuItem*> projectDirectoryPopup() const;
	void addProjectDirectoryPopupItem(MenuItem * item);
	void addProjectDirectoryPopupSeparator();

public slots:
	//! Call when a new selection is made in the projectDirectory dock
	void updateProjectSelection(QList<XinxAction::ProjectAction::RowInfo> rows);

	//! Call plugins and generate list (erase all). emit changed when finish
	void generateMenu();

	//! Update the state of menu item and the visibility of separator of different section
	void updateMenuItemState();
signals:
	void changed();
private slots:
	virtual void currentEditorChanged(int index);
private:
	ActionManager();

	PrivateActionManager * d;
	friend class PrivateActionManager;
	friend class XinxLibSingleton<ActionManager>;
};

}

#endif // ACTIONMANAGER_H
