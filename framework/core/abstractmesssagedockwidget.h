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
#ifndef ABSTRACTMESSSAGEDOCKWIDGET_H
#define ABSTRACTMESSSAGEDOCKWIDGET_H

#include <core/lib-config.h>
#include <application/xinxdockwidget.h>

class LIBEXPORT AbstractMessageDockWidget : public XinxDockWidget
{
	Q_OBJECT
public:
	AbstractMessageDockWidget(QWidget * parent = 0);
	virtual ~AbstractMessageDockWidget();

	virtual bool automatcallyShow() const = 0;
	virtual bool automaticallyClose() const = 0;

	void setNotifyCount(int notification);
	int notifyCount() const;

	virtual QString windowTitle() const;
signals:
	void open(const QString & filename, int line);

private:
	int _notification;
};

#endif // ABSTRACTMESSSAGEDOCKWIDGET_H
