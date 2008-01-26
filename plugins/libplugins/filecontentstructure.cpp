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

// Xinx header
#include "filecontentstructure.h"
#include "exceptions.h"

/* FileContentException */

FileContentException::FileContentException( QString message, int line, int column ) 
	: XinxException( QString("Error : %1 at line %2:%3").arg( message ).arg( line ).arg( column ) ), m_line( line ), m_column( column ) {
	
}

int FileContentException::getLine() const {
	return m_line;
}

int FileContentException::getColumn() const {
	return m_column;
}

/* PrivateFileContentElement */

class PrivateFileContentElement {
public:
	PrivateFileContentElement( FileContentElement * parent );
	~PrivateFileContentElement();
	
	bool m_flagDelete;
	
	int m_line;
	QString m_name, m_filename;
	FileContentElement * m_parentElement;
	
	QList<FileContentElement*> m_elements;
private:
	FileContentElement * m_parent;
};

PrivateFileContentElement::PrivateFileContentElement( FileContentElement * parent ) {
	XINX_TRACE( "PrivateFileContentElement", "( parent )" );

	m_parent = parent;
	m_line   = -1;
	m_name   = QString();
	m_parentElement = NULL;
	m_flagDelete = false;
}

PrivateFileContentElement::~PrivateFileContentElement() {
	XINX_TRACE( "~PrivateFileContentElement", "()" );
	//qDeleteAll( m_elements );
	// Can cause some problem. Don't forget to clean memory
}

/* FileContentElement */

FileContentElement::FileContentElement( FileContentElement * parent, const QString & name, int line ) {
	XINX_TRACE( "FileContentElement", QString( "( %1, %2, %3 )" ).arg( (unsigned int)parent, 0, 16 ).arg( name ).arg( line ) );

	d = new PrivateFileContentElement( this );
	d->m_line = line;
	d->m_name = name;
	d->m_parentElement = parent;
	if( parent ) { 
		d->m_filename = parent->d->m_filename;
		connect( this, SIGNAL(aboutToRemove(FileContentElement*)), d->m_parentElement, SIGNAL(aboutToRemove(FileContentElement*)) );
		connect( this, SIGNAL(aboutToAdd(FileContentElement*,int)), d->m_parentElement, SIGNAL(aboutToAdd(FileContentElement*,int)) );
		connect( this, SIGNAL(removed()), d->m_parentElement, SIGNAL(removed()) );
		connect( this, SIGNAL(added()), d->m_parentElement, SIGNAL(added()) );
	}

}

FileContentElement::~FileContentElement() {
	XINX_TRACE( "~FileContentElement", "()" );
	
	this->disconnect( d->m_parentElement );

	delete d;
}

bool FileContentElement::equals( FileContentElement * element ) {
	//XINX_TRACE( "FileContentElement::equals", QString( "( %1 )" ).arg( (unsigned int)element, 0, 16 ) );

	return ( ( typeid( *element ) == typeid( *this ) )
		  && ( d->m_name == element->d->m_name ) );
}

void FileContentElement::copyFrom( FileContentElement * element ) {
	XINX_TRACE( "FileContentElement::copyFrom", QString( "( %1 )" ).arg( (unsigned int)element, 0, 16 ) );

	d->m_filename = element->d->m_filename;
	d->m_line = element->d->m_line;
	d->m_name = element->d->m_name;
}

QIcon FileContentElement::icon() const {
	XINX_TRACE( "FileContentElement::icon", "()" );

	return QIcon("images/warning.png");
}

const QString & FileContentElement::name() const {
	XINX_TRACE( "FileContentElement::name", "()" );

	return d->m_name;
}

QString FileContentElement::displayName() const {
	XINX_TRACE( "FileContentElement::displayName", "()" );

	return name();
}

QString FileContentElement::displayTips() const {
	XINX_TRACE( "FileContentElement::displayTips", "()" );

	return tr( "Element at line : %1" ).arg( d->m_line );
}

int FileContentElement::line() {
	XINX_TRACE( "FileContentElement::line", "()" );

	return d->m_line;
}

