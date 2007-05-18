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

#include "texteditor.h"

#include <QSyntaxHighlighter>
#include <QKeyEvent>

TextEditor::TextEditor( QWidget * parent ) : QTextEdit( parent ) { 
	// Setup the main view
	QFont font;
	font.setFamily( "Monospace" );
	font.setFixedPitch( true );
	font.setPointSize( 8 );
  
	setAcceptRichText(false);

	setFont( font );
	setTabStopWidth( 15 );
}

TextEditor::~TextEditor() {
}

QAbstractItemModel * TextEditor::model() {
	return NULL;
}

void TextEditor::updateModel() {
	
}

void TextEditor::commentSelectedText( bool uncomment ) {
	Q_UNUSED( uncomment );
}

void TextEditor::complete() {
}

void TextEditor::keyPressEvent( QKeyEvent *e ) {
	if ( ( e->key() == Qt::Key_Home ) && ( e->modifiers() == Qt::ShiftModifier || e->modifiers() == Qt::NoModifier ) ) {
		key_home( e->modifiers() == Qt::ShiftModifier );
		e->accept();
	} else if( ( e->key() == Qt::Key_Enter ) || ( e->key() == Qt::Key_Return ) ) {
		QTextEdit::keyPressEvent( e );
		key_enter();
		e->accept();
	} else
		QTextEdit::keyPressEvent( e );
}

void TextEditor::key_enter() {
	QTextCursor tc( textCursor() );
	QTextBlock previous = tc.block().previous();	
	QString indent = previous.text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );
	tc.movePosition( QTextCursor::StartOfLine );
	tc.insertText( indent );
	setTextCursor( tc );
}

void TextEditor::key_home( bool select ) {
	QTextCursor cursor = textCursor();
	int col = cursor.columnNumber();
	cursor.movePosition( QTextCursor::StartOfLine, select ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
	QTextCursor cursorStart( cursor );
	QTextBlock b = cursorStart.block();
	int i = 0;
	while ( b.text().at(i) == ' ' || b.text().at(i) == '\t' ) {
		cursorStart.movePosition( QTextCursor::NextCharacter, select ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor );
		i++;
	}
	if( col == cursorStart.columnNumber() )
		setTextCursor( cursor );
	else
		setTextCursor( cursorStart );
}


void TextEditor::mouseDoubleClickEvent( QMouseEvent * event ) {
	QPoint mousePosition = event->pos();
	QString m_plainText = toPlainText();
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    while ( (pos>0)  && ( m_plainText.at( pos-1 ).isLetter() || (m_plainText.at( pos-1 ) == QChar('_')) ) )
        pos--;
    cursor.setPosition(pos, QTextCursor::MoveAnchor);

    while ( (pos < m_plainText.length()) && ( m_plainText.at( pos ).isLetter() || (m_plainText.at( pos ) == QChar('_')) ) )
        pos++;
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    setTextCursor( cursor );
}
