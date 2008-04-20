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
#include "private/p_snipetlist.h"

/* Static member */

SnipetListManager * SnipetListManager::s_self = 0;

/* SnipetListException */

SnipetListException::SnipetListException( const QString & message ) : XinxException( message ) {
}
	
/* SnipetList */

SnipetList::SnipetList() {

}

SnipetList::~SnipetList() {

}

void PrivateSnipetList::addCategory( QString newCategory ) {
	if( ! m_categories.contains( newCategory ) )
		m_categories.append( newCategory );
}

QStringList SnipetList::categories() const {
	QStringList result;
	foreach( const Snipet & snipet, *this ) {
		if( ! result.contains( snipet.category() ) )
			result << snipet.category();
	}
	return result;
}

int SnipetList::indexOf( const QString & key, int from ) {
	for( int i = from ; i < size() ; i++ ) {
		const Snipet & snipet = at( i );
		if( snipet.key() == key )
			return i;
	}
	return -1;
}

void SnipetList::saveToFile( const QString & filename ) {
	const int IndentSize = 2;

	QDomDocument document( "SnipetList" );
	QDomNode xmlCodec = document.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" );
	document.appendChild( xmlCodec );
	QDomElement root = document.createElement( "SnipetList" );
	document.appendChild( root );
	
	foreach( const Snipet & snipet, *this ) {
		QDomElement s = document.createElement( "Snipet" );
		root.appendChild( s );
		
		s.setAttribute( "name", snipet.name() );
		s.setAttribute( "key", snipet.key() );
		s.setAttribute( "type", snipet.type() );
		s.setAttribute( "category", snipet.category() );
		s.setAttribute( "icon", snipet.œicon() );
		
		QDomElement description = document.createElement( "Description" );
		s.appendChild( description );
		QDomText text = document.createTextNode( snipet.œdescription() );
		description.appendChild( text );
		
		QDomElement textElement = document.createElement( "Text" );
		s.appendChild( textElement );
		text = document.createTextNode( snipet.text() );
		textElement.appendChild( text );
		
		foreach( QString params, snipet.params() ) {
			QDomElement param = document.createElement( "Param" );
			s.appendChild( param );
			param.setAttribute( "name", params );
		}
	}

	QFile file( filename );
	if ( ! file.open( QFile::WriteOnly ) )
		throw SnipetListException( QApplication::translate("SnipetList", "Cannot write file %1:\n%2.").arg(usedFilename).arg(file.errorString()) );
	QTextStream out( &file );
	document.save( out, IndentSize );
}

void SnipetList::loadFromFile( const QString & filename ) {
	QFile file( filename );
	if( ! file.open( QFile::ReadOnly ) )
		throw SnipetListException( QApplication::translate("SnipetList", "Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()) ); 
		
	QDomDocument document( "SnipetList" );
	if( ! document.setContent( &file ) )
		throw SnipetListException( QApplication::translate("SnipetList", "Parse error exception.") );
	
	QDomElement root = document.documentElement();
	if( root.tagName() != "SnipetList" ) 
		throw SnipetListException( QApplication::translate("SnipetList", "Parse error exception.") );

	QDomElement snipet = root.firstChildElement( "Snipet" );
	while( ! snipet.isNull() ) {
		Snipet newSnipet;
		newSnipet.setName( snipet.attribute( "name" ) );
		newSnipet.setKey( snipet.attribute( "key" ) );
		newSnipet.setType( snipet.attribute( "type" ) );
		newSnipet.setCategory( snipet.attribute( "category" ) );
		newSnipet.setIcon( snipet.attribute( "icon" ) );
		
		QDomElement description = snipet.firstChildElement( "Description" );
		newSnipet.setDescription( description.text() );

		QDomElement textElement = snipet.firstChildElement( "Text" );
		newSnipet->setText( textElement.text() );
		
		QDomElement param = snipet.firstChildElement( "Param" );
		while( ! param.isNull() ) {
			newSnipet.params().append( param.attribute( "name" ) );
			param = param.nextSiblingElement( "Param" );
		}

		add( newSnipet );		
		snipet = snipet.nextSiblingElement( "Snipet" );
	}
}

/* SnipetListManager */

SnipetListManager::SnipetListManager() {
	
}

SnipetListManager::~SnipetListManager() {
	if( s_self == this )
		s_self = 0;
}

SnipetListManager * SnipetListManager::self() {
	if( s_self == 0 ) {
		s_self = new SnipetListManager();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}


