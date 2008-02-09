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

#ifndef __P_SNIPETLIST_H__
#define __P_SNIPETLIST_H__
 
// Xinx header
#include "xinxcore.h"
#include "snipetlist.h"
#include "snipet.h"

// Qt header
#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>

class PrivateSnipetList : public QObject {
	Q_OBJECT
public:
	PrivateSnipetList( SnipetList * parent );
	virtual ~PrivateSnipetList();
	
	QStringList m_categories;
	
	QList<Snipet*> m_list;
	QString m_filename;
public slots:
	void addCategory( QString newCategory );
	void snipetPropertyChange();
private:
	SnipetList * m_parent;
};

#endif // __P_SNIPETLIST_H__
