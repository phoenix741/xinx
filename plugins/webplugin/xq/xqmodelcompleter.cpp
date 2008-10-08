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

#include "xqmodelcompleter.h"
#include "xquery_keyword.h"
#include <xinxconfig.h>

#include <QIcon>

/* XQModelCompleter */

XQModelCompleter::XQModelCompleter( QObject *parent ) : QAbstractListModel( parent ) {
}

XQModelCompleter::~XQModelCompleter() {

}

QVariant XQModelCompleter::data( const QModelIndex &index, int role ) const {
	if ( ! index.isValid() ) return QVariant();

	QString e = XQueryKeyword::self()->keywords().keys().at( index.row() );

	if( role == Qt::ForegroundRole ) {
		QString format =  "xq_" + XQueryKeyword::self()->keywords()[e];
		return XINXConfig::self()->config().formats[ format ].foreground();
	} else
	if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) )
		return e;

	return QVariant();
}

Qt::ItemFlags XQModelCompleter::flags( const QModelIndex &index ) const {
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int XQModelCompleter::rowCount( const QModelIndex &parent ) const {
	return XQueryKeyword::self()->keywords().keys().count();
}

