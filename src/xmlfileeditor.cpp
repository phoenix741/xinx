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
	
	static void constructXML( int level, QString & result, const QDomNode & n, bool insertTab );
	static void constructXMLText( QString & result, const QDomNode & n );
	static QString constructAttributes( QDomNode node );
private:
	XMLFileEditor * m_parent;
};

PrivateXMLFileEditor::PrivateXMLFileEditor( XMLFileEditor * parent ) : m_parent( parent ) {
	
}

PrivateXMLFileEditor::~PrivateXMLFileEditor() {
	
}

void PrivateXMLFileEditor::constructXML( int level, QString & result, const QDomNode & n, bool insertTab ) {
	QDomNode node = n;
	while( ! node.isNull() ) {
		if( node.isElement() ) {
			if( insertTab )
				for ( int i = 0 ; i < level ; i++)
					result.append( '\t' );
			result.append( '<' );
			result.append( node.nodeName() );

			if( node.hasAttributes() )
				result.append( constructAttributes( node ) );

			if( node.hasChildNodes() ) {
				result.append( '>' );
				if( node.firstChild().isCDATASection() || node.firstChild().isText() ) {
					constructXML( level + 1, result, node.firstChild(), false );
				} else {
					result.append('\n');
					constructXML( level + 1, result, node.firstChild(), true );
					for ( int i = 0; i < level; i++ )
						result.append( '\t' );
				}

				result.append( "</" );
				result.append( node.nodeName() );
				result.append( ">" );
			} else {
				result.append( "/>" );
			}
			if( insertTab )
				result.append( '\n' );
		} else if( node.isCDATASection() || node.isText() ) {
			constructXMLText( result, node );
		} else {
			if( ! node.isDocument() ) {
				for ( int i = 0 ; i < level ; i++)
					result.append( '\t' );
				QTextStream text( &result );
				node.save( text, 0 );
			}
			if( node.hasChildNodes() ) {
				constructXML( level + 1, result, node.firstChild(), true );
			}
		}
		
		node = node.nextSibling();
	}
}

void PrivateXMLFileEditor::constructXMLText( QString & result, const QDomNode & node ) {
	QString toStore;
	QTextStream text( &toStore );
	node.save( text, 0 );
	if( ! toStore.simplified().isEmpty() )
		result += toStore.trimmed();
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

XMLFileEditor::XMLFileEditor( QWidget *parent, TextEditor * textEditor ) : FileEditor( textEditor ? textEditor : new XMLEditor( parent ), parent ) {
	d = new PrivateXMLFileEditor( this );
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
	QString errorStr;
	int errorLine = 0, errorColumn = 0;  
	
	if ( document.setContent(textEdit()->toPlainText(), true, &errorStr, &errorLine, &errorColumn) ) {
		document.normalize();
		QString result;
		d->constructXML( -1, result, document, true );

		textEdit()->selectAll();
		QTextCursor cursor = textEdit()->textCursor();
		cursor.beginEditBlock();
		cursor.removeSelectedText();
		cursor.insertText( result );
		cursor.endEditBlock();
		textEdit()->setTextCursor( cursor );
	} else {
		setMessage( tr("Parse error line %1 column %2 : %3").arg(errorLine).arg(errorColumn).arg(errorStr) );
	}
}

XSLFileEditor::XSLFileEditor( QWidget *parent ) : XMLFileEditor( parent, new XSLEditor( parent ) ) {
	connect( m_view, SIGNAL(hasError(QString)), this, SLOT(setMessage(QString)) );
}

XSLFileEditor::~XSLFileEditor() {
	
}

QString XSLFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "xsl";
	else
		return XMLFileEditor::getSuffix();
}
