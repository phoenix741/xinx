/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "xqmodelcompleter.h"
#include "xquery_keyword.h"
#include <xinxconfig.h>
#include <xinxformatscheme.h>

// QCodeedit header
#include <qformat.h>

// Qt header
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
		QString format =  "xquery_" + XQueryKeyword::self()->keywords()[e];
		return XINXConfig::self()->scheme( "XQuery" )->format( format ).foreground;
	} else if( role == Qt::DecorationRole ) {
		return QIcon(":/images/complete_fn.png");
	} else if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) )
		return e;

	return QVariant();
}

Qt::ItemFlags XQModelCompleter::flags( const QModelIndex &index ) const {
	Q_UNUSED( index );
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int XQModelCompleter::rowCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	return XQueryKeyword::self()->keywords().keys().count();
}

