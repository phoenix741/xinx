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
#include "csscompleter.h"
#include "iextendededitor.h"

// Qt header
#include <QCompleter>
#include <QTextEdit>
#include <QRegExp>

/* CssCompleter */

CssCompleter::CssCompleter( IXinxExtendedEditor * editor ) : m_editor( editor ) {
	
}

CssCompleter::~CssCompleter() {
	
}
	
void CssCompleter::commentSelectedText( IXinxExtendedEditor * editor, bool uncomment ) {
	QTextCursor cursor( editor->qTextEdit()->textCursor() );
 
	QTextCursor cursorStart( editor->qTextEdit()->textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( editor->qTextEdit(), cursorStart ) == cpEditComment;

	QTextCursor cursorEnd( editor->qTextEdit()->textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( editor->qTextEdit(), cursorEnd ) == cpEditComment;

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

CssCompleter::cursorPosition CssCompleter::editPosition( const QTextEdit * textEdit, const QTextCursor & cursor ) {
	QTextCursor cursorStartOfComment = textEdit->document()->find( "/*", cursor, QTextDocument::FindBackward );
	QTextCursor cursorEndOfComment   = textEdit->document()->find( "*/", cursor, QTextDocument::FindBackward );
	
	bool inComment = 
		(! ( cursorStartOfComment.isNull() || ( !cursorEndOfComment.isNull() && (cursorStartOfComment < cursorEndOfComment ) ) ));
	
	if( inComment ) 
		return cpEditComment;
	
	int bloc = 0;
	QTextCursor c = textEdit->document()->find( "{", cursor, QTextDocument::FindBackward );
	while( c < cursor ) {
		QString text = c.selectedText();
		if( text.contains( '{' ) )
			bloc++;
		else if( text.contains( '}' ) )
			bloc--;
		
		c = textEdit->document()->find( QRegExp("[\\S]+"), c );
	}
	
	if( bloc > 0 )
		return cpEditTag;
	else {
		return cpEditGlobal;
	}
}

CssCompleter::cursorPosition CssCompleter::editPosition( const QTextCursor & cursor ) {
	return editPosition( m_editor->qTextEdit(), cursor );
}

