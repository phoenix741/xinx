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
#ifndef XINXJOBWIDGETLIST_H
#define XINXJOBWIDGETLIST_H

#include <core/lib-config.h>
#include <QScrollArea>

class PrivateXinxJobWidgetList;
class XinxJobWidget;
class XinxJob;

class LIBEXPORT XinxJobWidgetList : public QScrollArea
{
	Q_OBJECT
public:
	explicit XinxJobWidgetList(QWidget *parent = 0);
	virtual ~XinxJobWidgetList();

	XinxJobWidget* addJobItem(XinxJob * job);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;
protected:
	virtual void resizeEvent(QResizeEvent *event);
private:
	QScopedPointer<PrivateXinxJobWidgetList> d;
};

#endif // XINXJOBWIDGETLIST_H
