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

#ifndef RCSLOGDIALOGIMPL_H
#define RCSLOGDIALOGIMPL_H
#pragma once

// Xinx header
#include <versioncontrol/rcs.h>
#include "ui_rcsdock.h"
#include <versioncontrol/versioncontrolmanager.h>
#include <core/abstractmesssagedockwidget.h>

// Qt header
#include <QDockWidget>

class RCSLogDockWidgetImpl : public AbstractMessageDockWidget, public VersionControl::IManagerUI
{
	Q_OBJECT
public:
	RCSLogDockWidgetImpl(QWidget * parent = 0);
	virtual ~RCSLogDockWidgetImpl();

	void init();
	void end();

    virtual bool automaticallyClose() const;

	void startVersionControlOperations();
	void logVersionControlOperation(RCS::rcsLog niveau, const QString & info);
	void stopVersionControlOperations();
private slots:
	void timeout();
private:
	Ui::RCSDockWidget * _widget;
	bool _rcsVisible;
	QTimer * _timer;
};
#endif





