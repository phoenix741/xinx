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

// Xinx header
#include <rcs/rcs.h>
#include "ui_logform.h"

// Qt header
#include <QDockWidget>

class PrivateLogDockWidget;

class LogDockWidget : public QDockWidget {
	Q_OBJECT
public:
	LogDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	LogDockWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	virtual ~LogDockWidget();

	void init();
	void end();
public slots:
	void log( RCS::rcsLog niveau, const QString & info );
	void find( const QString & filename, const QString & text, int line );
signals:
	void open( const QString & filename, int line );
protected slots:
	void on_m_searchTreeWidget_doubleClicked( const QModelIndex & index );;
private:
	Ui::LogWidget * m_logwidget;
};
#endif





