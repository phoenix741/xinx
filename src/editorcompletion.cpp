/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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
 
#include "editorcompletion.h"

#include <QDomElement>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

/* Completion */

Completion * completionContents = NULL;


Completion::Completion( const QString & name ) : m_name( name ) {
	if( ! m_name.isEmpty() )
		load();
}

Completion::~Completion() {
	qDeleteAll( m_xmlBalises );
}

void Completion::load() {
	QFile file( m_name );
	QDomDocument objectFile;
  
	// Open the file
	if ( !file.open( QFile::ReadOnly | QFile::Text ) ) {
		QMessageBox::warning( 
			qApp->activeWindow(), 
			QApplication::translate("Completion", "Completion"), 
			QApplication::translate("Completion", "Cannot read file %1:\n%2.").arg( m_name ).arg( file.errorString() ) );
		throw ENotCompletionFile( QApplication::translate("Completion", "Can't open or read the file %1.").arg( m_name ) );
	}

	// Load XML Document
	QString errorStr;
	int errorLine;
	int errorColumn;  
	if ( !objectFile.setContent(&file, false, &errorStr, &errorLine, &errorColumn) ) {
		QMessageBox::information(
			qApp->activeWindow(), 
			QApplication::translate("Completion", "Completion"), 
			QApplication::translate("Completion", "Parse error at line %1, column %2:\n%3")
				.arg(errorLine)
				.arg(errorColumn)
				.arg(errorStr) );
		throw ENotCompletionFile( QApplication::translate("Completion", "Can't parse the file %1.").arg( m_name ) );
	}  
  
	QDomElement root = objectFile.documentElement();
  
	// Test if Completion
	if( root.tagName() != "completion" ) 
		throw ENotCompletionFile( QApplication::translate("Completion", "%1 is not auto completion file").arg( m_name ) );
  	
	// Read the XML part of the completion
	QDomElement xml = root.firstChildElement( "xml" );
	if( ! xml.isNull() ) {
		QDomElement type = xml.firstChildElement( "type" ); // Read the category to use. (html, stylesheet)
		while( !type.isNull() ) {
			QString typeName = type.attribute( "name", "other" ); // If no category, use "other"
			
			QDomNodeList balises = type.elementsByTagName( "balise" ); // Search ALL the children balise (balise, and children of balise
			for( int i = 0; i < balises.count(); i++ ) // For each append to list
				if( balises.at( i ).isElement() ) 
					m_xmlBalises.append( new CompletionXMLBalise( typeName, balises.at( i ).toElement() ) );
				
			
			type = type.nextSiblingElement( "type" );
		};
	}
}

CompletionXMLBalise* Completion::balise( const QString & name ) const {
	foreach( CompletionXMLBalise* b, m_xmlBalises ) { // Search the balise with the name ...
		if( b->name() == name ) 
			return b;
	}
	return NULL;
}


/* CompletionXMLBalise */

CompletionXMLBalise::CompletionXMLBalise( const QString & category, const QDomElement & node ) : m_category( category ) {
	// Read node attribute
	m_name = node.attribute( "name" );
	m_isDefault = node.attribute( "default" ) == "true";
	
	/* If children balise is present, add them as child */
	QDomNodeList balises = node.elementsByTagName( "balise" ); 
	for( int i = 0; i < balises.count(); i++ ) 
		if( balises.at( i ).isElement() ) 
			m_balises.append( new CompletionXMLBalise( category, balises.at( i ).toElement() ) );
	
	/* Read all attribute for the balise */
	QDomElement attribute = node.firstChildElement( "attribute" );
	while( ! attribute.isNull() ) {
		m_attributes.append( new CompletionXMLAttribute( attribute ) );
		attribute = attribute.nextSiblingElement( "attribute" );
	}
}

CompletionXMLBalise::~CompletionXMLBalise() {
	qDeleteAll( m_attributes );
	qDeleteAll( m_balises );
}

CompletionXMLBalise* CompletionXMLBalise::balise( const QString & name ) const {
	foreach( CompletionXMLBalise* b, m_balises ) {
		if( b->name() == name ) 
			return b;
	}
	return NULL;
}

CompletionXMLAttribute* CompletionXMLBalise::attribute( const QString & name ) const {
	foreach( CompletionXMLAttribute* b, m_attributes ) {
		if( b->name() == name ) 
			return b;
	}
	return NULL;
}


/* CompletionXMLAttribute */

CompletionXMLAttribute::CompletionXMLAttribute( const QDomElement & node ) {
	m_name = node.attribute( "name" );
	m_isDefault = node.attribute( "default" ) == "true";
	m_defaultValue = -1;

	QDomNodeList values = node.elementsByTagName( "value" );
	for( int i = 0; i < values.count(); i++ ) 
		if( values.at( i ).isElement() ) {
			QDomElement value = values.at( i ).toElement();
			
			m_values.append( value.text() );
			
			if( ( m_defaultValue == -1 ) && ( value.attribute( "default" ) == "true" ) ) {
				m_defaultValue = m_values.size() - 1;
			}
		}
}

CompletionXMLAttribute::~CompletionXMLAttribute() {
	
}


