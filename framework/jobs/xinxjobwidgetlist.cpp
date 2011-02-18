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

#include "xinxjobwidgetlist.h"
#include "xinxjobwidget.h"
#include <QVBoxLayout>
#include <QScrollBar>

/* PrivateXinxJobWidgetList */

class PrivateXinxJobWidgetList
{
public:
	QWidget * _big_box;
};

/* XinxJobWidgetList */

XinxJobWidgetList::XinxJobWidgetList(QWidget *parent) : QScrollArea(parent), d(new PrivateXinxJobWidgetList)
{
	setFrameStyle(NoFrame);

	d->_big_box = new QWidget(this);
	d->_big_box->setLayout(new QVBoxLayout);

	setWidget(d->_big_box);
	setWidgetResizable(true);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

XinxJobWidgetList::~XinxJobWidgetList()
{

}

XinxJobWidget* XinxJobWidgetList::addJobItem(XinxJob * job)
{
	XinxJobWidget *ti = new XinxJobWidget(job, d->_big_box);
	d->_big_box->layout()->addWidget(ti);

	resize(d->_big_box->width(), d->_big_box->height());

	return ti;
}

void XinxJobWidgetList::resizeEvent(QResizeEvent *event)
{
	// Tell the layout in the parent (progressdialog) that our size changed
	updateGeometry();

	QSize sz = parentWidget()->sizeHint();
	int currentWidth = parentWidget()->width();

	// Don't resize to sz.width() every time when it only reduces a little bit
	if (currentWidth < sz.width() || currentWidth > sz.width() + 100)
	{
		currentWidth = sz.width();
	}

	parentWidget()->resize(currentWidth, sz.height());

	QScrollArea::resizeEvent(event);
}

QSize XinxJobWidgetList::sizeHint() const
{
	return minimumSizeHint();
}

QSize XinxJobWidgetList::minimumSizeHint() const
{
	int f = 2 * frameWidth();
	// Make room for a vertical scrollbar in all cases, to avoid a horizontal one
	int vsbExt = verticalScrollBar()->sizeHint().width();
	int minw = topLevelWidget()->width() / 3;
	int maxh = topLevelWidget()->height() / 2;
	QSize sz(d->_big_box->minimumSizeHint());
	sz.setWidth(qMax(sz.width(), minw) + f + vsbExt);
	sz.setHeight(qMin(sz.height(), maxh) + f);
	return sz;
}

