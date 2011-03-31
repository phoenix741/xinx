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

#include "actionmanager_p.h"

namespace XinxAction
{

/* ActionManagerPrivate */

ActionManagerPrivate::ActionManagerPrivate()
{

}

ActionManagerPrivate::~ActionManagerPrivate()
{
	qDeleteAll(_items);
}

void ActionManagerPrivate::currentEditorChanged(int index)
{
	AbstractEditor * editor = index >= 0 ? EditorManager::self()->editor(index) : NULL;
	foreach(MenuItem * item, _items)
	{
		Action * action = dynamic_cast<Action*>(item);
		if (action)
		{
			action->setCurrentEditor(editor);
		}
	}

	_parent->updateMenuItemState();
}

void ActionManagerPrivate::updateSeparatorState(const QList<MenuItem*> & items)
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

/*!
 * \class XinxAction::ActionManager
 * \ingroup XinxAction
 * \brief Singleton class that provide a way to manage all actions.
 * \since 0.10.0.0
 *
 * \bc 0.10.0.0
 *
 * This class is used by plugins to add new action to
 *  - Menu
 *  - Tool bar
 *  - Editor popup menu
 *  - Project directory dock popup menu
 *
 * Next this class is used by Mainform, Editor, Project directory dock, to
 * read actions provide by plugins to add it on the GUI.
 *
 * When method IXinxPlugin::generateActionMenu() is called, the plugin must use
 * add method on the ActionManager singleton.
 *
 * This method is called by local methode generateMenu()
 *
 * Action will be destroyed by the application automatically (not the plugin). IXinxPlugin
 * musn't be owner of plugins.
 */

/*!
 * \fn void ActionManager::changed()
 * \brief Signal emited when list of action of ActionManager is updated.
 */

/*!
 * \brief Create the ActionManager.
 * \intern
 */
ActionManager::ActionManager() : d(new ActionManagerPrivate)
{
	d->_parent = this;
	connect(EditorManager::self(), SIGNAL(currentChanged(int)), d.data(), SLOT(currentEditorChanged(int)));
}

//! Destroy the ActionManager
ActionManager::~ActionManager()
{

}

//! Retrieve the name to show to the screen for the menu \e menu.
const QString ActionManager::nameOfMenu(const QString & menu) const
{
	return d->_libelles.value(menu);
}

/*!
 * \brief Insert a name for the menu of id \e menu with the name \e name
 *
 * \param menu The id of the menu
 * \param name The translated name of the menu (the menu must be protected with tr()
 * function.
 */
void ActionManager::insertNameOfMenu(const QString & menu, const QString & name)
{
	d->_libelles.insert(menu, name);
}

//! Return the list of ID menu to create on the application
QStringList ActionManager::menus() const
{
	return d->_menus.uniqueKeys();
}

/*!
 * \brief Add the \e item to the menu given by \e menuId
 *
 * There is several default menu and where there is no need to create the menu with
 * insertNameOfMenu() :
 *
 *  - session
 *  - project
 *  - file
 *  - edit
 *  - search
 *  - bookmark
 *  - windows
 *  - tools
 *  - help
 *
 */
void ActionManager::addMenuItem(const QString& menuId, MenuItem* item)
{
	d->_menus.insert(menuId, item);
	d->_items.insert(item);
}

/*!
 * \brief Add a separator in the menu \e menuId
 * \sa addMenuItem()
 */
void ActionManager::addMenuSeparator(const QString& menuId)
{
	Separator * separator = new Separator;
	addMenuItem(menuId, separator);
}

//! List all item for the given menu (separator included)
QList< MenuItem* > ActionManager::menu(const QString& menuId) const
{
	return d->_menus.values(menuId);
}

//! List all available toolbars ID
QStringList ActionManager::toolBars() const
{
	return d->_toolBars.uniqueKeys();
}

/*!
 * \brief Add the \e item to the toolbar given by \e toolbarId
 *
 * There is several default toolbar :
 *  - project
 *  - file
 *  - edit
 *  - search
 *
 */
void ActionManager::addToolBarItem(const QString& toolbarId, MenuItem* item)
{
	d->_toolBars.insert(toolbarId, item);
	d->_items.insert(item);
}

/*!
 * \brief Add a separator into the toolbar
 * \sa addToolBarItem()
 */
void ActionManager::addToolBarSeparator(const QString& toolbarId)
{
	Separator * separator = new Separator;
	addToolBarItem(toolbarId, separator);
}

//! List all item for the given toolbar (separator included)
QList< MenuItem* > ActionManager::toolBar(const QString& toolbarId) const
{
	return d->_toolBars.values(toolbarId);
}

//! List all item for the editor popup menu (separator included)
const QList< MenuItem* >& ActionManager::popup() const
{
	return d->_popups;
}

//! Add a new item to the editor's popup menu
void ActionManager::addPopupItem(MenuItem* item)
{
	d->_popups.append(item);
	d->_items.insert(item);
}

//! Add a separator to the editor's popup menu
void ActionManager::addPopupSeparator()
{
	Separator * separator = new Separator;
	addPopupItem(separator);
}

//! List all item for the project directory dock popup menu (separator included)
const QList< MenuItem* > ActionManager::projectDirectoryPopup() const
{
	return d->_directoryProjectPopup;
}

//! Add a new item in the project directory dock popup menu
void ActionManager::addProjectDirectoryPopupItem(MenuItem* item)
{
	d->_directoryProjectPopup.append(item);
	d->_items.insert(item);
}

//! Add a new separator in the project directory dock popup menu
void ActionManager::addProjectDirectoryPopupSeparator()
{
	Separator * separator = new Separator;
	addProjectDirectoryPopupItem(separator);
}

//! Clear (and delete) old actions and regenerate actions list by calling IXinxPlugin::generateActionMenu()
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

	// In case the menu item are in use
	qDeleteAllLater(backupItem);

	updateMenuItemState();
}

//! Update the state (visible, enable) of each action (and separator)
void ActionManager::updateMenuItemState()
{
	// Update of state of each action
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

/*!
 * \brief Update the project selection for each XinxAction::ProjectAction with \e rows
 *
 * Call when the selection change in the project directory dock.
 */
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
