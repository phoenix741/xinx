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
#include "contentview2completionmodel.h"
#include "project/xinxproject.h"

namespace ContentView2 {

/* CompletionModelSort */

bool CompletionModelSort( uint node1, uint node2 ) {
	ContentView2::Node d1( XINXProjectManager::self()->session()->database(), node1 );
	ContentView2::Node d2( XINXProjectManager::self()->session()->database(), node2 );
	return d1.data( ContentView2::Node::NODE_DISPLAY_NAME ).toString().compare( d2.data( ContentView2::Node::NODE_DISPLAY_NAME ).toString(), Qt::CaseInsensitive ) < 0;
}

/* CompletionModel */

CompletionModel::CompletionModel( QSqlDatabase db, uint fileId, QObject * parent ) : QAbstractListModel( parent ) {
}

CompletionModel::~CompletionModel() {
}

QVariant CompletionModel::data( const QModelIndex &index, int role ) const {
}

int CompletionModel::rowCount( const QModelIndex &parent ) const {

}

void CompletionModel::select() {
}

void CompletionModel::addWhereClause( const QString & whereClause ) {
}

} // namespace ContentView2
