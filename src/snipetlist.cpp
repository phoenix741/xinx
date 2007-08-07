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

#include "snipetlist.h"
#include "snipet.h"

#include <QApplication>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>

/* SnipetListException */

/*!
 * \class SnipetListException
 * The snipet list exception, is throw when Snipet list load or save file.
 */

/*!
 * Exception throw by the Snipet list.
 * \param message Message of the error.
 */
SnipetListException::SnipetListException( const QString & message ) {
	m_message = message;
}
	
const QString & SnipetListException::getMessage() const {
	return m_message;
}


/* PrivateSnipetList */

class PrivateSnipetList : public QObject {
	Q_OBJECT
public:
	PrivateSnipetList( SnipetList * parent );
	virtual ~PrivateSnipetList();
	
	QStringList m_categories;
	
	QList<Snipet*> m_list;
public slots:
	void addCategory( QString newCategory );
private:
	SnipetList * m_parent;
};

PrivateSnipetList::PrivateSnipetList( SnipetList * parent ) : m_parent( parent ) {
	
}

PrivateSnipetList::~PrivateSnipetList() {
	
}

void PrivateSnipetList::addCategory( QString newCategory ) {
	if( ! m_categories.contains( newCategory ) )
		m_categories.append( newCategory );
	emit m_parent->listChanged();
}


/* SnipetList */

/*!
 * \class SnipetList
 * The snipet list contains all snipet defined in XINX. This snipet is stored in a 
 * file in XML format. 
 */

/*!
 * Create an empty snipet list. 
 */
SnipetList::SnipetList() {
	d = new PrivateSnipetList( this );
}

/*!
 * Destroy a snipet list.
 */
SnipetList::~SnipetList() {
	delete d;
}

/*!
 * Add a snipet to the list.
 * \param snipet The snipet to add.
 */
void SnipetList::add( Snipet * snipet ) {
	d->m_list.append( snipet );
	d->addCategory( snipet->category() );
	connect( snipet, SIGNAL(categoryChange(QString)), d, SLOT(addCategory(QString)) );
}

/*!
 * Remove the snipet of the list.
 * \param index The index of the snipet to remove.
 */
void SnipetList::remove( int index ) {
	d->m_list.removeAt( index );
	emit listChanged();
}

/*!
 * Replace a snipet in the list. The snipet is replaced by the one in the
 * parameter. The old snipet is returned.
 * \param index The index of the element to replace
 * \param snipet The new snipet used to replace the older.
 * \return The older snipet.
 */
Snipet * SnipetList::replace( int index, Snipet * snipet ) {
	Snipet * s = d->m_list.at( index );	
	d->m_list.replace( index, snipet );
	emit listChanged();
	return s;
}

/*!
 * Return the element at index from the list.
 * \param index The index in the list.
 * \return The snipet at the index.
 */
Snipet * SnipetList::at( int index ) {
	return d->m_list.at( index );
}

/*!
 * Return the number of element in the list.
 * \return Number of element.
 */
int SnipetList::count() {
	return d->m_list.count();
}

/*!
 * List of categories used by templates
 * \return List of template.
 */
const QStringList & SnipetList::categories() const {
	return d->m_categories;
}

Snipet * SnipetList::indexOf( const QString & key ) {
	foreach( Snipet * snipet, d->m_list ) {
		if( snipet->key() == key )
			return snipet;
	}
	return NULL;
}

/*!
 * Save the snipet list into a file.
 * \param filename The filename where we want save snipet.
 * \throw SnipetListException
 */
void SnipetList::saveToFile( const QString & filename ) {
	const int IndentSize = 2;

	QDomDocument document( "SnipetList" );
	QDomElement root = document.createElement( "SnipetList" );
	document.appendChild( root );
	
	foreach( Snipet * snipet, d->m_list ) {
		QDomElement s = document.createElement( "Snipet" );
		root.appendChild( s );
		
		s.setAttribute( "name", snipet->name() );
		s.setAttribute( "key", snipet->key() );
		s.setAttribute( "type", snipet->type() );
		s.setAttribute( "category", snipet->category() );
		s.setAttribute( "icon", snipet->icon() );
		
		QDomElement description = document.createElement( "Description" );
		s.appendChild( description );
		QDomText text = document.createTextNode( snipet->description() );
		description.appendChild( text );
		
		QDomElement textElement = document.createElement( "Text" );
		s.appendChild( textElement );
		text = document.createTextNode( snipet->text() );
		textElement.appendChild( text );
		
		foreach( QString params, snipet->params() ) {
			QDomElement param = document.createElement( "Param" );
			s.appendChild( param );
			param.setAttribute( "name", params );
		}
	}

	QFile file( filename );
	if ( ! file.open( QFile::WriteOnly | QFile::Text ) )
		throw SnipetListException( QApplication::translate("SnipetList", "Cannot write file %1:\n%2.", 0, QApplication::UnicodeUTF8).arg(filename).arg(file.errorString()) );
	QTextStream out( &file );
	document.save( out, IndentSize );
}

/*!
 * Load the snipet from a file.
 * \param filename The filename used to load snipet.
 */
void SnipetList::loadFromFile( const QString & filename ) {
	d->m_categories.clear();
	qDeleteAll( d->m_list );
	d->m_list.clear();
	
	QFile file( filename );
	if( ! file.open( QFile::ReadOnly | QFile::Text ) )
		throw SnipetListException( QApplication::translate("SnipetList", "Cannot read file %1:\n%2.", 0, QApplication::UnicodeUTF8).arg(filename).arg(file.errorString()) ); 
		
	QDomDocument document( "SnipetList" );
	if( ! document.setContent( &file ) )
		throw SnipetListException( QApplication::translate("SnipetList", "Parse error exception.", 0, QApplication::UnicodeUTF8) );
	
	QDomElement root = document.documentElement();
	if( root.tagName() != "SnipetList" ) 
		throw SnipetListException( QApplication::translate("SnipetList", "Parse error exception.", 0, QApplication::UnicodeUTF8) );

	Snipet *  newSnipet;
	QDomElement snipet = root.firstChildElement( "Snipet" );
	while( ! snipet.isNull() ) {
		newSnipet = new Snipet();
		newSnipet->setName( snipet.attribute( "name" ) );
		newSnipet->setKey( snipet.attribute( "key" ) );
		newSnipet->setType( (enum Snipet::SnipetType)snipet.attribute( "type" ).toInt() );
		newSnipet->setCategory( snipet.attribute( "category" ) );
		newSnipet->setIcon( snipet.attribute( "icon" ) );
		
		QDomElement description = snipet.firstChildElement( "Description" );
		QDomNode text = description.firstChild(); 
		QString strText = "";
		while( ! text.isNull() ) {
			if( text.isText() ) 
				strText += text.toText().data();
			text = text.nextSibling();			
		}
		newSnipet->setDescription( strText );

		QDomElement textElement = snipet.firstChildElement( "Text" );
		text = textElement.firstChild(); 
		strText = "";
		while( ! text.isNull() ) {
			if( text.isText() ) 
				strText += text.toText().data();
			text = text.nextSibling();			
		}
		newSnipet->setText( strText );
		
		QDomElement param = snipet.firstChildElement( "Param" );
		while( ! param.isNull() ) {
			newSnipet->params().append( param.attribute( "name" ) );
			param = param.nextSiblingElement( "Param" );
		}
		
		add( newSnipet );		
		snipet = snipet.nextSiblingElement( "Snipet" );
	}
	emit listChanged();
}

#include "snipetlist.moc"
