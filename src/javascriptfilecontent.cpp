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

#include "javascriptfilecontent.h"
#include "javascriptparser.h"

/* PrivateJavascriptFileContent */

class PrivateJavascriptFileContent {
public:
	PrivateJavascriptFileContent( JavascriptFileContent * parent );
	virtual ~PrivateJavascriptFileContent();
	
	JavaScriptParser * m_parser;
private:
	JavascriptFileContent * m_parent;
};

PrivateJavascriptFileContent::PrivateJavascriptFileContent( JavascriptFileContent * parent ) : m_parent( parent ) {
	m_parser = NULL;
}

PrivateJavascriptFileContent::~PrivateJavascriptFileContent() {
	
}


/* JavascriptFileContent */

JavascriptFileContent::JavascriptFileContent( JavaScriptParser * parser, QObject *parent ) : FileContentItemModel( parent ) {
	d = new PrivateJavascriptFileContent( this );
	d->m_parser = parser;
}

JavascriptFileContent::~JavascriptFileContent() {
	delete d;
}

QVariant JavascriptFileContent::data( const QModelIndex &index, int role ) const {
	if( !index.isValid() ) 
		return QVariant();
		
	FileContentItemModel::struct_file_content data;
	JavaScriptElement * element = static_cast<JavaScriptElement*>( index.internalPointer() );
		
	switch( role ) {
	case Qt::DisplayRole:
		return element->name();
	case Qt::DecorationRole:
		return QVariant();
	case Qt::UserRole:
		data.line = element->line();
		data.filename = QString();
		return QVariant::fromValue( data );
	default:
		return QVariant();
	}
}

Qt::ItemFlags JavascriptFileContent::flags( const QModelIndex &index ) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;	
}

QModelIndex JavascriptFileContent::index( int row, int column, const QModelIndex &parent ) const {
	JavaScriptElement * parentElement = d->m_parser, * currentElement;
	if( parent.isValid() ) {
		parentElement = static_cast<JavaScriptElement*>( parent.internalPointer() );
	}
	currentElement = parentElement->element( row );
	
	if( currentElement )
		return createIndex(row, column, currentElement );
	else 
		return QModelIndex();
}
	
QModelIndex JavascriptFileContent::parent( const QModelIndex &index ) const {
	if( !index.isValid() )
		return QModelIndex();
	
	JavaScriptElement * element = static_cast<JavaScriptElement*>( index.internalPointer() ),
					  * parent  = element->parent();
	
	if( element == d->m_parser )
		return QModelIndex();
	
	return createIndex( parent->row(), 0, parent );
}

int JavascriptFileContent::rowCount( const QModelIndex &parent ) const {
	if( parent.isValid() ) {
		JavaScriptElement * element = static_cast<JavaScriptElement*>( parent.internalPointer() );
		return element->rowCount();		
	} else {
		return d->m_parser->rowCount();
	}
}
	
int JavascriptFileContent::columnCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	return 1;
}
