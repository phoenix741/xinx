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

#ifndef _XINXTHREAD_H_
#define _XINXTHREAD_H_

// Qt header
#include <QThread>
#include <QMutex>

class XinxThread;

class MetaXinxThread : public QObject {
	Q_OBJECT
public:
	MetaXinxThread();
	virtual ~MetaXinxThread();
	
	int getThreadCount();
	static MetaXinxThread * getMetaThread();
signals:
	void threadCountChange( int count );
private:
	static MetaXinxThread m_metaXinxThread;
	
	friend class XinxThread;
};

class XinxThread : public QThread {
	Q_OBJECT
public:
	XinxThread( QObject * parent = 0 );
	virtual ~XinxThread();
	
protected:
	virtual void run();
	virtual void threadrun() = 0;
	
private:
	static int m_threadCount;
	static QMutex m_mutex;
	
	friend class MetaXinxThread;
};

#endif /* _XINXTHREAD_H_ */
