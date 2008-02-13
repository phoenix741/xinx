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

#include "p_xinxpluginselector.h"

/* PrivateXinxPluginSelector */

PrivateXinxPluginSelector::PrivateXinxPluginSelector( XinxPluginSelector * parent ) : m_parent( parent ) {
	m_model = new XinxPluginModel( parent );
}

/* XinxPluginModel */

XinxPluginModel::XinxPluginModel( QWidget * parent ) : QAbstractListModel( parent ) {
	
}

XinxPluginModel::~XinxPluginModel() {
	
}

bool XinxPluginModel::setData( const QModelIndex &index, const QVariant &value, int role ) {
	
}

QVariant XinxPluginModel::data( const QModelIndex &index, int role ) const {
	
}

Qt::ItemFlags XinxPluginModel::flags( const QModelIndex &index ) const {
	
}

QModelIndex XinxPluginModel::index( int row, int column, const QModelIndex &parent ) const {
	
}

int XinxPluginModel::rowCount( const QModelIndex &parent ) const {
	return 0;
}


/* XinxPluginSelector */

XinxPluginSelector::XinxPluginSelector( QWidget *parent ) : QListView( parent ) {
	d = new PrivateXinxPluginSelector( this );
	setModel( d->m_model );
}

XinxPluginSelector::~XinxPluginSelector() {
	delete d;
}
    
void XinxPluginSelector::addPlugin( IXinxPlugin * plugin ) {
	
}
