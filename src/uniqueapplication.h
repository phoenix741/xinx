/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
 
#ifndef _UNIQUEAPPLICATION_H_
#define _UNIQUEAPPLICATION_H_

#include <QApplication>

#ifdef Q_WS_WIN
	#ifndef QT_QTDBUS
		#define DBUS
	#endif
#else
	#define DBUS
#endif

#ifndef DBUS
	#include <windows.h>
#endif

class UniqueApplication : public QApplication {
	Q_OBJECT
public:
	UniqueApplication ( int & argc, char ** argv );
	UniqueApplication ( int & argc, char ** argv, bool GUIenabled );
	UniqueApplication ( int & argc, char ** argv, Type type );
#ifndef Q_WS_WIN
	UniqueApplication ( Display * display, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0 );
	UniqueApplication ( Display * display, int & argc, char ** argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0 );
#endif
	
	virtual ~UniqueApplication ();
	
	void start();
	
	bool isUnique() { return m_isUnique; }
	
	void sendSignalOpen(const QString &fileName);
Q_SIGNALS:
    void open(const QString &fileName);
	void hasFileToOpen(const QString & fileName);

#ifndef DBUS
private Q_SLOTS:
	void timerApplicationEvent();
#endif

private:
	bool m_isUnique;
	
#ifndef DBUS
	void openSharedMem();

	HWND m_handle, m_handleMutex, m_handleMutexGbl, m_handleEvent;
	char* m_fileView;
#endif
};
#endif
