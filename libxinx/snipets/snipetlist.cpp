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
#include "snipetlist.h"

// Qt header
#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QTextStream>
#include <QApplication>

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

QStringList SnipetList::categories() const {
	QStringList result;
	foreach( const Snipet & snipet, *this ) {
		if( ! result.contains( snipet.category() ) )
			result << snipet.category();
	}
	return result;
}

int SnipetList::indexOf( const QString & key, int from ) const {
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
		s.setAttribute( "category", snipet.category() );
		s.setAttribute( "icon", snipet.icon() );
		s.setAttribute( "automatique", snipet.callIsAutomatic() );

		QDomElement description = document.createElement( "Description" );
		s.appendChild( description );
		QDomText text = document.createTextNode( snipet.description() );
		description.appendChild( text );

		QDomElement textElement = document.createElement( "Text" );
		s.appendChild( textElement );
		text = document.createTextNode( snipet.text() );
		textElement.appendChild( text );

		QDomElement availableScript = document.createElement( "AvailableScript" );
		s.appendChild( availableScript );
		text = document.createTextNode( snipet.availableScript() );
		availableScript.appendChild( text );
		
		QDomElement parentCategory = s;
		foreach( const QString & category, categories() ) {
			QDomElement categoryElement = document.createElement( "Category" );
			parentCategory.appendChild( categoryElement );
			categoryElement.setAttribute( "name", category );
			parentCategory = categoryElement;
		}
		
		
		foreach( const Snipet::Parameter & snipetParam, snipet.params() ) {
			QDomElement param = document.createElement( "Param" );
			s.appendChild( param );
			param.setAttribute( "name", snipetParam.name );
			param.setAttribute( "defaultValue", snipetParam.defaultValue );
		}
		
		foreach( const QString & extention, snipet.extentions() ) {
			QDomElement extentionElement = document.createElement( "Extentions" );
			s.appendChild( extentionElement );
			extentionElement.setAttribute( "value", extention );
		}
	}

	QFile file( filename );
	if ( ! file.open( QFile::WriteOnly ) )
		throw SnipetListException( QApplication::translate("SnipetList", "Cannot write file %1:\n%2.").arg(filename).arg(file.errorString()) );
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
		newSnipet.setIcon( snipet.attribute( "icon" ) );
		newSnipet.setCallIsAutomatic( snipet.attribute( "automatique" ).toInt() != 1 );

		QDomElement description = snipet.firstChildElement( "Description" );
		newSnipet.setDescription( description.text() );

		QDomElement textElement = snipet.firstChildElement( "Text" );
		newSnipet.setText( textElement.text() );
		
		QDomElement availableScriptElement = snipet.firstChildElement( "AvailableScript" );
		newSnipet.setAvailableScript( availableScriptElement.text() );
	
		QDomElement param = snipet.firstChildElement( "Param" );
		while( ! param.isNull() ) {
			Snipet::Parameter snipetParameter;
			snipetParameter.name = param.attribute( "name" );
			snipetParameter.defaultValue = param.attribute( "defaultValue" );
			newSnipet.params().append( snipetParameter );
			param = param.nextSiblingElement( "Param" );
		}

		QString type = snipet.attribute( "type" );
		if( ! type.isEmpty() ) {
			newSnipet.setType( snipet.attribute( "type" ) );
		} else {
			QDomElement extentionElement = snipet.firstChildElement( "Extention" );
			while( ! extentionElement.isNull() ) {
				type = extentionElement.attribute( "value" );
				newSnipet.extentions().append( type );
				extentionElement = extentionElement.nextSiblingElement( "Extention" );
			}
		}
		
		QString category = snipet.attribute( "category" );
		if( ! type.isEmpty() ) {
			newSnipet.setCategory( snipet.attribute( "category" ) );
		} else {
			QDomElement categoryElement = snipet.firstChildElement( "Category" );
			while( ! categoryElement.isNull() ) {
				category = categoryElement.attribute( "value" );
				newSnipet.categories().append( category );
				categoryElement = categoryElement.firstChildElement( "Category" );
			}
		}
		
		append( newSnipet );
		snipet = snipet.nextSiblingElement( "Snipet" );
	}
	qSort( *this );
}

SnipetList SnipetList::categorie( const QString & category ) {
	SnipetList result;
	foreach( const Snipet & s, *this ) {
		if( s.category() == category ) result += s;
	}
	return result;
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

void SnipetListManager::loadFromSnipetFile() {
	m_snipets.clear();
	m_snipets.loadFromFile( "datas:template.xml" );
	emit listChanged();
}

void SnipetListManager::saveToSnipetFile() {
	m_snipets.saveToFile( "datas:template.xml" );
}

const SnipetList & SnipetListManager::snipets() const {
	return m_snipets;
}

void SnipetListManager::setSnipets( const SnipetList & list ) {
	if( list != m_snipets ) {
		m_snipets = list;
		emit listChanged();
	}
}

