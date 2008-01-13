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
#include "jscompleter.h"
#include "javascriptmodelcompleter.h"

// Qt header
#include <QCompleter>
#include <QTextEdit>
#include <QRegExp>

/* JsCompleter */

JsCompleter::JsCompleter( IXinxExtendedEditor * editor ) : m_editor( editor ) {
	m_modelCompleter = NULL;
	m_completer 	 = NULL;

	m_completer = new QCompleter( this );
	m_completer->setWidget( editor->qTextEdit() );
	m_completer->setCompletionMode( QCompleter::PopupCompletion );
	m_completer->setCaseSensitivity( Qt::CaseInsensitive );
	m_completer->setCompletionRole( Qt::DisplayRole );
	connect( m_completer, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );

	m_modelCompleter = new JavascriptModelCompleter( editor->modelData(), this );
	m_completer->setModel( m_modelCompleter );
}

JsCompleter::~JsCompleter() {
	delete m_completer;
	delete m_modelCompleter;
}

void JsCompleter::commentSelectedText( IXinxExtendedEditor * editor, bool uncomment ) {
	QString functionName;
	
	QTextCursor cursor( editor->qTextEdit()->textCursor() );
 
	QTextCursor cursorStart( editor->qTextEdit()->textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( editor->qTextEdit(), cursorStart, functionName ) == cpEditLongComment;

	QTextCursor cursorEnd( editor->qTextEdit()->textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( editor->qTextEdit(), cursorEnd, functionName ) == cpEditLongComment;

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

bool JsCompleter::keyPressEvent( QKeyEvent *e ) {
	Q_UNUSED( e );
	editPosition( m_editor->qTextEdit()->textCursor() );
	if( m_modelCompleter )
		m_modelCompleter->setFilter( m_functionName );
	return false;
}

QCompleter * JsCompleter::currentCompleter( const QTextCursor & cursor ) {
	Q_UNUSED( cursor );
	return m_completer;
}

void JsCompleter::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = m_editor->qTextEdit()->textCursor();
	QCompleter * c = m_completer;
	
	QString completion = c->completionModel()->data( index ).toString(),
			prefix     = c->completionPrefix();
	
	m_editor->textUnderCursor( tc, true );
	tc.insertText( completion );

	m_editor->qTextEdit()->setTextCursor( tc );
}

JsCompleter::cursorPosition JsCompleter::editPosition( const QTextEdit * textEdit, const QTextCursor & cursor, QString & functionName ) {
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
	while( c < cursor ) {
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

JsCompleter::cursorPosition JsCompleter::editPosition( const QTextCursor & cursor ) {
	return editPosition( m_editor->qTextEdit(), cursor, m_functionName );
}

