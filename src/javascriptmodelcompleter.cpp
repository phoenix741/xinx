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
 
#include "javascriptmodelcompleter.h"
#include "javascriptparser.h"

#include <QIcon>

/* JavascriptModelCompleter */

JavascriptModelCompleter::JavascriptModelCompleter( JavaScriptParser * parser, QObject *parent ) : QAbstractListModel( parent ) {
	m_functionFiltre = QString();
	m_parser = parser;
	refreshList( m_parser );
}

JavascriptModelCompleter::~JavascriptModelCompleter() {
	
}
	
QVariant JavascriptModelCompleter::data( const QModelIndex &index, int role ) const {
	if ( ! index.isValid() ) return QVariant();

	JavaScriptElement * e = m_objList.at( index.row() );
	
	if( role == Qt::DecorationRole ) {
		if( dynamic_cast<JavaScriptFunction*>( e ) ) {
			return QIcon( ":/images/noeud.png" );			
		} else
		if( dynamic_cast<JavaScriptVariables*>( e ) ) {
			return QIcon( ":/images/variable.png" );			
		} else
		if( dynamic_cast<JavaScriptParams*>( e ) ) {
			return QIcon( ":/images/html_value.png" );			
		} else
			return QVariant();
	} else
	if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
		return e->name();
	
	return QVariant();	
}

Qt::ItemFlags JavascriptModelCompleter::flags( const QModelIndex &index ) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int JavascriptModelCompleter::rowCount( const QModelIndex &parent ) const {
	if( ! parent.isValid() ) 
		return m_objList.size();
	else
		return 0;
}

void JavascriptModelCompleter::refreshList( JavaScriptElement * element ) {
	JavaScriptElement * e = element;
	for( int i = 0 ; i < e->rowCount() ; i++ ) {
		m_objList.append( e->element( i ) );
		refreshList( e->element( i ) );
	}
}

void JavascriptModelCompleter::setFilter( const QString functionName ) {
	if( m_functionFiltre == functionName ) return;
	emit layoutAboutToBeChanged();
	
	m_functionFiltre = functionName;
	
	m_objList.clear();
	if( functionName.isEmpty() ) 
		refreshList( m_parser );
	else foreach( JavaScriptFunction* function, m_parser->functions() ) {
		m_objList.append( function );
		if( function->name() == functionName ) 
			refreshList( function );
	}
			
	emit layoutChanged();
}
