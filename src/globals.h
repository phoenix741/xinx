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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <QObject>
#include <QList>

class XSLProject;
class WebServicesModel;
class WebServices;

typedef QList<WebServices*> WebServicesList;

/* Ajouter XINXConfig ici */ 

class Globals : public QObject {
	Q_OBJECT
public:
	Globals();
	~Globals();

	WebServicesList*  m_webServices;
	WebServicesModel* m_webServicesModel;
	XSLProject * m_project;
Q_SIGNALS:
	void projectChanged();
	void webServicesChanged();
};

extern Globals global;

#endif // __GLOBALS_H__
