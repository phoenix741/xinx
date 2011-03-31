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

// Xinx header
#include "actioninterface_p.h"
#include <core/xinxcore.h>
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>

// Qt header
#include <QTimer>

/*!
 * \namespace XinxAction
 * \since 0.8.1.0
 *
 * \brief Interfaces to manage action in XINX
 */

/*!
 * \defgroup XinxAction Actions of the XINX framework
 */

using namespace XinxAction;

/* MenuItem */

/*!
 * \class XinxAction::MenuItem
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief This class provide a menu item for ActionList
 *
 * \bc
 *
 * Class represent a menu item. A menu item can be an action or a separator. This used by XINX throw plugin,
 * to define the type of the element in the menu.
 */


/*!
 * \brief Create a new menu item based on action \e a
 * \param a The action (or separator) of the menu item
 */
MenuItem::MenuItem(QAction * a) : d(new MenuItemPrivate)
{
	d->m_action = a;
}

//! Destroy the menu item
MenuItem::~MenuItem()
{
}

//! This method return the QAction object
QAction * MenuItem::action() const
{
	return d->m_action;
}

//! This methode set the current QAction
void MenuItem::setAction(QAction* action)
{
	d->m_action = action;
}

/*!
 * \brief This method return true if the action can be called
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool MenuItem::isEnabled() const
{
	return true;
}

//! Use methode isVisible() and isEnabled() to update the status of the action.
void MenuItem::updateActionState()
{
	d->m_action->setVisible(isVisible());
	d->m_action->setEnabled(isEnabled());
}

/*!
 * \brief This method must return true if the action is visible to user
 *
 * \note This method should be redefine in child class.
 *
 * By default this method return \e true.
 */
bool MenuItem::isVisible() const
{
	return true;
}

/* Separator */

/*!
 * \class XinxAction::Separator
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief This class provide a separator item for ActionList
 *
 * \bc
 *
 * This class represent a separator. For a separator there is no data expected.
 */

/*!
 * \brief Create new separator
 *
 * An internal action is automatically created as Separator.
 */
Separator::Separator() : d(new SeparatorPrivate)
{
	d->_visible = true;
	QAction * action = new QAction(this);
	action->setSeparator(true);
	setAction(action);
}

//! Destroy the separator
Separator::~Separator()
{
}

bool Separator::isVisible() const
{
	return d->_visible;
}

void Separator::setVisible(bool value)
{
	d->_visible = value;
}

/* Action */

/*!
 * \class XinxAction::Action
 * \ingroup XinxAction
 * \since 0.8.1.0
 *
 * \brief This class provide an action for ActionList
 *
 * \bc
 *
 * This class is a wrapper of a QAction. This wrapper is used to facilite
 * the enable, and visible feature of the action. This class inherits from
 * MenuItem too, so we can use it in ActionList.
 */

/*!
 * \brief Create an action with the QAction \a a
 *
 * This method set enabled and visible to true.
 */
