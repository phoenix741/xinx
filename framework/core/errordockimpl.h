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

#ifndef ERRORDOCKWIDGETIMPL_H
#define ERRORDOCKWIDGETIMPL_H
#pragma once

// Xinx header
#include "ui_errordock.h"

// Qt header
#include <QDockWidget>

class ErrorDockWidgetImpl: public QWidget
{
	Q_OBJECT
public:
	ErrorDockWidgetImpl(QWidget * parent = 0);
	virtual ~ErrorDockWidgetImpl();

	void setDock(QDockWidget * dock);
public slots:
	void updateErrors();
signals:
	void open(const QString & filename, int line);
protected slots:
	void doubleClicked(const QModelIndex & index);;
private:
	Ui::ErrorDockWidget * _widget;
	QDockWidget * m_dock;
};
#endif /* ERRORDOCKWIDGETIMPL_H */





