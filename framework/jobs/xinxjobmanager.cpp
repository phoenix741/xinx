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

#include "xinxjobmanager.h"
#include "xinxjob.h"

#include <QThreadPool>
#include <QDebug>

class PrivateXinxJobManager
{
public:
	QThreadPool * _pool;
	QAtomicInt _count, _progress_value, _progress_max;
};

/* XinxJobManager */

XinxJobManager::XinxJobManager()
{
	d = new PrivateXinxJobManager;
	d->_pool = new QThreadPool;
}

XinxJobManager::~XinxJobManager()
{
	// FIXME : Que faire si le pool tourne encore
	delete d->_pool;
	delete d;
}

void XinxJobManager::addJob(XinxJob* job)
{
	connect(job, SIGNAL(jobStarting()), this, SLOT(slotJobStarting()));
	connect(job, SIGNAL(destroyed(QObject*)), this, SLOT(removeJob()));
	if (d->_count == 0)
	{
		d->_progress_max = 1;
		d->_progress_value = 0;
	}
	else
	{
		d->_progress_max.ref ();
	}
	d->_count.ref();

	qDebug() << "Add job " << job->description () << " (" << d->_progress_max << ")";

	d->_pool->start(job);
	//emit jobStarted(countTotalJob (), countRunningJob ());
	emit progressRangeChanged (0, d->_progress_max);
	emit progressValueChanged (d->_progress_value);
}

void XinxJobManager::slotJobStarting()
{
	qDebug() << "Starting job (" << countRunningJob () << "/" << countTotalJob () << ")";
	emit jobStarted (countTotalJob (), countRunningJob ());
}

void XinxJobManager::removeJob()
{
	d->_count.deref();
	d->_progress_value.ref ();
	emit progressValueChanged (d->_progress_value);
	emit jobEnded (countTotalJob (), countRunningJob ());
	if (d->_count == 0)
	{
		emit allJobEnded();
	}

	qDebug() << "Remove job (" << countRunningJob () << "/" << countTotalJob () << ")";
}

int XinxJobManager::countRunningJob()
{
	return XinxJob::countRunningJob();
}

int XinxJobManager::countTotalJob()
{
	return d->_count;
}
