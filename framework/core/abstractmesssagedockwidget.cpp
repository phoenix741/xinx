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

#include "abstractmesssagedockwidget.h"
#include <QDockWidget>
#include "xinxconfig.h"

/*!
 * \ingroup gui
 * \class AbstractMessageDockWidget
 * \since 0.10.2
 *
 * \brief Abstract class to use to make a dock widget.
 *
 * You must subclass this class when you want to make a new dock for XINX that implement << notifications >> like the error dock or
 * search dock. This class contains method to show notification and method to show title.
 */

/*!
* \fn void AbstractMessageDockWidget::open(const QString & filename, int line)
* \brief Signal emited when the dock want to open a file (like content view, project view, ...)
*/

/*!
 * \fn virtual bool AbstractMessageDockWidget::automatcallyShow() const = 0
 * \brief Indicator that tell to show the dock when notification changed
 *
 * This method is used to tell to XINX if the dock must be automatically show when the notification changed.
 * @return true if the dock is automatically show or false is if the dock must be keep hidden.
 */

/*!
 * \fn virtual bool AbstractMessageDockWidget::automaticallyClose() const = 0
 * \brief Indicator that tell to close the dock when notification changed
 *
 * This method is used to tell to XINX if the dock must be automatically closed when the notification is set to 0.
 * @return true if the dock is automatically closed or else false.
 */

//! Create a new dock with the parent \p parent.
AbstractMessageDockWidget::AbstractMessageDockWidget(QWidget* parent): XinxDockWidget(parent), _notification(0)
{
}

//! Destroy the object
AbstractMessageDockWidget::~AbstractMessageDockWidget()
{

}

/*!
 * \brief Show the window title in the button.
 *
 * This method is subclassed to show the number of notification in the title.
 */
QString AbstractMessageDockWidget::windowTitle() const
{
	if (_notification == 0)
	{
		return XinxDockWidget::windowTitle();
	}
	return XinxDockWidget::windowTitle() + QString(" - %1").arg(_notification);
}

/*!
 * \brief Change the number of notification to show.
 *
 * Set the number of notification to \p notification. When a notification is changed,
 * the signal windowChanged() is emited.
 *
 * \sa notifyCount(), windowChanged()
 */
void AbstractMessageDockWidget::setNotifyCount(int notification)
{
	if (_notification != notification)
	{
		_notification = notification;

		emit windowChanged();
	}
}

/*!
 * \brief Return the number of notification.
 *
 * Return the number of notification of the dock (the number show in the title). The notification is used
 * by search dock, and by error dock.
 *
 * \sa setNotifyCount()
 */
int AbstractMessageDockWidget::notifyCount() const
{
	return _notification;
}
