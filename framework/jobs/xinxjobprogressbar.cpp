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

#include "xinxjobprogressbar.h"
#include "xinxjobmanager.h"
#include <QProgressBar>
#include <QToolButton>
#include <QHBoxLayout>

/* PrivateXinxJobProgressBar */

class PrivateXinxJobProgressBar
{
public:
	QProgressBar * _progress_bar;
};

/* XinxJobProgressBar */

XinxJobProgressBar::XinxJobProgressBar(QWidget *parent) : QWidget(parent), d(new PrivateXinxJobProgressBar)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

	d->_progress_bar = new QProgressBar(this);

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget(d->_progress_bar);
	hbox->setSpacing(0);
	hbox->setMargin(0);
	setLayout(hbox);

	connect(XinxJobManager::self(), SIGNAL(allJobEnded()), this, SLOT(hide()));
	connect(XinxJobManager::self(), SIGNAL(progressValueChanged(int)), this, SLOT(show()));

	connect(XinxJobManager::self(), SIGNAL(progressRangeChanged(int,int)), d->_progress_bar, SLOT(setRange(int,int)));
	connect(XinxJobManager::self(), SIGNAL(progressValueChanged(int)), d->_progress_bar, SLOT(setValue(int)));

	connect(XinxJobManager::self(), SIGNAL(progressRangeChanged(int,int)), this, SLOT(updateProgressBar()));
	connect(XinxJobManager::self(), SIGNAL(progressValueChanged(int)), this, SLOT(updateProgressBar()));
	connect(XinxJobManager::self(), SIGNAL(progressValueChanged(int)), this, SLOT(updateProgressBar()));
}

XinxJobProgressBar::~XinxJobProgressBar()
{

}

void XinxJobProgressBar::updateProgressBar()
{
	d->_progress_bar->setToolTip(XinxJobManager::self()->descriptions().join("\n"));
}
