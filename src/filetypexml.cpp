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
#include "filetypexml.h"
#include "texteditor.h"
#include "exceptions.h"

// Define
#define EOWREGEXP	"[~!@\\$#%\\^&\\*\\(\\)\\+\\{\\}|\"<>\\?,/;'\\[\\]\\\\=\\s]"

FileTypeXml::FileTypeXml( TextEditor * parent ) : FileTypeInterface( parent ) {
	
}

FileTypeXml::~FileTypeXml() {
	
}
	
FileTypeXml::cursorPosition FileTypeXml::editPosition( const QTextCursor & cursor ) {
	XINX_ASSERT( ! cursor.isNull() );
	
	FileTypeXml::cursorPosition cPosition = cpNone;
	m_nodeName = QString();
	m_paramName = QString();
	
	/* is a Comment ? */
	QTextCursor cursorCommentStart ( textEdit()->document()->find ( "<!--", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorCommentEnd ( textEdit()->document()->find ( "-->", cursor, QTextDocument::FindBackward ) );

	bool inComment = ! ( 
			cursorCommentStart.isNull() // No comment before where i am
		||	( (! cursorCommentEnd.isNull()) && ( cursorCommentStart < cursorCommentEnd ) ) // There is a end before, and the last is the end balise
	 );

	if( inComment ) {
		cPosition = FileTypeXml::cpEditComment;
		return cPosition;
	} // else
	
	/* or simple text */
	QTextCursor cursorBaliseStart ( textEdit()->document()->find ( QRegExp("<(?!\\!\\-\\-)"), cursor, QTextDocument::FindBackward ) ); // A balise is'nt a comment
	QTextCursor cursorBaliseEnd ( textEdit()->document()->find ( ">", cursor, QTextDocument::FindBackward ) );

	bool inNode = (! cursorBaliseStart.isNull()) && ( cursorBaliseEnd.isNull() || ( cursorBaliseEnd < cursorBaliseStart ) );
	if( ! inNode ) {
		return cPosition; // = cpNone;
	} // else

	/* Retrieve the name of node */ /* Space ? or other ? */
	QTextCursor cursorSpaceAfterNodeName ( textEdit()->document()->find ( QRegExp( EOWREGEXP ), cursorBaliseStart ) );
	QTextCursor tc = cursor;
	
	tc.setPosition( cursorBaliseStart.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorSpaceAfterNodeName.isNull() )
		tc.setPosition( cursorSpaceAfterNodeName.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	m_nodeName = tc.selectedText().trimmed();
	
	/* i'm editing a balise name ? */
	QTextCursor cursorSpace ( textEdit()->document()->find ( QRegExp("\\s"), cursor, QTextDocument::FindBackward ) );
	if( cursorSpace < cursorBaliseStart ) 
		cursorSpace = QTextCursor();

	bool editBaliseName = cursorSpace.isNull();
	if( editBaliseName ) {
		cPosition = FileTypeXml::cpEditNodeName;
		return cPosition;
	} // else
	
	QTextCursor cursorEgal ( textEdit()->document()->find ( "=", cursor, QTextDocument::FindBackward ) );
	QTextCursor cursorQuote (textEdit()->document()->find ( "\"", cursor, QTextDocument::FindBackward ) );
	if( cursorEgal < cursorBaliseStart ) 
		cursorEgal = QTextCursor();
	if( cursorQuote < cursorBaliseStart ) 
		cursorQuote = QTextCursor();

	int quoteCount = 0;
	if( ! cursorQuote.isNull() ) {
		quoteCount ++;
		QTextCursor cursorPreviousQuote( cursorQuote );

		cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );			
		cursorPreviousQuote = textEdit()->document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
		while( ( ! cursorPreviousQuote.isNull() ) &&  ( cursorBaliseStart < cursorPreviousQuote ) ) {
			quoteCount ++;

			cursorPreviousQuote.movePosition( QTextCursor::PreviousCharacter );			
			cursorPreviousQuote = textEdit()->document()->find ( "\"", cursorPreviousQuote, QTextDocument::FindBackward );
		}
	}
	
	/* 
		<Noeud Param1=Value1 Param2="Value 2" Param3="Value3" Param4 Param5="Value 5"/>
		       1   5    10   15   20   25   30   35   40   45   50   55   60   65   70
		       
		1 => Param / Space not null, Egal null, Quote null / 0
		8 => Value / Space < Egal, Quote null / 0
		15 => Param / Egal < Space, Quote null / 0
		22 => Value / Space < Egal, Quote null / 1
		23 => Value / Space < Egal < Quote / 1
		29 => Value / Egal < Quote < Space / 1
		31 => Param / Egal < Space  < Quote	 / 2
		35 => Param / Egal < Quote < Space / 2
	
		Compter le nombre de guillement	
	*/
	
	QTextCursor cursorParam ( textEdit()->document()->find ( QRegExp( "[=\\s]" ), cursorSpace ) );
	tc.setPosition( cursorSpace.position(), QTextCursor::MoveAnchor ) ;
	if( ! cursorParam.isNull() )
		tc.setPosition( cursorParam.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
		
	m_paramName = tc.selectedText().trimmed();

	if( cursorEgal.isNull() || ( ( cursorEgal < cursorQuote ) && ( cursorEgal < cursorSpace ) && ( ( quoteCount % 2 ) == 0 ) ) ) {
		cPosition = FileTypeXml::cpEditParamName;
		return cPosition;
	}
	
	cPosition = FileTypeXml::cpEditParamValue;
	return cPosition;
}

void FileTypeXml::commentSelectedText( bool uncomment ) {
	QTextCursor cursor( textEdit()->textCursor() );
 
	QTextCursor cursorStart( textEdit()->textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( cursorStart ) == FileTypeXml::cpEditComment;

	QTextCursor cursorEnd( textEdit()->textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( cursorEnd ) == FileTypeXml::cpEditComment;

	QString text = cursor.selectedText();
	text = text.replace( "<!--", "" );
	text = text.replace( "-->", "" );
  
	cursor.beginEditBlock();

	cursor.removeSelectedText();
	if(! ( isStartCommented ^ uncomment ) ) {
		// Comment  	
		if(! uncomment)
			cursor.insertText("<!--");  
		else
			cursor.insertText("-->");  
	}
	cursor.insertText(text);  
	if(! ( isEndCommented ^ uncomment )) {
		// End the comment  	
		if(! uncomment)
			cursor.insertText("-->");  
		else
			cursor.insertText("<!--");  
	}

	cursor.endEditBlock();
}

void FileTypeXml::updateModel() {
	
}

QAbstractItemModel * FileTypeXml::model() {
	return NULL;
}

void FileTypeXml::complete() {
	
}


