/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef RCSLOGDIALOGIMPL_H
#define RCSLOGDIALOGIMPL_H
#pragma once

// Xinx header
#include <rcs/rcs.h>
#include "ui_rcsdock.h"

// Qt header
#include <QDockWidget>

class RCSLogDockWidgetImpl : public QWidget
{
	Q_OBJECT
public:
	RCSLogDockWidgetImpl(QWidget * parent = 0);
	virtual ~RCSLogDockWidgetImpl();

	void init();
	void end();

	void setDock(QDockWidget * dock);
public slots:
	void log(RCS::rcsLog niveau, const QString & info);
private:
	Ui::RCSDockWidget * _widget;
	QDockWidget * m_dock;
};
#endif





