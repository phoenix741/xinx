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
#include "xmlprettyprinter.h"

// Qt header
#include <QTextStream>

/* XMLPrettyPrinter */

XMLPrettyPrinter::XMLPrettyPrinter() {
}

XMLPrettyPrinter::~XMLPrettyPrinter() {
}

void XMLPrettyPrinter::setText( const QString & text ) {
	QString errorStr, content = text;
	int errorLine = 0, errorColumn = 0;
	
	content = content.replace( "&#160;", "&nbsp;" ); // Hack (Create a function that replace all chars by entity reference ?)
	
	if (! m_document.setContent( content, false, &errorStr, &errorLine, &errorColumn ) )
		throw XMLPrettyPrinterException( errorStr, errorLine, errorColumn );
}

void XMLPrettyPrinter::process() {
	m_result = QString();
	constructXML( -1, m_result, m_document );
	m_result = m_result.replace( "&nbsp;", "&#160;" ); // Hack (Create a function that replace all chars by entity reference ?)
	
}

const QString & XMLPrettyPrinter::getResult() {
	return m_result;
}

void XMLPrettyPrinter::constructXML( int level, QString & result, const QDomNode & n ) {
	QDomNode node = n;
	while( ! node.isNull() ) {
		QDomNode prev  = node.previousSibling(),
				 next  = node.nextSibling(),
				 first = node.firstChild(),
				 last  = node.lastChild();
				 
		bool prevText  = prev.isEntityReference() || prev.isCDATASection() || prev.isText(),
			 nextText  = next.isEntityReference() || next.isCDATASection() || next.isText(),
			 firstText = first.isEntityReference() || first.isCDATASection() || first.isText(),
			 lastText  = last.isEntityReference() || last.isCDATASection() || last.isText();
		
		if( node.isElement() ) {
			if( !( !prev.isNull() && prevText) )
				result.append( QString( level, '\t' ) );

			result.append( "<" );
			result.append( node.nodeName() );

			if( node.hasAttributes() )
				result.append( constructAttributes( node ) );

			if( node.hasChildNodes() ) {
				if( firstText ) 
					result.append( ">" );
				else
					result.append( ">\n" );
					
				constructXML( level + 1, result, node.firstChild() );
				if( ! lastText )
					result.append( QString( level, '\t' ) );

				result.append( "</" );
				result.append( node.nodeName() );
				result.append( ">" );
			} else {
				result.append( "/>" );
			}

			if( ! (!next.isNull() && nextText) )
				result.append( '\n' );
		} else if( node.isEntityReference() || node.isCDATASection() || node.isText() ) {
			constructXMLText( result, node );
		} else if( node.isComment() ) {
			constructXMLComment( level, result, node );
		} else {
			if( ! node.isDocument() ) {
				for ( int i = 0 ; i < level ; i++)
					result.append( '\t' );
				QTextStream text( &result );
				node.save( text, 0 );
			}
			if( node.hasChildNodes() ) {
				constructXML( level + 1, result, node.firstChild() );
			}
		}

		node = node.nextSibling();
	}
}

void XMLPrettyPrinter::constructXMLText( QString & result, const QDomNode & node ) {
	QTextStream text( &result );
	node.save( text, 0 );
}

void XMLPrettyPrinter::constructXMLComment( int level, QString & result, const QDomNode & node ) {
	QString value = node.nodeValue();
	
	QDomNode prev  = node.previousSibling(),
			 next  = node.nextSibling(),
			 first = node.firstChild(),
			 last  = node.lastChild();
			 
	bool prevText  = prev.isEntityReference() || prev.isCDATASection() || prev.isText(),
		 nextText  = next.isEntityReference() || next.isCDATASection() || next.isText();

	if( !( !prev.isNull() && prevText ) )
		result.append( QString( level, '\t' ) );

	result.append( "<!--" );
	result.append( value );
	if( value.endsWith( QLatin1Char('-') ) )
		result.append( " " );
	result.append( "-->" );

	if( ! (!next.isNull() && nextText) )
		result.append( '\n' );
}

QString XMLPrettyPrinter::constructAttributes( QDomNode node ) {
	QString result;
	QTextStream text( &result );
	QDomNamedNodeMap nnm = node.attributes();
	
	for( unsigned int i = 0; i < nnm.length() ; i++ ) {
		text << " ";
		nnm.item( i ).save( text, 0 );
	}
	
	return result;
}