Action::Action(QAction * a) : MenuItem(a), d(new ActionPrivate)
{
	d->_editor = 0;
	updateActionState();
	connect(action(), SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

/*!
* \brief Create an action with the text \e text.
*
* This methode create a QAction object with the different information from parameters and
* use him self as parent of the QAction.
*/
Action::Action(const QString& text): d(new ActionPrivate)
{
	d->_editor = 0;
	QAction * action = new QAction(text, this);
	setAction(action);
	updateActionState();
	connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

/*!
 * \brief Create an action with the text \e text and the shortcut \e shortcut.
 *
 * This methode create a QAction object with the different information from parameters and
 * use him self as parent of the QAction.
 */
Action::Action(const QString & text, const QKeySequence & shortcut) : d(new ActionPrivate)
{
	d->_editor = 0;
	QAction * action = new QAction(text, this);
	action->setShortcut(shortcut);
	setAction(action);
	updateActionState();
	connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

/*!
 * \brief Create an actiion with the icon \e icon, text \e text, and the shortcut \e shortcut
 *
 * This methode create a QAction object with the different information from parameters and
 * use him self as parent of the QAction.
 */
Action::Action(const QIcon & icon, const QString & text, const QKeySequence & shortcut) : d(new ActionPrivate)
{
	d->_editor = 0;
	QAction * action = new QAction(icon, text, this);
	action->setShortcut(shortcut);
	setAction(action);
	updateActionState();
	connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

//! Destroy the action
Action::~Action()
{
}

/*!
 * \brief Get the current editor (or not) and trigger the action.
 *
 * \note This method should be redefine in child class.
 *
 * By default this method do nothing.
 */
void Action::actionTriggered()
{
}

/*!
 * \brief Add a signal to observe to update action state.
 * \since 0.10.0.0
 *
 * The signal is automatically added to the list of signal to observer. When the signal
 * is triggered, the methode updateActionState() is automatically called.
 *
 * \param signal The signal to observe. Must have the form SIGNAL(methode())
 */
void Action::addEditorSignals(const char* signal)
{
	d->_signals.append(signal);
}

/*!
 * \brief Return the current editor
 * \since 0.10.0.0
 */

AbstractEditor* Action::currentEditor() const
{
	return d->_editor;
}

/*!
 * \brief Change the current editor on the action
 * \since 0.10.0.0
 *
 * When the editor is changed, signal added by addEditorSignals() are activated to
 * observe the editor.
 *
 * \param editor The editor used by user
 */
void Action::setCurrentEditor(AbstractEditor * editor)
{
	if (d->_editor && d->_signals.count())
	{
		foreach(const char * signal, d->_signals)
		{
			disconnect(d->_editor, signal, this, SLOT(updateActionState()));
		}
	}
	d->_editor = editor;
	if (d->_editor && d->_signals.count())
	{
		foreach(const char * signal, d->_signals)
		{
			connect(d->_editor, signal, this, SLOT(updateActionState()));
		}
	}
}

/* ProjectAction */

/*!
 * \class ProjectAction
 * \brief Represent an action for the project directory dock
 * \since 0.10.0.0
 *
 * \bc
 *
 * A project action is not only dependant of the current editor, but is dependant from the
 * list of selected element in the project directory dock.
 *
 * This type of action can be use when the status of the action, or the execution of the action
 * is depending on the number or type of selected element.
 */

/*!
 * \brief Create a ProjectAction with the QAction a
 * \reimp
 */
ProjectAction::ProjectAction(QAction* a): Action(a), d(new ProjectActionPrivate)
{

}

/*!
* \brief Create a ProjectAction with the text \e text
* \reimp
*/
ProjectAction::ProjectAction(const QString& text): Action(text), d(new ProjectActionPrivate)
{

}

/*!
* \brief Create a ProjectAction with the text \e text and the shortcut \e shortcut
* \reimp
*/
ProjectAction::ProjectAction(const QString& text, const QKeySequence& shortcut): Action(text, shortcut), d(new ProjectActionPrivate)
{

}

/*!
* \brief Create a ProjectAction with the icon \e icon, the text \e text, and the shortcut \e shortcut.
* \reimp
*/
ProjectAction::ProjectAction(const QIcon& icon, const QString& text, const QKeySequence& shortcut): Action(icon, text, shortcut), d(new ProjectActionPrivate)
{

}

/*!
* \brief Destroy the ProjectAction
* \reimp
*/
ProjectAction::~ProjectAction()
{

}

/*!
 * \brief Set the selected row in the project dock view.
 *
 * This method is called by the project dock when the user change the selection.
 *
 * \param rows List of selected rows. Each row contains the file/directory selected and
 * the project where is attached the file.
 *
 * \sa selectedRows()
 */
void ProjectAction::setSelectedRows(const QList< ProjectAction::RowInfo >& rows)
{
	d->_selectedRows = rows;
	updateActionState();
}

/*!
 * \brief Get the selected row list
 *
 * This method can be used to get the list of selected rows.
 *
 * \sa setSelectedRows()
 */
const QList< ProjectAction::RowInfo >& ProjectAction::selectedRows() const
{
	return d->_selectedRows;
}


