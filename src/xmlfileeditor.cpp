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

// Xinx header
#include "xmlfileeditor.h"
#include "xmleditor.h"
#include "xsleditor.h"

// Qt header
#include <QDomDocument>
#include <QTextStream>

/* PrivateXMLFileEditor */

class PrivateXMLFileEditor {
public:
	PrivateXMLFileEditor( XMLFileEditor * parent );
	~PrivateXMLFileEditor();
	
	static void constructXML( int level, QString & result, const QDomNode & n );
	static void constructXMLText( QString & result, const QDomNode & n );
	static void constructXMLComment( int level, QString & result, const QDomNode & n );
	static QString constructAttributes( QDomNode node );
private:
	XMLFileEditor * m_parent;
};

PrivateXMLFileEditor::PrivateXMLFileEditor( XMLFileEditor * parent ) : m_parent( parent ) {
	
}

PrivateXMLFileEditor::~PrivateXMLFileEditor() {
	
}

void PrivateXMLFileEditor::constructXML( int level, QString & result, const QDomNode & n ) {
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

void PrivateXMLFileEditor::constructXMLText( QString & result, const QDomNode & node ) {
	QTextStream text( &result );
	node.save( text, 0 );
}

void PrivateXMLFileEditor::constructXMLComment( int level, QString & result, const QDomNode & node ) {
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

QString PrivateXMLFileEditor::constructAttributes( QDomNode node ) {
	QString result;
	QTextStream text( &result );
	QDomNamedNodeMap nnm = node.attributes();
	
	for( unsigned int i = 0; i < nnm.length() ; i++ ) {
		text << " ";
		nnm.item( i ).save( text, 0 );
	}
	
	return result;
}

/* XMLFileEditor */

Q_DECLARE_METATYPE( XMLFileEditor );

XMLFileEditor::XMLFileEditor( QWidget *parent ) : FileEditor( parent ) {
	d = new PrivateXMLFileEditor( this );
	setSyntaxHighlighterType( FileEditor::XMLHighlighter );
	setFileType( FileEditor::XMLFileType );
}

XMLFileEditor::~XMLFileEditor() {
	delete d;
}

QString XMLFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "xml";
	else
		return FileEditor::getSuffix();
}

void XMLFileEditor::autoIndent() {
	QDomDocument document;
	QString errorStr, content;
	int errorLine = 0, errorColumn = 0;  
	content = textEdit()->toPlainText();
	content = content.replace( "&#160;", "&nbsp;" );
	if ( document.setContent(content, false, &errorStr, &errorLine, &errorColumn) ) {
//		document.normalize();
		QString result;
		d->constructXML( -1, result, document );
		result = result.replace( "&nbsp;", "&#160;" ); // Hack (Create a function that replace all chars by entity reference ?)

		textEdit()->selectAll();
		QTextCursor cursor = textEdit()->textCursor();
		cursor.beginEditBlock();
		cursor.removeSelectedText();
		cursor.insertText( result );
//		cursor.insertText( document.toString( 2 ) );
		cursor.endEditBlock();
		textEdit()->setTextCursor( cursor );
	} else {
		setMessage( tr("Parse error line %1 column %2 : %3").arg(errorLine).arg(errorColumn).arg(errorStr) );
	}
}

QIcon XMLFileEditor::icon() {
	return QIcon( ":/images/typexml.png" );
}

/* XSLFileEditor */

Q_DECLARE_METATYPE( XSLFileEditor );

XSLFileEditor::XSLFileEditor( QWidget *parent ) : XMLFileEditor( parent ) {
	setFileType( FileEditor::XSLFileType );
}

XSLFileEditor::~XSLFileEditor() {
	
}

QString XSLFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "xsl";
	else
		return XMLFileEditor::getSuffix();
}

QIcon XSLFileEditor::icon() {
	return QIcon( ":/images/typexsl.png" );
}
