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
#include "jstexteditor.h"
#include "javascriptmodelcompleter.h"
#include "../config/selfwebpluginsettings.h"

#include <xinxconfig.h>
#include <xinxpluginsloader.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>

JSTextEditor::JSTextEditor( QWidget * parent ) : TextEditor( parent ) {
	SyntaxHighlighter * highlighter = XinxPluginsLoader::self()->createHighlighter( "JS", document() );
	setHighlighter( highlighter );
}

JSTextEditor::~JSTextEditor() {

}

QCompleter * JSTextEditor::completer() {
	if( ! SelfWebPluginSettings::self()->config().javascript.activeCompletion ) return 0;

	if( TextEditor::completer() ) {
		JavascriptModelCompleter * c = qobject_cast<JavascriptModelCompleter*>( TextEditor::completer()->model() );
		if( c ) {
			editPosition( textCursor() );
			c->setFilter( m_functionName );

			return TextEditor::completer();
		}
	}
	return TextEditor::completer();
}

void JSTextEditor::commentSelectedText( bool uncomment ) {
	QString functionName;

	QTextCursor cursor( textCursor() );

	QTextCursor cursorStart( textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( this, cursorStart, functionName ) == cpEditLongComment;

	QTextCursor cursorEnd( textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( this, cursorEnd, functionName ) == cpEditLongComment;

	QString text = cursor.selectedText();
	text = text.replace( "/*", "" );
	text = text.replace( "*/", "" );

	cursor.beginEditBlock();

	cursor.removeSelectedText();
	if(! ( isStartCommented ^ uncomment ) ) {
		// Comment
		if(! uncomment)
			cursor.insertText("/*");
		else
			cursor.insertText("*/");
	}
	cursor.insertText(text);
	if(! ( isEndCommented ^ uncomment )) {
		// End the comment
		if(! uncomment)
			cursor.insertText("*/");
		else
			cursor.insertText("/*");
	}

	cursor.endEditBlock();
}


JSTextEditor::cursorPosition JSTextEditor::editPosition( const TextEditor * textEdit, const QTextCursor & cursor, QString & functionName ) {
	QTextCursor cursorStartOfComment = textEdit->document()->find( "/*", cursor, QTextDocument::FindBackward );
	QTextCursor cursorEndOfComment   = textEdit->document()->find( "*/", cursor, QTextDocument::FindBackward );
	QTextCursor cursorLineCommented  = textEdit->document()->find( "//", cursor, QTextDocument::FindBackward );

	functionName = QString();

	if(! ( cursorStartOfComment.isNull() || ( !cursorEndOfComment.isNull() && (cursorStartOfComment < cursorEndOfComment ) ) ))
		return cpEditLongComment;
	if(! cursorLineCommented.isNull() && ( cursorLineCommented.blockNumber() == cursor.blockNumber() ) )
		return cpEditSimpleComment;

	QRegExp function( "function[\\s]*([a-zA-Z_][a-zA-Z0-9_]*)[\\s]*\\(" );
	QTextCursor cursorFunction = textEdit->document()->find( function, cursor, QTextDocument::FindBackward );
	QTextCursor endOfParam     = textEdit->document()->find( ")", cursor, QTextDocument::FindBackward );

	if( cursorFunction.isNull() )
		return cpEditGlobal;

	QTextCursor cursorOfFunctionName = cursorFunction;
	functionName = cursorOfFunctionName.selectedText();
	function.indexIn( functionName, 0 );
	functionName = function.cap(1);

	if( endOfParam.isNull() || ( endOfParam < cursorFunction ) ) {
		return cpEditParams;
	}

	int bloc = 0;
	QTextCursor c = textEdit->document()->find( ")", cursorFunction );
	while( ( c < cursor ) && ( c.position() > 0 ) ) {
		QString text = c.selectedText();
		if( text.contains( '{' ) )
			bloc++;
		else if( text.contains( '}' ) )
			bloc--;

		c = textEdit->document()->find( QRegExp("[\\S]+"), c );
	}

	if( bloc > 0 )
		return cpEditFunction;
	else {
		functionName = QString();
		return cpEditGlobal;
	}
}

JSTextEditor::cursorPosition JSTextEditor::editPosition( const QTextCursor & cursor ) {
	return editPosition( this, cursor, m_functionName );
}
