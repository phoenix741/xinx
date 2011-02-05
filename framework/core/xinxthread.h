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
#ifndef _XINXTHREAD_H_
#define _XINXTHREAD_H_

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>
#include <core/xinxsingleton.h>

// Qt header
#include <QThread>
#include <QMutex>

class XinxThread;

/*!
 * The XinxThreadManager class is a debugger class to count the number of XinxThread object
 * created and the number of XinxThread object in the threadrun method.
 *
 * To be used, each created thread must be a XinxThread (derivated from a QThread).
 */
class LIBEXPORT XinxThreadManager : public XinxLibSingleton<XinxThreadManager>
{
	Q_OBJECT
public:
	/// Destroy the mananger
	virtual ~XinxThreadManager();

	/// Get the number of thread running
	int getThreadCount() const;
	/// Get the number of thread class object created
	int getThreadClassCount() const;
signals:
	/// Signal emited when the number of running thread, or object change.
	void threadCountChange();
private:
	XinxThreadManager();

	int m_threadCount, m_threadClassCount;
	QMutex m_mutex;

	friend class XinxThread;
	friend class XinxLibSingleton<XinxThreadManager>;
};

/*!
 * This class is used to create thread in XINX and count the number of created Thread and
 * the number of running thread.
 */
class LIBEXPORT XinxThread : public QThread
{
	Q_OBJECT
public:
	/// Create a new thread object and increment XinxThreadManager::getThreadClassCount()
	explicit XinxThread(QObject * parent = 0);
	/// Destroy a thread and decrement XinxThreadManager::getThreadClassCount()
	virtual ~XinxThread();
protected:
	/*!
	 * This function is derivated to incremente and decremente  XinxThreadManager::getThreadCount() before
	 * and after call of threadrun().
	 */
	virtual void run();
	/*!
	 * This is the new function to implemente for running thread.
	 */
	virtual void threadrun() = 0;
};

#endif /* _XINXTHREAD_H_ */
