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

FileContentParser::~FileContentParser() {
	
}

/* PrivateFileContentElement */

class PrivateFileContentElement {
public:
	PrivateFileContentElement( FileContentElement * parent );
	~PrivateFileContentElement();
	
	bool m_flagDelete, m_flagEmit;
	
	int m_line;
	QString m_name, m_filename;
	FileContentElement * m_parentElement;
	
	QList<FileContentElement*> m_elements;
	
	QReadWriteLock m_locker;
private:
	FileContentElement * m_parent;
};

PrivateFileContentElement::PrivateFileContentElement( FileContentElement * parent ) {
	m_parent = parent;
	m_line   = -1;
	m_name   = QString();
	m_parentElement = NULL;
	m_flagDelete = false;
	m_flagEmit = true;
}

PrivateFileContentElement::~PrivateFileContentElement() {
	//qDeleteAll( m_elements );
	// Can cause some problem. Don't forget to clean memory
}

/* FileContentElement */

FileContentElement::FileContentElement( FileContentElement * parent, const QString & name, int line ) {
	d = new PrivateFileContentElement( this );
	d->m_line = line;
	d->m_name = name;
	d->m_parentElement = parent;
	if( parent ) { 
		d->m_filename = parent->d->m_filename;
		setFlagEmit( true );
	}
}

FileContentElement::~FileContentElement() {
	delete d;
}

QReadWriteLock & FileContentElement::locker() {
	return d->m_locker;
}

void FileContentElement::setFlagEmit( bool value ) {
	d->m_flagEmit = value;
	if( value && d->m_parentElement ) {
		connect( this, SIGNAL(aboutToRemove(FileContentElement*)), d->m_parentElement, SIGNAL(aboutToRemove(FileContentElement*)) );
		connect( this, SIGNAL(aboutToAdd(FileContentElement*,int)), d->m_parentElement, SIGNAL(aboutToAdd(FileContentElement*,int)) );
		connect( this, SIGNAL(removed()), d->m_parentElement, SIGNAL(removed()) );
		connect( this, SIGNAL(added()), d->m_parentElement, SIGNAL(added()) );
	} else if( d->m_parentElement ) {
		this->disconnect( d->m_parentElement );
	}
}

bool FileContentElement::equals( FileContentElement * element ) {
	return ( ( typeid( *element ) == typeid( *this ) )
		  && ( d->m_name == element->d->m_name ) 
		  && ( d->m_filename == element->d->m_filename )
		   );
}

void FileContentElement::copyFrom( FileContentElement * element ) {
	d->m_filename = element->d->m_filename;
	d->m_line = element->d->m_line;
	d->m_name = element->d->m_name;
}

QIcon FileContentElement::icon() const {
	return QIcon("images/warning.png");
}

const QString & FileContentElement::name() const {
	return d->m_name;
}

QString FileContentElement::displayName() const {
	return name();
}

QString FileContentElement::displayTips() const {
	return tr( "Element at line : %1" ).arg( d->m_line );
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

FileContentElement * FileContentElement::parent() {
	return d->m_parentElement;
}

int FileContentElement::row() {
	if( d->m_parentElement )
		for( int i = 0 ; i < d->m_parentElement->rowCount(); i++ ) {
			if( this == d->m_parentElement->element( i ) ) 
				return i;
		}
	return 0;
}

int FileContentElement::rowCount() {
	return d->m_elements.size();
}

FileContentElement * FileContentElement::element( int index ) {
	return d->m_elements.at( index );
}

void FileContentElement::remove( int index ) {
	emit aboutToRemove( d->m_elements.at( index ) );

	QWriteLocker locker( &(d->m_locker) );
	delete d->m_elements.at( index );
	d->m_elements.removeAt( index );
	locker.unlock();

	emit removed();
}

FileContentElement * FileContentElement::append( FileContentElement * element ) {
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

		QWriteLocker locker( &(d->m_locker) );
		d->m_elements.append( element );
		locker.unlock();

		emit added();
		return element;
	}
}


void FileContentElement::clear() {
	QWriteLocker locker( &(d->m_locker) );
	for( int i = d->m_elements.size() - 1; i >= 0; i-- )
		remove( i );
}

FileContentElement * FileContentElement::contains( FileContentElement * element ) {
	QReadLocker locker( &(d->m_locker) );
	foreach( FileContentElement * e, d->m_elements ) {
		if( element->equals( e ) ) 
			return e;
	}
	return NULL;
}

void FileContentElement::markDeleted() {
	d->m_flagDelete = true;
}

void FileContentElement::markKeeped() {
	d->m_flagDelete = false;
}

void FileContentElement::markAllDeleted() {
	QWriteLocker locker( &(d->m_locker) );
	foreach( FileContentElement * e, d->m_elements ) {
		e->markDeleted();
	}
}

void FileContentElement::removeMarkedDeleted() {
	QWriteLocker locker( &(d->m_locker) );
	for( int i = d->m_elements.size() - 1 ; i >= 0 ; i-- ) {
		if( d->m_elements.at( i )->d->m_flagDelete ) {
			remove( i );
		}
	}
}

bool FileContentElementModelObjListSort( FileContentElement * d1, FileContentElement * d2 ) {
	return d1->name() < d2->name();
}

