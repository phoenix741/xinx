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

#ifndef SEARCHDOCKIMPL_H
#define SEARCHDOCKIMPL_H
#pragma once

// Xinx header
#include "ui_searchdock.h"

// Qt header
#include <QDockWidget>

class SearchDockWidgetImpl : public QWidget
{
	Q_OBJECT
public:
	SearchDockWidgetImpl(QWidget * parent = 0);
	virtual ~SearchDockWidgetImpl();

	void init();
	void end();

	void setDock(QDockWidget * dock);
	QDockWidget * dock() const;
public slots:
	void find(const QString & filename, const QString & text, int line);
signals:
	void open(const QString & filename, int line);
protected slots:
	void doubleClicked(const QModelIndex & index);;
private:
	Ui::SearchDockWidget * _widget;
	QDockWidget * m_dock;
};

#endif /* SEARCHDOCKIMPL_H */





