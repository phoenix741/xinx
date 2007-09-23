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

#ifndef __P_UNIQUEAPPLICATION_H__
#define __P_UNIQUEAPPLICATION_H__

// Define
#ifdef Q_WS_WIN
	#ifndef QT_QTDBUS
		#define DBUS
	#endif
#else
	#define DBUS
#endif

// Xinx header
#include "../uniqueapplication.h"
#include "../mainformimpl.h"
#ifdef DBUS
	#include "../studioadaptor.h"
	#include "../studiointerface.h"
#endif

// Qt header
#include <QObject>
#ifdef DBUS
	#include <QtDBus>
#else
	#include <QTimer>
	#include <QtGui>
#endif

// Standard header
#ifdef DBUS
	#include <unistd.h>
#endif

class PrivateUniqueApplication : public QObject {
	Q_OBJECT
public:
	PrivateUniqueApplication( UniqueApplication * parent );
	virtual ~PrivateUniqueApplication();
	
	bool m_isUnique;
	MainformImpl * m_mainform;
#ifndef DBUS
	HWND m_handle, m_handleMutex, m_handleMutexGbl, m_handleEvent;
	char* m_fileView;
	void openSharedMem();
#else
	ComEditorXinxInterface * m_interface;
#endif
	void start();

public slots:
#ifndef DBUS
	void timerApplicationEvent();
#endif
private:
	UniqueApplication * m_parent;
};

#endif // __P_UNIQUEAPPLICATION_H__
