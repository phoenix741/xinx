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
#include "csstexteditor.h"

#include <xinxconfig.h>
#include <xinxpluginsloader.h>

// Qt header
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>

CSSTextEditor::CSSTextEditor( QWidget * parent ) : TextEditor( parent ) {
	SyntaxHighlighter * highlighter = XinxPluginsLoader::self()->createHighlighter( "CSS", document() );
	setHighlighter( highlighter );
}

CSSTextEditor::~CSSTextEditor() {

}

void CSSTextEditor::commentSelectedText( bool uncomment ) {
	QTextCursor cursor( textCursor() );

	QTextCursor cursorStart( textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( this, cursorStart ) == cpEditComment;

	QTextCursor cursorEnd( textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( this, cursorEnd ) == cpEditComment;

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


CSSTextEditor::cursorPosition CSSTextEditor::editPosition( const TextEditor * textEdit, const QTextCursor & cursor ) {
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

CSSTextEditor::cursorPosition CSSTextEditor::editPosition( const QTextCursor & cursor ) {
	return editPosition( this, cursor );
}
