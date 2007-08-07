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

#ifndef __SNIPETLIST_H__
#define __SNIPETLIST_H__

#include <QObject>
#include <QString>

class QMenu;

class SnipetListException {
public:
	SnipetListException( const QString & message );
	const QString & getMessage() const;
private:
	QString m_message;
};

class Snipet;
class PrivateSnipetList;

class SnipetList : public QObject {
	Q_OBJECT
public:
	SnipetList();
	virtual ~SnipetList();
	
	void add( Snipet * snipet );
	void remove( int index );
	Snipet * replace( int index, Snipet * snipet );
	Snipet * at( int index );
	int count();
	
	Snipet * indexOf( const QString & key );
	
	void saveToFile( const QString & filename );
	void loadFromFile( const QString & filename );
	
	const QStringList & categories() const;
signals:
	void listChanged();
private:
	PrivateSnipetList * d;
	friend class PrivateSnipetList;
};

#endif // __SNIPETLIST_H__
