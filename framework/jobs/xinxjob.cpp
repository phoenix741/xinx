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

#include "xinxjob.h"
#include <core/exceptions.h>

// Qt header
#include <QDebug>

/* Static member */

QAtomicInt XinxJob::_count_job;

/* XinxJob */

XinxJob::XinxJob() : _state(JOB_WAIT), _manager_delete(true)
{
	setAutoDelete(false);
}

XinxJob::~XinxJob()
{

}

bool XinxJob::isUnique() const
{
	return false;
}

QString XinxJob::uniqKey() const
{
	return QString();
}

void XinxJob::setManagerDelete(bool value)
{
	_manager_delete = value;
}

bool XinxJob::managerDelete() const
{
	return _manager_delete;
}

QString XinxJob::status() const
{
	switch (_state)
	{
	case JOB_WAIT:
		return tr("Waiting ...");
	case JOB_RUNNING:
		return tr("Running ...");
	case JOB_ENDING:
		return tr("Cleaning ...");
	default:
		return tr("Unknown");
	}
}

int XinxJob::maximum() const
{
	switch (_state)
	{
	case JOB_WAIT:
	case JOB_ENDING:
		return 100;
	default:
		return 0;
	}
}

int XinxJob::progress() const
{
	switch (_state)
	{
		case JOB_WAIT:
			return 0;
		case JOB_ENDING:
			return 100;
		default:
			return 0;
	}
}

bool XinxJob::canBeCanceled() const
{
	return false;
}

int XinxJob::countRunningJob()
{
	return _count_job;
}

bool XinxJob::isRunning()
{
	return (_state == JOB_RUNNING) || (_state == JOB_ABORTING);
}

void XinxJob::setState(int state)
{
	_state = state;
	emit setStatus(status());
	emit setProgress(progress(), maximum());
}

void XinxJob::run()
{
	ExceptionManager::installSignalHandler ();

	emit jobStarting();

	_count_job.ref();
	setState(JOB_RUNNING);

	try
	{
		startJob();
	}
	catch (const std::exception & e)
	{
		qCritical () << e.what();
	}
	catch (...)
	{
		qCritical () << tr("Unknown exception");
	}
	_count_job.deref();
	setState(JOB_ENDING);

	emit jobEnding();
}

void XinxJob::abort()
{
	setState(JOB_ABORTING);
}
