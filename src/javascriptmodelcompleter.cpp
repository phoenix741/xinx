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
	connect( m_parser, SIGNAL(aboutToAdd(FileContentElement*,int)), this, SLOT(addElement(FileContentElement*,int)) );
	connect( m_parser, SIGNAL(aboutToRemove(FileContentElement*)), this, SLOT(removeElement(FileContentElement*)) );
}

JavascriptModelCompleter::~JavascriptModelCompleter() {
	
}
	
QVariant JavascriptModelCompleter::data( const QModelIndex &index, int role ) const {
	if ( ! index.isValid() ) return QVariant();

	FileContentElement * e = m_objList.at( index.row() );
	
	if( role == Qt::DecorationRole ) {
		return e->icon();
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

void JavascriptModelCompleter::addElement( FileContentElement* element, int row ) {
	Q_UNUSED( row );
	
	if( (!m_functionFiltre.isEmpty()) && element->parent() && ( element->parent()->name() == m_functionFiltre ) ) {
		QList<FileContentElement*>::iterator i = qLowerBound( m_objList.begin(), m_objList.end(), element, FileContentElementModelObjListSort );
		int index = i - m_objList.begin();
		beginInsertRows( QModelIndex(), index, index );
		m_objList.insert( i, element );
		endInsertRows();
	}
}

void JavascriptModelCompleter::removeElement( FileContentElement* element ) {
	int index = m_objList.indexOf( element );
	if( index >= 0 ) {
		beginRemoveRows( QModelIndex(), index, index );
		m_objList.removeAt( index );
		endRemoveRows();
	}
}


void JavascriptModelCompleter::refreshList( FileContentElement * element ) {
	FileContentElement * e = element;
	for( int i = 0 ; i < e->rowCount() ; i++ ) {
		m_objList.append( e->element( i ) );
		//refreshList( e->element( i ) ); // Pas de recursivite pour ne pas acceder au variables des functions.
	}
	qSort( m_objList.begin(), m_objList.end(), FileContentElementModelObjListSort );
}

void JavascriptModelCompleter::setFilter( const QString functionName ) {
	if( m_functionFiltre == functionName ) return;
	emit layoutAboutToBeChanged();
	
	m_functionFiltre = functionName;
	
	m_objList.clear();
	if( functionName.isEmpty() ) 
		refreshList( m_parser );
	else {
		for( int i = 0 ; i < m_parser->rowCount() ; i++ ) {
			FileContentElement * element = m_parser->element( i );
			JavaScriptFunction * function = dynamic_cast<JavaScriptFunction*>( element );
			m_objList.append( element );
			if( function && function->name() == functionName )
				refreshList( element );
		}
	}
			
	emit layoutChanged();
}
