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

// Qt header
#include <QBuffer>
#include <QFile>

// Std header
#include <typeinfo>

/* FileContentParser */

void FileContentParser::loadFromContent( const QString & content ) {
	QByteArray contentArray = content.toUtf8();
	QBuffer buffer( &contentArray );
	buffer.open( QIODevice::ReadOnly );

	loadFromDevice( &buffer );
}

void FileContentParser::loadFromFile( const QString & filename ) {
	QFile file( filename );

	// Open the file
	if (!file.open(QFile::ReadOnly))
		throw FileContentException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	loadFromDevice( & file );
}

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

/* PrivateFileContentElement */ /*! \cond PRIVATE */

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
	m_parent = parent;
	m_line   = -1;
	m_name   = QString();
	m_parentElement = NULL;
	m_flagDelete = false;
}

PrivateFileContentElement::~PrivateFileContentElement() {

}

/*! \endcond */

/* FileContentElement */

FileContentElement::FileContentElement( FileContentElement * parent, const QString & name, int line ) : QObject( parent ) {
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
	delete d;
}

bool FileContentElement::equals( FileContentElement * element ) {
	return ( ( typeid( *element ) == typeid( *this ) )
		  && ( d->m_name == element->d->m_name ) );
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

	delete d->m_elements.at( index );
	d->m_elements.removeAt( index );

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

		d->m_elements.append( element );
		element->setParent( this );

		emit added();
		return element;
	}
}


void FileContentElement::clear() {
	for( int i = d->m_elements.size() - 1; i >= 0; i-- )
		remove( i );
}

FileContentElement * FileContentElement::contains( FileContentElement * element ) {
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
	foreach( FileContentElement * e, d->m_elements ) {
		e->markDeleted();
	}
}

void FileContentElement::removeMarkedDeleted() {
	for( int i = d->m_elements.size() - 1 ; i >= 0 ; i-- ) {
		if( d->m_elements.at( i )->d->m_flagDelete ) {
			remove( i );
		}
	}
}

/* FileContentElementModelObjListSort */

bool FileContentElementModelObjListSort( FileContentElement * d1, FileContentElement * d2 ) {
	return d1->name() < d2->name();
}

/* FileContentElementList */

FileContentElementList::FileContentElementList( FileContentElement * root ) : m_root( root ) {
	if( m_root ) {
		connect( m_root, SIGNAL(aboutToAdd(FileContentElement*,int)), this, SLOT(addElement(FileContentElement*,int)) );
		connect( m_root, SIGNAL(aboutToRemove(FileContentElement*)), this, SLOT(removeElement(FileContentElement*)) );
	}
	refreshList();
}

FileContentElementList::~FileContentElementList() {

}

const QList<FileContentElement*> & FileContentElementList::list() const {
	return m_list;
}

FileContentElement * FileContentElementList::rootElement() const {
	return m_root;
}


void FileContentElementList::refreshList() {
	m_list.clear();
	if( m_root ) {
		refreshRecursive( m_root );
		qSort( m_list.begin(), m_list.end(), FileContentElementModelObjListSort );
	}
	emit reset();
}

void FileContentElementList::addElement( FileContentElement * element, int row ) {
	Q_UNUSED( row );
	if( dynamic_cast<FileContentParser*>( element ) )
		refreshRecursive( element );
	else if( ! contains( element ) )
		addElement( element );
}

bool FileContentElementList::contains( FileContentElement * data ) {
	foreach( FileContentElement * element, m_list ) {
		if( element->equals( data ) )
			return true;
	}
	return false;
}

void FileContentElementList::refreshRecursive( FileContentElement * data ) {
	Q_ASSERT( data );

	for( int i = 0; i < data->rowCount(); i++ ) {
		FileContentElement * e = data->element( i );
		if( dynamic_cast<FileContentParser*>( e ) ) {
			QString name = e->name();
			if( ! m_files.contains( name ) ) {
				m_files.append( name );
				refreshRecursive( e );
			}
		} else {
			if( ! contains( e ) )
				addElement( e );
		}
	}
}

void FileContentElementList::addElement( FileContentElement* element ) {
	if( ! isElementShowed( element ) ) return;
	QList<FileContentElement*>::iterator i = qLowerBound( m_list.begin(), m_list.end(), element, FileContentElementModelObjListSort );
	int index = i - m_list.begin();
	emit aboutToAdd( index );
	m_list.insert( i, element );
	emit added();
}

void FileContentElementList::removeElement( FileContentElement* element ) {
	int index = m_list.indexOf( element );
	if( index >= 0 ) {
		emit aboutToRemove( index );
		m_list.removeAt( index );
		emit removed();
	}
}


