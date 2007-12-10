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

#ifndef __P_FLATTREEVIEW_H__
#define __P_FLATTREEVIEW_H__

// Xinx header
#include "flattreeview.h"
#include "exceptions.h"

// Qt header
#include <QHash>
#include <QDirModel>

class PrivateFlatModel : public QObject {
	Q_OBJECT
public:
	PrivateFlatModel( FlatModel * parent );
	virtual ~PrivateFlatModel();
	
	QDirModel * m_model;
	QPersistentModelIndex m_root;
	QStringList m_pathList;
		
public slots:
	void recalcPathList();
	void rowsInserted ( const QModelIndex & parent, int start, int end );
	void rowsRemoved ( const QModelIndex & parent, int start, int end );	
private:
	void insertIntoPathList( QModelIndex index );

	FlatModel * m_parent;
};

#endif // __P_FLATTREEVIEW_H__
