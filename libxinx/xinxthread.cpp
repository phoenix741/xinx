/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "xinxthread.h"
#include "exceptions.h"
#include "xinxcore.h"

/* Static member */

XinxThreadManager * XinxThreadManager::s_self = NULL;

/* XinxThreadManager */

XinxThreadManager::XinxThreadManager() : m_threadCount( 0 ), m_threadClassCount( 0 ) {

}

XinxThreadManager::~XinxThreadManager() {
	if( this == s_self )
		s_self = NULL;
}

int XinxThreadManager::getThreadCount() const {
	return m_threadCount;
}

int XinxThreadManager::getThreadClassCount() const {
	return m_threadClassCount;
}

XinxThreadManager * XinxThreadManager::self() {
	if( ! s_self ) {
		s_self = new XinxThreadManager();
		XINXStaticDeleter::self()->add( s_self );
	}

	return s_self;
}

/* XinxThread */

XinxThread::XinxThread( QObject * parent ) : QThread( parent ) {
	QMutexLocker locker( &(XinxThreadManager::self()->m_mutex) );
	XinxThreadManager::self()->m_threadClassCount++;
	emit XinxThreadManager::self()->threadCountChange();
}

XinxThread::~XinxThread() {
	QMutexLocker locker( &(XinxThreadManager::self()->m_mutex) );
	XinxThreadManager::self()->m_threadClassCount--;
	emit XinxThreadManager::self()->threadCountChange();
}

void XinxThread::run() {
	{
		QMutexLocker locker( &(XinxThreadManager::self()->m_mutex) );
		XinxThreadManager::self()->m_threadCount++;
		emit XinxThreadManager::self()->threadCountChange();
	}

	threadrun();

	{
		QMutexLocker locker( &(XinxThreadManager::self()->m_mutex) );
		XinxThreadManager::self()->m_threadCount--;
		emit XinxThreadManager::self()->threadCountChange();
	}
}
