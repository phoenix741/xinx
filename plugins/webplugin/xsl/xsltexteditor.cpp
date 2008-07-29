/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "xsltexteditor.h"
#include "xsllistview.h"
#include "xslmodelcompleter.h"
#include "../config/selfwebpluginsettings.h"

// Qt header
#include <QCompleter>
#include <QTextCursor>
#include <QTextBlock>

/* XslTextEditor */

XslTextEditor::XslTextEditor( QWidget * parent ) : XmlTextEditor( parent ), m_parser( 0 ) {
	
}

XslTextEditor::~XslTextEditor() {
	
}

void XslTextEditor::setParser( XslContentElementList * parser ) {
	m_parser = parser;
}

QCompleter * XslTextEditor::completer() {
	QCompleter * completer = XmlTextEditor::completer();

	if( completer ) {
		XSLCompletionModel * c = qobject_cast<XSLCompletionModel*>( completer->model() );
		if( c ) {
			c->setHiddenAttribute( QStringList() );
			cursorPosition pos = editPosition( textCursor() );
			if( pos == cpEditParamName ) {
				c->setFilter( m_nodeName );
				c->setHiddenAttribute( paramOfNode( textCursor() ) );
			} else if( pos == cpEditNodeName ) {
				c->setFilter();
			} else if( pos == cpEditParamValue ) {
				c->setFilter( m_nodeName, m_paramName );
		}

		return completer;
		}
	}
	return completer;
}


int XslTextEditor::insertCompletionBalises( QTextCursor & tc, QString node ) {
	QString indent = tc.block().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	int position = -1, cnt = 0;

	if( m_parser && SelfWebPluginSettings::self()->config().xml.addDefaultSubBalise && ( ( node == "xsl:call-template" ) || ( node == "xsl:apply-templates" ) ) ) {
		if( node == "xsl:call-template" ) {
			QTextCursor c = document()->find( QRegExp( "name=\"[^\"]*\"" ), tc, QTextDocument::FindBackward );
			if( ! c.isNull() ) {
				QString nodeName = c.selectedText();
				nodeName.remove( 0, 6 ).chop( 1 );
				foreach( QString param, m_parser->params( nodeName ) ) {
					tc.insertText( "\n" + indent + "\t" );
					tc.insertText( "<xsl:with-param name=\"" + param + "\" select=\"\"/>" );
					if( position == -1 ) position = tc.position() - 3;
					cnt++;
				}
			}
		}
	}
	
	if( cnt > 0 )
		tc.insertText( "\n" + indent );

	if( position == -1 ) { 
		position = XmlTextEditor::insertCompletionBalises( tc, node );
	} else {
		XmlTextEditor::insertCompletionBalises( tc, node );
		tc.setPosition( position );
	}
		
	return position;
}
