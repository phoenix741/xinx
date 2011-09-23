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

#include "xinxdockwidget.h"
#include <dtoolview.h>
#include <dviewbutton.h>
#include <core/xinxconfig.h>

/*!
* \defgroup gui Application's class
*/

/*!
 * \ingroup gui
 * \class XinxDockWidget
 * \since 0.10.2
 *
 * \brief Abstract class to use to make a dock widget.
 *
 * You must subclass this class when you want to make a new dock for XINX. This class contains method to show
 * title and to show shortcut.
 *
 * This class propose also two methods to get and set the dock.
 *
 * If you want add an information in the title, you must subclass the method windowTitle().
 */

/*!
 * \fn void XinxDockWidget::dockChanged()
 * \brief The signal is emited when DTooView is changed.
 *
 */

/*!
 * \fn void XinxDockWidget::windowChanged()
 * \brief The signal is emited when the title is changed (the shortcut or notification is modified)
 */

//! Create a new dock for XINX.
XinxDockWidget::XinxDockWidget(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f), _dock(0)
{
	connect(this, SIGNAL(dockChanged()), this, SLOT(updateDockTitle()));
	connect(this, SIGNAL(windowChanged()), this, SLOT(updateDockTitle()));
}

//! Destroy the dock.
XinxDockWidget::~XinxDockWidget()
{

}

/*!
 * \brief Return the title of the dock.
 *
 * The title is shown in the button, that you can see below the dock. This button is used
 * to show or hide the dock.
 *
 * This default method show the shortcut in the toolbar.
 *
 * This method must be subclassed to show a more detailled title.
 */
QString XinxDockWidget::windowTitle() const
{
	if (!_shortcut.isEmpty() && XINXConfig::self()->config().dock.showShortcut)
	{
		return QWidget::windowTitle() + QString(" (%1)").arg(_shortcut.toString());
	}
	return QWidget::windowTitle();
}

/*!
 * \brief Set the DToolView dock to \p dock
 *
 * When the dock is changed the signal dockChanged() is emited.
 *
 * \sa dock(), dockChanged()
 */
void XinxDockWidget::setDock(DToolView* dock)
{
	if (_dock != dock)
	{
		_dock = dock;

		emit dockChanged();
	}
}

/*!
 * \brief Return the dock of type DToolView of the class.
 * \sa setDock()
 */
DToolView* XinxDockWidget::dock()
{
	return _dock;
}

/*!
 * \brief Set the shortcut for the dock to show in the title
 *
 * When the shortcut is changed the signal windowChanged() is emited.
 *
 * \sa getShortcut(), windowChanged()
 */
void XinxDockWidget::setShortcut(const QKeySequence & shortcut)
{
	if (_shortcut != shortcut)
	{
		_shortcut = shortcut;

		emit windowChanged();
	}
}

/*!
 * \brief Return the current shortcut of the dock.
 * \sa setShortcut()
 */
const QKeySequence & XinxDockWidget::getShortcut() const
{
	return _shortcut;
}

//! This method is called when the title is modified to update the button title.
void XinxDockWidget::updateDockTitle()
{
	if (_dock && _dock->button())
	{
		_dock->button()->setText(windowTitle());
	}
}
