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

AbstractMessageDockWidget::AbstractMessageDockWidget(QWidget* parent): XinxDockWidget(parent), _notification(0)
{
}

AbstractMessageDockWidget::~AbstractMessageDockWidget()
{

}

QString AbstractMessageDockWidget::windowTitle() const
{
	if (_notification == 0)
	{
		return XinxDockWidget::windowTitle();
	}
	return XinxDockWidget::windowTitle() + QString(" - %1").arg(_notification);
}

void AbstractMessageDockWidget::setNotifyCount(int notification)
{
	if (_notification != notification)
	{
		_notification = notification;

		emit windowChanged();
	}
}

int AbstractMessageDockWidget::notifyCount() const
{
	return _notification;
}
