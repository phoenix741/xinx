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

#include "xinxjobwidget.h"
#include "xinxjob.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>

#define MAX_LABEL_WIDTH 650

/* PrivateXinxJobWidget */

class PrivateXinxJobWidget
{
public:
	PrivateXinxJobWidget();

	XinxJob * _job;

	XinxJobWidget * _widget;
	QVBoxLayout * _widget_layout;

	QFrame * _frame;
	QPushButton * _cancel_button;
	QLabel * _item_label;
	QLabel * _item_status;
	QProgressBar * _progressBar;

	void init();
};

PrivateXinxJobWidget::PrivateXinxJobWidget() : _cancel_button(0)
{

}

void PrivateXinxJobWidget::init()
{
	_widget_layout = new QVBoxLayout(_widget);
	_widget_layout->setSpacing(2);
	_widget_layout->setMargin(2);
	_widget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
	_widget->setLayout(_widget_layout);

	_frame = new QFrame(_widget);
	_frame->setFrameShape(QFrame::HLine);
	_frame->setFrameShadow(QFrame::Raised);
	_frame->show();

	_widget_layout->setStretchFactor(_frame, 3);
	_widget_layout->addWidget(_frame);

	QHBoxLayout * h = new QHBoxLayout;
	h->setSpacing(5);
	_widget_layout->addLayout(h);

	_item_label = new QLabel(_widget->fontMetrics().elidedText(_job->description(), Qt::ElideRight, MAX_LABEL_WIDTH), _widget);
	h->addWidget(_item_label);

	_progressBar = new QProgressBar(_widget);
	_progressBar->setMaximum(_job->maximum());
	_progressBar->setValue(_job->progress());
	h->addWidget(_progressBar);

	if (_job->canBeCanceled())
	{
		_cancel_button = new QPushButton(QIcon(":/images/stop_search.png"), QString(), _widget);
		_cancel_button ->setToolTip(XinxJobWidget::tr("Cancel this operation."));
		QObject::connect(_cancel_button , SIGNAL(clicked()), _widget, SLOT(slotItemCanceled()));
		h->addWidget(_cancel_button);
	}

	h = new QHBoxLayout;
	h->setSpacing(5);
	_widget_layout->addLayout(h);

	_item_status = new QLabel(_widget->fontMetrics().elidedText(_job->status(), Qt::ElideRight, MAX_LABEL_WIDTH), _widget);
	h->layout()->addWidget(_item_status);
}

/* XinxJobWidget */

XinxJobWidget::XinxJobWidget(XinxJob * job, QWidget *parent) : QFrame(parent), d(new PrivateXinxJobWidget)
{
	d->_widget = this;
	d->_job = job;

	d->init();

	connect(job, SIGNAL(setDescription(QString)), this, SLOT(setLabel(QString)));
	connect(job, SIGNAL(setStatus(QString)), this, SLOT(setStatus(QString)));
	connect(job, SIGNAL(setProgress(int,int)), this, SLOT(setProgress(int, int)));
}

XinxJobWidget::~XinxJobWidget()
{

}

void XinxJobWidget::hideHLine()
{
	d->_frame->hide();
}

void XinxJobWidget::setProgress(int progress, int maximum)
{
	d->_progressBar->setMaximum(maximum);
	d->_progressBar->setValue(progress);
}

void XinxJobWidget::setLabel(const QString& label)
{
	d->_item_label->setText(fontMetrics().elidedText(label, Qt::ElideRight, MAX_LABEL_WIDTH));
}

void XinxJobWidget::setStatus(const QString& status)
{
	d->_item_status->setText(fontMetrics().elidedText(status, Qt::ElideRight, MAX_LABEL_WIDTH));
}

XinxJob* XinxJobWidget::job() const
{
	return d->_job;
}

void XinxJobWidget::slotItemCanceled()
{
	if (d->_job)
		d->_job->abort();
}
