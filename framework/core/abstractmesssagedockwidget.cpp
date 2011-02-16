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
#include <dtoolview.h>
#include <dviewbutton.h>

AbstractMesssageDockWidget::AbstractMesssageDockWidget(QWidget* parent): QWidget(parent), _dock(0), _automatically_show(false), _automatically_close(false), _notification(0)
{
	// Read automatically Show and Close from config
	_automatically_close = XINXConfig::self()->config().editor.closeErrorDockAutomatically;
}

AbstractMesssageDockWidget::~AbstractMesssageDockWidget()
{

}

void AbstractMesssageDockWidget::setDock(DToolView* dock)
{
	if (_dock != dock)
	{
		int nc = notifyCount();
		setNotifyCount(0);
		_dock = dock;
		setNotifyCount(nc);
	}
}

DToolView* AbstractMesssageDockWidget::dock()
{
	return _dock;
}

void AbstractMesssageDockWidget::setAutomaticallyShow(bool value)
{
	_automatically_show = value;
}

bool AbstractMesssageDockWidget::automatcallyShow() const
{
	return _automatically_show;
}

void AbstractMesssageDockWidget::setAutomaticallyClose(bool value)
{
	_automatically_close = value;
}

bool AbstractMesssageDockWidget::automaticallyClose() const
{
	return _automatically_close;
}

void AbstractMesssageDockWidget::setNotifyCount(int notification)
{
	if (_notification != notification)
	{
		_notification = notification;
		if (_dock && _notification == 0)
		{
			_dock->button()->setText(windowTitle());
		}
		else
		{
			_dock->button()->setText(windowTitle() + QString("(%1)").arg(_notification));
		}
	}
}

int AbstractMesssageDockWidget::notifyCount() const
{
	return _notification;
}
