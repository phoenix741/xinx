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

#include "xinxjobprogressdock.h"
#include "xinxjobwidgetlist.h"
#include "xinxjobmanager.h"
#include "xinxjobwidget.h"
#include "xinxjob.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QMap>
#include <QIcon>

/* PrivateXinxJobProgressDock */

class PrivateXinxJobProgressDock
{
public:
	XinxJobWidgetList* _scroll_view;
	QMap<const XinxJob*, XinxJobWidget*> _list_items;
};

/* XinxJobProgressDock */

XinxJobProgressDock::XinxJobProgressDock(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), d(new PrivateXinxJobProgressDock)
{
	setLayout(new QHBoxLayout);
	setWindowIcon(QIcon(":/images/run.png"));
	setWindowTitle(tr("Progress"));

	d->_scroll_view = new XinxJobWidgetList(this);
	layout()->addWidget(d->_scroll_view);

	connect(XinxJobManager::self(), SIGNAL(jobAdded(XinxJob*)), this, SLOT(slotJobAdded(XinxJob*)));
	connect(XinxJobManager::self(), SIGNAL(jobEnded(XinxJob*)), this, SLOT(slotJobEnded(XinxJob*)));
}

XinxJobProgressDock::~XinxJobProgressDock()
{

}

void XinxJobProgressDock::slotJobAdded(XinxJob* job)
{
	XinxJobWidget * ji = d->_scroll_view->addJobItem(job);
	if ( ji )
	{
		d->_list_items.insert(job, ji);
	}
}

void XinxJobProgressDock::slotJobEnded(XinxJob* job)
{
	if (d->_list_items.contains(job))
	{
		XinxJobWidget *ji = d->_list_items[job];
		d->_list_items.remove(job);

		QTimer::singleShot(3000, ji, SLOT(deleteLater()));
	}
}

