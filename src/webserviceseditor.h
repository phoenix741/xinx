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

#ifndef __WEBSERVICESEDITOR_H__
#define __WEBSERVICESEDITOR_H__

#include "fileeditor.h"

class WebServices;
class QComboBox;

typedef QList<WebServices*> WebServicesList;

class WebServicesEditor : public FileEditor {
	Q_OBJECT
public:
	WebServicesEditor( QWidget *parent = 0 );

	virtual QString getSuffix() const;
private Q_SLOTS:
	void webServicesChanged();
	void webServicesActivated( int );
	void webServicesParamActivated( int );
private:
	QComboBox * m_paramList, * m_servicesList, * m_actionList;
};

#endif // __WEBSERVICESEDITOR_H__
