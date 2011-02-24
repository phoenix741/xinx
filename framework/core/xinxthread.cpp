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

// Xinx header
#include "core/xinxthread.h"
#include "core/exceptions.h"
#include "core/xinxcore.h"

/* XinxThreadManager */

XinxThreadManager::XinxThreadManager() : m_threadCount(0), m_threadClassCount(0)
{

}

XinxThreadManager::~XinxThreadManager()
{

}

int XinxThreadManager::getThreadCount() const
{
	return m_threadCount;
}

int XinxThreadManager::getThreadClassCount() const
{
	return m_threadClassCount;
}

/* XinxThread */

XinxThread::XinxThread(QObject * parent) : QThread(parent)
{
	QMutexLocker locker(&(XinxThreadManager::self()->m_mutex));
	XinxThreadManager::self()->m_threadClassCount++;
	emit XinxThreadManager::self()->threadCountChange();
}

XinxThread::~XinxThread()
{
	QMutexLocker locker(&(XinxThreadManager::self()->m_mutex));
	XinxThreadManager::self()->m_threadClassCount--;
	emit XinxThreadManager::self()->threadCountChange();
}

void XinxThread::run()
{
	ExceptionManager::installSignalHandler ();
	{
		QMutexLocker locker(&(XinxThreadManager::self()->m_mutex));
		XinxThreadManager::self()->m_threadCount++;
		emit XinxThreadManager::self()->threadCountChange();
	}

	try
	{
		threadrun();
	}
	catch (const std::exception & e)
	{
		qCritical (e.what());
	}
	catch (...)
	{
		qCritical ("Generic Exception");
	}

	{
		QMutexLocker locker(&(XinxThreadManager::self()->m_mutex));
		XinxThreadManager::self()->m_threadCount--;
		emit XinxThreadManager::self()->threadCountChange();
	}
}
