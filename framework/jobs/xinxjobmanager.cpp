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
#include <QStringList>

class PrivateXinxJobManager
{
public:
	QThreadPool * _pool;
	QAtomicInt _count, _progress_value, _progress_max;

	QScopedPointer<QMutex> _waiting_job_mutex;
	QList<XinxJob*> _waiting_job_list;

	QScopedPointer<QMutex> _descriptions_mutex;
	QHash<XinxJob*,QString> _descriptions_list;
};

/* XinxJobManager */

XinxJobManager::XinxJobManager()
{
	d = new PrivateXinxJobManager;
	d->_descriptions_mutex.reset(new QMutex);
	d->_waiting_job_mutex.reset(new QMutex);
	d->_pool = new QThreadPool;
}

XinxJobManager::~XinxJobManager()
{
	d->_pool->waitForDone();

	// FIXME : Que faire si le pool tourne encore
	delete d->_pool;
	delete d;
}

void XinxJobManager::addJob(XinxJob* job)
{
	if (job->isUnique())
	{
		QMutexLocker locker(d->_waiting_job_mutex.data());
		foreach(XinxJob * jb, d->_waiting_job_list)
		{
			if (jb->uniqKey() == job->uniqKey())
			{
				emit jobCanceled(job);
				return ;
			}
		}
	}

	connect(job, SIGNAL(jobStarting()), this, SLOT(slotJobStarting()), Qt::BlockingQueuedConnection);
	connect(job, SIGNAL(jobEnding()), this, SLOT(slotJobEnding()));
	if (d->_count == 0)
	{
		d->_progress_max = 1;
		d->_progress_value = 0;
	}
	else
	{
		d->_progress_max.ref();
	}
	d->_count.ref();

	qDebug() << "Add job " << job->description() << " (" << d->_progress_max << ")";

	{
		QMutexLocker locker(d->_waiting_job_mutex.data());
		d->_waiting_job_list.append(job);
	}
	emit jobAdded(job);
	d->_pool->start(job, job->priority());
	//emit jobStarted(countTotalJob (), countRunningJob ());
	emit progressRangeChanged(0, d->_progress_max);
	emit progressValueChanged(d->_progress_value);
}

void XinxJobManager::slotJobStarting()
{
	XinxJob * job = qobject_cast<XinxJob*> (sender());

	{
		QMutexLocker locker(d->_descriptions_mutex.data());
		d->_descriptions_list.insert(job, job->description());
	}
	{
		QMutexLocker locker(d->_waiting_job_mutex.data());
		d->_waiting_job_list.removeAll(job);
	}

	qDebug() << "Starting job (" << countRunningJob() << "/" << countTotalJob() << ")";
	emit jobStarted(job);
}

void XinxJobManager::slotJobEnding()
{
	XinxJob * job = qobject_cast<XinxJob*>(sender());

	{
		QMutexLocker locker(d->_descriptions_mutex.data());
		d->_descriptions_list.remove(job);
	}

	d->_count.deref();
	d->_progress_value.ref();
	emit progressValueChanged(d->_progress_value);
	emit jobEnded(job);
	if (d->_count == 0)
	{
		emit allJobEnded();
	}

	qDebug() << "Remove job (" << countRunningJob() << "/" << countTotalJob() << ")";

	if (job->isManagerDelete())
	{
		job->deleteLater();
	}
}

QStringList XinxJobManager::descriptions() const
{
	QMutexLocker locker(d->_descriptions_mutex.data());
	return d->_descriptions_list.values();
}

int XinxJobManager::countRunningJob()
{
	return XinxJob::countRunningJob();
}

int XinxJobManager::countTotalJob()
{
	return d->_count;
}