const QString & FileContentElement::filename() const {
	XINX_TRACE( "FileContentElement::filename", "()" );

	return d->m_filename;
}

void FileContentElement::setName( const QString & name ) {
	XINX_TRACE( "FileContentElement::setName", QString( "( %1 )" ).arg( name ) );

	d->m_name = name;
}
	
void FileContentElement::setLine( int line ) {
	XINX_TRACE( "FileContentElement::setLine", QString( "( %1 )" ).arg( line ) );

	d->m_line = line;
}

void FileContentElement::setFilename( const QString & filename ) {
	XINX_TRACE( "FileContentElement::setFilename", QString( "( %1 )" ).arg( filename ) );

	d->m_filename = filename;
}

FileContentElement * FileContentElement::parent() {
	XINX_TRACE( "FileContentElement::parent", "()" );

	return d->m_parentElement;
}

int FileContentElement::row() {
	XINX_TRACE( "FileContentElement::row", "()" );

	if( d->m_parentElement )
		for( int i = 0 ; i < d->m_parentElement->rowCount(); i++ ) {
			if( this == d->m_parentElement->element( i ) ) 
				return i;
		}
	return 0;
}

int FileContentElement::rowCount() {
	XINX_TRACE( "FileContentElement::rowCount", "()" );

	return d->m_elements.size();
}

FileContentElement * FileContentElement::element( int index ) {
	XINX_TRACE( "FileContentElement::element", QString( "( %1 )" ).arg( index ) );

	return d->m_elements.at( index );
}

void FileContentElement::remove( int index ) {
	XINX_TRACE( "FileContentElement::remove", QString( "( %1 )" ).arg( index ) );

	emit aboutToRemove( d->m_elements.at( index ) );

	delete d->m_elements.at( index );
	d->m_elements.removeAt( index );

	emit removed();
}

FileContentElement * FileContentElement::append( FileContentElement * element ) {
	XINX_TRACE( "FileContentElement::append", QString( "( %1 )" ).arg( (unsigned int)element, 0, 16 ) );

	FileContentElement * old = contains( element ); 
	if( old ) {
		old->copyFrom( element );
		old->d->m_flagDelete = false;
		delete element;
		emit updated( old );
		return old;
	} else {
		element->d->m_parentElement = this; // Appropriation
		emit aboutToAdd( element, d->m_elements.size() );

		d->m_elements.append( element );
		
		emit added();
		return element;
	}
}


void FileContentElement::clear() {
	XINX_TRACE( "FileContentElement::clear", "()" );

	for( int i = d->m_elements.size() - 1; i >= 0; i-- )
		remove( i );
}

FileContentElement * FileContentElement::contains( FileContentElement * element ) {
	XINX_TRACE( "FileContentElement::contains", QString( "( %1 )" ).arg( (unsigned int)element, 0, 16 ) );

	foreach( FileContentElement * e, d->m_elements ) {
		if( element->equals( e ) ) 
			return e;
	}
	return NULL;
}

void FileContentElement::markDeleted() {
	XINX_TRACE( "FileContentElement::markDeleted", "()" );

	d->m_flagDelete = true;
}

void FileContentElement::markKeeped() {
	XINX_TRACE( "FileContentElement::markKeeped", "()" );

	d->m_flagDelete = false;
}

void FileContentElement::markAllDeleted() {
	XINX_TRACE( "FileContentElement::markAllDeleted", "()" );

	foreach( FileContentElement * e, d->m_elements ) {
		e->markDeleted();
	}
}

void FileContentElement::removeMarkedDeleted() {
	XINX_TRACE( "FileContentElement::removeMarkedDeleted", "()" );

	for( int i = d->m_elements.size() - 1 ; i >= 0 ; i-- ) {
		if( d->m_elements.at( i )->d->m_flagDelete ) {
			remove( i );
		}
	}
}

bool FileContentElementModelObjListSort( FileContentElement * d1, FileContentElement * d2 ) {
	XINX_TRACE( "FileContentElementModelObjListSort", QString( "( %1, %2 )" ).arg( d1->name() ).arg( d2->name() ) );

	return d1->name() < d2->name();
}

