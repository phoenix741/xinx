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

#ifndef DICTIONARYDOCKWIDGET_H
#define DICTIONARYDOCKWIDGET_H
#pragma once

// Xinx header
#include <ui_dictionarydockwidget.h>

// Qt header
#include <QDockWidget>

class DictionaryModel;
namespace ContentView2
{
class Parser;
class File;
}

class DictionaryDockWidgetImpl : public QDockWidget, private Ui::DictionaryDockWidget
{
	Q_OBJECT
public:
	DictionaryDockWidgetImpl(QWidget * parent = 0);
	virtual ~DictionaryDockWidgetImpl();
private slots:
	void on_m_filterLine_textChanged(QString filter);
	void projectChanged();
	void update(const ContentView2::File & file);
private:
	virtual void timerEvent(QTimerEvent * event);

};

#endif // DICTIONARYDOCKWIDGET_H
