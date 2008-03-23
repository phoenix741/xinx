/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#ifndef P_XINXPLUGINSELECTOR_H_
#define P_XINXPLUGINSELECTOR_H_

// Xinx header
#include "xinxpluginselector.h"
#include "plugininterfaces.h"

// Qt header
#include <QAbstractListModel>

class XinxPluginModel : public QAbstractListModel {
	Q_OBJECT
public:
	XinxPluginModel( QWidget * parent );
	virtual ~XinxPluginModel();

	void addPlugin( IXinxPlugin * plugin );
	
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::CheckStateRole );
    QVariant data( const QModelIndex &index, int role ) const;
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
private:
	QList<IXinxPlugin*> m_plugins;
};

class PrivateXinxPluginSelector : public QObject {
	Q_OBJECT
public:
	PrivateXinxPluginSelector( XinxPluginSelector * parent );
	
	XinxPluginModel * m_model;
private:
	XinxPluginSelector * m_parent;
};


#endif /*P_XINXPLUGINSELECTOR_H_*/
