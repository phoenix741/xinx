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

#include <QWidget>

class DToolView;

class AbstractMesssageDockWidget : public QWidget
{
	Q_OBJECT
public:
	AbstractMesssageDockWidget(QWidget * parent = 0);
	virtual ~AbstractMesssageDockWidget();

	void setAutomaticallyShow(bool value);
	bool automatcallyShow() const;

	void setAutomaticallyClose(bool value);
	bool automaticallyClose() const;

	void setNotifyCount(int notification);
	int notifyCount() const;

	void setDock(DToolView * dock);
	DToolView * dock();
signals:
	void open(const QString & filename, int line);

private:
	DToolView * _dock;
    bool _automatically_show;
    bool _automatically_close;
    int _notification;
};

#endif // ABSTRACTMESSSAGEDOCKWIDGET_H
