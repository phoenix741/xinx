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

#include "xinxjoboverlayprogress.h"
#include "xinxjobwidgetlist.h"
#include "xinxjobwidget.h"
#include "xinxjob.h"
#include <QLayout>
#include <QMap>
#include <QCloseEvent>
#include <QTimer>
#include "xinxjobmanager.h"

/* PrivateXinxJobOverlayProgress */

class PrivateXinxJobOverlayProgress
{
public:
	PrivateXinxJobOverlayProgress();

	void reposition();

	QWidget * _align_widget, * _widget;

	XinxJobWidgetList* _scroll_view;
	QMap<const XinxJob*, XinxJobWidget*> _list_items;
};

PrivateXinxJobOverlayProgress::PrivateXinxJobOverlayProgress() : _align_widget(0)
{

}

void PrivateXinxJobOverlayProgress::reposition()
{
	if (!_align_widget)
	{
		return;
	}

	QPoint p;
	p.setX(_align_widget->width() - _widget->width());
	p.setY(-_widget->height());

	QPoint pTopLevel = _align_widget->mapTo(_widget->topLevelWidget(), p);
	QPoint pParent = _widget->parentWidget()->mapFrom(_widget->topLevelWidget(), pTopLevel);

	_widget->move(pParent);
}

/* XinxJobOverlayProgress */

XinxJobOverlayProgress::XinxJobOverlayProgress(QWidget* alignWidget, QWidget* parent) : QFrame(parent), d(new PrivateXinxJobOverlayProgress)
{
	setFrameStyle(QFrame::Panel | QFrame::Sunken);   // QFrame
	setAutoFillBackground(true);

	setLayout(new QHBoxLayout);

	d->_widget = this;
	d->_scroll_view = new XinxJobWidgetList(this);
	layout()->addWidget(d->_scroll_view);

	setAlignWidget(alignWidget);

	connect(XinxJobManager::self(), SIGNAL(jobAdded(XinxJob*)), this, SLOT(slotJobAdded(XinxJob*)));
	connect(XinxJobManager::self(), SIGNAL(jobEnded(XinxJob*)), this, SLOT(slotJobEnded(XinxJob*)));
}

XinxJobOverlayProgress::~XinxJobOverlayProgress()
{

}

void XinxJobOverlayProgress::slotJobAdded(XinxJob* job)
{
	XinxJobWidget * ji = d->_scroll_view->addJobItem(job);
	if ( ji )
	{
		d->_list_items.insert(job, ji);
	}

	if ( ! d->_list_items.empty() )
	{
		show();
	}
}

void XinxJobOverlayProgress::slotJobEnded(XinxJob* job)
{
	if (d->_list_items.contains(job))
	{
		XinxJobWidget *ji = d->_list_items[job];
		d->_list_items.remove(job);
		
		QTimer::singleShot(3000, ji, SLOT(deleteLater()));
	}

	if ( d->_list_items.empty() )
	{
		hide();
	}
}

void XinxJobOverlayProgress::closeEvent(QCloseEvent* e)
{
	e->accept();
	hide();
}

QWidget * XinxJobOverlayProgress::alignWidget() const
{
	return d->_align_widget;
}

void XinxJobOverlayProgress::setAlignWidget(QWidget * alignWidget)
{
	if (alignWidget == d->_align_widget)
		return;

	if (d->_align_widget)
		d->_align_widget->removeEventFilter(this);

	d->_align_widget = alignWidget;

	if (d->_align_widget)
		d->_align_widget->installEventFilter(this);

	d->reposition();
}

void XinxJobOverlayProgress::resizeEvent(QResizeEvent* ev)
{
	d->reposition();

	QFrame::resizeEvent(ev);
}

bool XinxJobOverlayProgress::eventFilter(QObject* o, QEvent* e)
{
	if (o == d->_align_widget && (e->type() == QEvent::Move || e->type() == QEvent::Resize))
	{
		d->reposition();
	}

	return QFrame::eventFilter(o,e);
}

