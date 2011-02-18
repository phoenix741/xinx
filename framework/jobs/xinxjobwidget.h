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
#ifndef XINXJOBWIDGET_H
#define XINXJOBWIDGET_H

#include <core/lib-config.h>
#include <QFrame>

class PrivateXinxJobWidget;
class XinxJob;

class LIBEXPORT XinxJobWidget : public QFrame
{
	Q_OBJECT
public:
	explicit XinxJobWidget(XinxJob * job, QWidget *parent = 0);
	virtual ~XinxJobWidget();

	void hideHLine();
	void setProgress(int progress, int maximum);
	void setLabel(const QString& value);
	void setStatus(const QString& value);
	XinxJob* job() const;

signals:

private slots:
	void slotItemCanceled();

private:
	QScopedPointer<PrivateXinxJobWidget> d;
	friend class PrivateXinxJobWidget;
};

#endif // XINXJOBWIDGET_H
