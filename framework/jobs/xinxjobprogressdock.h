/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#ifndef XINXJOBPROGRESSDOCK_H
#define XINXJOBPROGRESSDOCK_H

#include <application/xinxdockwidget.h>

class XinxJob;
class PrivateXinxJobProgressDock;

class XinxJobProgressDock : public XinxDockWidget
{
	Q_OBJECT
public:
	XinxJobProgressDock(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~XinxJobProgressDock();

private slots:
	void slotJobAdded(XinxJob * job);
	void slotJobEnded(XinxJob * job);
	void slotJobDelayed(XinxJob * job);
private:
	QScopedPointer<PrivateXinxJobProgressDock> d;
	friend class PrivateXinxJobProgressDock;
};

#endif // XINXJOBPROGRESSDOCK_H
