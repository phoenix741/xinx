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

// Qt header
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QIcon>

namespace ContentView2 {

/* CompletionModel */

CompletionModel::CompletionModel( QSqlDatabase db, QObject * parent ) : QSqlQueryModel( parent ), m_db( db ) {

}

CompletionModel::~CompletionModel() {

}

QVariant CompletionModel::data( const QModelIndex &idx, int role ) const {
	if( ! idx.isValid() ) return QVariant();
	if( idx.column() != 0 ) return QVariant();

	if( role == Qt::DecorationRole ) {
		return QIcon( record( idx.row() ).value( 2 ).toString() );
	} else if( role == CompletionModel::CompletionIdRole ) {
		return record( idx.row() ).value( 2 ).toInt();
	}

	return QSqlQueryModel::data( idx, role );
}

int CompletionModel::columnCount( const QModelIndex & parent ) const {
	Q_UNUSED( parent );
	return 1;
}

void CompletionModel::select() {
	QSqlQuery query( m_db );

	QString queryStr =
		"SELECT cv_node.display_name, cv_node.name, cv_node.icon, cv_node.id "
		"FROM cv_node ";

	if( ! (m_filesId.isEmpty() && m_whereClauses.isEmpty()) ) {
		queryStr += "WHERE ";
	}

	if( ! m_filesId.isEmpty() ) {
		QStringList ids;
		foreach( uint id, m_filesId ) {
			ids << QString( "%1" ).arg( id );
		}
		queryStr += QString( "cv_node.file_id in ( %1 ) ").arg( ids.join( ", " ) );
	}

	if( ! m_whereClauses.isEmpty() ) {
		queryStr += QString( "AND ( ( %1 ) )" ).arg( m_whereClauses.join( " ) OR ( " ) );
	}

	// Set the query used all snipet
	query.prepare( queryStr );

	bool result = query.exec();
	Q_ASSERT_X( result, "CompletionModel::select", qPrintable( query.lastError().text() ) );
	setQuery( query );
}

void CompletionModel::addWhereClause( const QString & whereClause ) {
	if( ! m_whereClauses.contains( whereClause ) ) {
		m_whereClauses.append( whereClause );
		select();
	}
}

void CompletionModel::addFile( uint file ) {
	if( ! m_filesId.contains( file ) ) {
		m_filesId.append( file );
		select();
	}
}

} // namespace ContentView2
