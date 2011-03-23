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

#include "actionmanager.h"
#include "actioninterface.h"
#include <plugins/xinxpluginsloader.h>
#include <plugins/xinxpluginelement.h>
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

// Qt header
#include <QMultiHash>
#include <QSet>

namespace XinxAction
{

/* PrivateActionManager */

class PrivateActionManager
{
public:
	PrivateActionManager();
	~PrivateActionManager();

	void updateSeparatorState(const QList<MenuItem*> & items);

	QHash<QString,QString> _libelles;
	QMultiHash<QString,MenuItem*> _menus;
	QMultiHash<QString,MenuItem*> _toolBars;
	QList<MenuItem*> _popups;
	QList<MenuItem*> _directoryProjectPopup;

	QSet<MenuItem*> _items;
};

PrivateActionManager::PrivateActionManager()
{

}

PrivateActionManager::~PrivateActionManager()
{
	qDeleteAll(_items);
}

void PrivateActionManager::updateSeparatorState(const QList<MenuItem*> & items)
{
	bool isPreviousSeparator = true;
	foreach(MenuItem * item, items)
	{
		Separator * separator = dynamic_cast<Separator*>(item);
		if (separator && isPreviousSeparator)
		{
			separator->setVisible(false);
			separator->updateActionState();
		}
		else if (separator)
		{
			separator->setVisible(true);
			separator->updateActionState();
			isPreviousSeparator = true;
		}
		else
		{
			Action * action = dynamic_cast<Action*>(item);
			if (action->action()->isVisible())   // The result is previously stored
			{
				isPreviousSeparator = false;
			}
		}
	}
}

/* ActionManager */

ActionManager::ActionManager()
{
	d = new PrivateActionManager;
	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(currentEditorChanged(int)));
}

ActionManager::~ActionManager()
{
	delete d;
}

const QString ActionManager::nameOfMenu(const QString & menu) const
{
	return d->_libelles.value(menu);
}

void ActionManager::insertNameOfMenu(const QString & menu, const QString & name)
{
	d->_libelles.insert(menu, name);
}

QStringList ActionManager::menus() const
{
	return d->_menus.uniqueKeys();
}

void ActionManager::addMenuItem(const QString& menu, MenuItem* item)
{
	d->_menus.insert(menu, item);
	d->_items.insert(item);
}

void ActionManager::addMenuSeparator(const QString& menu)
{
	Separator * separator = new Separator;
	addMenuItem(menu, separator);
}

QList< MenuItem* > ActionManager::menu(const QString& menu) const
{
	return d->_menus.values(menu);
}

QStringList ActionManager::toolBars() const
{
	return d->_toolBars.uniqueKeys();
}

void ActionManager::addToolBarItem(const QString& toolbar, MenuItem* item)
{
	d->_toolBars.insert(toolbar, item);
	d->_items.insert(item);
}

void ActionManager::addToolBarSeparator(const QString& toolbar)
{
	Separator * separator = new Separator;
	addToolBarItem(toolbar, separator);
}

QList< MenuItem* > ActionManager::toolBar(const QString& toolbar) const
{
	return d->_toolBars.values(toolbar);
}

const QList< MenuItem* >& ActionManager::popup() const
{
	return d->_popups;
}

void ActionManager::addPopupItem(MenuItem* item)
{
	d->_popups.append(item);
	d->_items.insert(item);
}

void ActionManager::addPopupSeparator()
{
	Separator * separator = new Separator;
	addPopupItem(separator);
}

const QList< MenuItem* > ActionManager::projectDirectoryPopup() const
{
	return d->_directoryProjectPopup;
}

void ActionManager::addProjectDirectoryPopupItem(MenuItem* item)
{
	d->_directoryProjectPopup.append(item);
	d->_items.insert(item);
}

void ActionManager::addProjectDirectoryPopupSeparator()
{
	Separator * separator = new Separator;
	addProjectDirectoryPopupItem(separator);
}

void ActionManager::currentEditorChanged(int index)
{
	AbstractEditor * editor = index >= 0 ? EditorManager::self()->editor(index) : NULL;
	foreach(MenuItem * item, d->_items)
	{
		Action * action = dynamic_cast<Action*>(item);
		if (action)
		{
			action->setCurrentEditor(editor);
		}
	}

	updateMenuItemState();
}

void ActionManager::generateMenu()
{
	QSet<MenuItem*> backupItem = d->_items;
	d->_libelles.clear();
	d->_menus.clear();
	d->_popups.clear();
	d->_directoryProjectPopup.clear();
	d->_items.clear();

	foreach(XinxPluginElement * element, XinxPluginsLoader::self()->plugins())
	{
		IXinxPlugin * pluginAction = qobject_cast<IXinxPlugin*> (element->plugin());
		if (element->isActivated() && pluginAction)
		{
			pluginAction->generateActionMenu();
		}
	}

	emit changed();

	qDeleteAll(backupItem);

	updateMenuItemState();
}

void ActionManager::updateMenuItemState()
{
	// Mise à jours des statuts des actions
	foreach(MenuItem * item, d->_items)
	{
		Action * action = dynamic_cast<Action*>(item);
		if (action)
		{
			action->updateActionState();
		}
	}

	// Browse menu to update separator state.
	foreach(const QString & key, d->_menus.uniqueKeys())
	{
		d->updateSeparatorState(d->_menus.values(key));
	}
	// Browse toolbar to update separator state
	foreach(const QString & key, d->_toolBars.uniqueKeys())
	{
		d->updateSeparatorState(d->_menus.values(key));
	}
	// Browse popup menu to update separator state
	d->updateSeparatorState(d->_popups);
}

void ActionManager::updateProjectSelection(QList< ProjectAction::RowInfo > rows)
{
	foreach(MenuItem * item, d->_directoryProjectPopup)
	{
		ProjectAction * action = dynamic_cast<ProjectAction*>(item);
		if (action)
		{
			action->setSelectedRows(rows);
		}
	}

	d->updateSeparatorState(d->_directoryProjectPopup);
}

}
