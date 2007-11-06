/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "xinxthread.h"

int XinxThread::m_threadCount = 0;
QMutex XinxThread::m_mutex;
MetaXinxThread MetaXinxThread::m_metaXinxThread;

/* MetaXinxThread */

MetaXinxThread::MetaXinxThread() {
	
}

MetaXinxThread::~MetaXinxThread() {
	
}

int MetaXinxThread::getThreadCount() {
	QMutexLocker locker( &XinxThread::m_mutex );
	return XinxThread::m_threadCount;
}

MetaXinxThread * MetaXinxThread::getMetaThread() {
	return &( m_metaXinxThread );
}


/* XinxThread */

XinxThread::XinxThread( QObject * parent ) : QThread( parent ) {
	
}

XinxThread::~XinxThread() {
	
}
	
void XinxThread::run() {
	{
		QMutexLocker locker( &m_mutex );
		m_threadCount++;
		emit MetaXinxThread::m_metaXinxThread.threadCountChange( m_threadCount );
	}
	
	threadrun();
	
	{
		QMutexLocker locker( &m_mutex );
		m_threadCount--;
		emit MetaXinxThread::m_metaXinxThread.threadCountChange( m_threadCount );
	}
}
