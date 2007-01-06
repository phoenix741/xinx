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

// HELP
/*
Verifier si DBUS est installé et fonction sous Windows ainsi que QtDbus
http://api.kde.org/cvs-api/kdelibs-apidocs/kdecore/html/kuniqueapplication_8h-source.html
http://api.kde.org/cvs-api/kdelibs-apidocs/kdecore/html/kuniqueapplication_8cpp-source.html
http://api.kde.org/cvs-api/kdelibs-apidocs/kdecore/html/classKUniqueApplication.html
*/

#include <QApplication>

class UniqueApplication : public QApplication {
	Q_OBJECT
public:
	UniqueApplication ( int & argc, char ** argv );
	UniqueApplication ( int & argc, char ** argv, bool GUIenabled );
	UniqueApplication ( int & argc, char ** argv, Type type );
	UniqueApplication ( Display * display, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0 );
	UniqueApplication ( Display * display, int & argc, char ** argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0 );
	
	virtual ~UniqueApplication ();
	
	void start();
	
	bool isUnique() { return m_isUnique; }
	
	void sendSignalOpen(const QString &fileName);
Q_SIGNALS:
    void open(const QString &fileName);
    
private:
	bool m_isUnique;
};
#endif
