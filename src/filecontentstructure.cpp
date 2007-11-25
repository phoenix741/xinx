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
 
#include "filecontentstructure.h"

/* PrivateFileContentElement */

class PrivateFileContentElement {
public:
	PrivateFileContentElement( FileContentElement * parent );
	
	int m_line;
	QString m_name, m_filename;
	FileContentElement * m_parentElement;
private:
	FileContentElement * m_parent;
};

PrivateFileContentElement::PrivateFileContentElement( FileContentElement * parent ) {
	m_parent = parent;
	m_line   = -1;
	m_name   = QString();
	m_parentElement = NULL;
}

/* FileContentElement */

FileContentElement::FileContentElement( FileContentElement * parent, const QString & name, int line ) {
	d = new PrivateFileContentElement( this );
	d->m_line = line;
	d->m_name = name;
	d->m_parentElement = parent;
	if( parent )
		d->m_filename = parent->d->m_filename;
}

FileContentElement::~FileContentElement() {
	delete d;
}
	
const QString & FileContentElement::name() {
	return d->m_name;
}
	
int FileContentElement::line() {
	return d->m_line;
}

const QString & FileContentElement::filename() const {
	return d->m_filename;
}

void FileContentElement::setName( const QString & name ) {
	d->m_name = name;
}
	
void FileContentElement::setLine( int line ) {
	d->m_line = line;
}

void FileContentElement::setFilename( const QString & filename ) {
	d->m_filename = filename;
}

int FileContentElement::rowCount() {
	return 0;
}

FileContentElement * FileContentElement::element( int index ) {
	Q_UNUSED( index );
	return NULL;
}

FileContentElement * FileContentElement::parent() {
	return d->m_parentElement;
}

int FileContentElement::row() {
	if( parent() )
		for( int i = 0 ; i < parent()->rowCount(); i++ ) {
			if( this == parent()->element( i ) ) 
				return i;
		}
	return -1;
}

/* FileContentElementProxy */

class FileContentElementProxy {
public:
	FileContentElement * m_element;
	QString m_tips;
	QIcon m_icon;
	
	QList<FileContentElementProxy*> m_list;
	
	int m_row;
	FileContentElementProxy * m_parent;
};


/* PrivateFileContentModel */

class PrivateFileContentModel {
public:
	PrivateFileContentModel( FileContentModel * parent );
	~PrivateFileContentModel();
	
	FileContentElementProxy * m_root;
private:
	FileContentModel * m_parent;
};

/* FileContentModel */

FileContentModel::FileContentModel( QObject *parent ) {
	d = new PrivateFileContentModel( this );
}

FileContentModel::~FileContentModel() {
	delete d;
}
	
void FileContentModel::loadFromFileContentElement( FileContentElement * element ) {
	
}

QVariant FileContentModel::data( const QModelIndex &index, int role ) const {
	if( !index.isValid() ) 
		return QVariant();
		
	FileContentItemModel::struct_file_content data;
	FileContentElementProxy * element = static_cast<FileContentElementProxy*>( index.internalPointer() );
		
	switch( role ) {
	case Qt::DisplayRole:
		return element->m_element->name();
	case Qt::DecorationRole:
		return element->m_icon;
	case Qt::UserRole:
		data.line = element->m_element->line();
		data.filename = element->m_element->filename();
		return QVariant::fromValue( data );
	default:
		return QVariant();
	}
}

Qt::ItemFlags flags( const QModelIndex &index ) const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;	
}

QModelIndex FileContentModel::index( int row, int column, const QModelIndex &parent ) const {
	FileContentElementProxy * parentElement = d->m_root, * currentElement;
	if( parent.isValid() ) {
		parentElement = static_cast<FileContentElementProxy*>( parent.internalPointer() );
	}
	currentElement = parentElement->m_element->at( row );
	
	if( currentElement )
		return createIndex( row, column, currentElement );
	else 
		return QModelIndex();
}

QModelIndex FileContentModel::parent( const QModelIndex &index ) const {
	if( !index.isValid() )
		return QModelIndex();
	
	FileContentElementProxy * element = static_cast<FileContentElementProxy*>( index.internalPointer() ),
					   * parent  = element->m_parent;
	
	if( element == d->m_root )
		return QModelIndex();
	
	return createIndex( parent->m_row, 0, parent );
}

int FileContentModel::rowCount( const QModelIndex &parent ) const {
	if( parent.isValid() ) {
		FileContentElementProxy * element = static_cast<FileContentElementProxy*>( parent.internalPointer() );
		return element->m_element->rowCount();		
	} else {
		return d->m_root->rowCount();
	}
}

QVariant FileContentModel::headerData( int section, Qt::Orientation orientation, int role ) const {
	if( ( orientation == Qt::Horizontal ) && ( role == Qt::DisplayRole ) && ( section == 0 ) )
		return tr("Content Name");
		
	return QVariant();
}

int FileContentModel::columnCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	return 1;
}

