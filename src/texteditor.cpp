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
#include "texteditor.h"
#include "globals.h"
#include "xinxconfig.h"
#include "exceptions.h"

// Qt header
#include <QTextBlock>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QScrollBar>
#include <QTextLayout>
#include <QPainter>
#include <QPixmap> 

/*!
 * Definition of the characters that can't be in a word.
 */
#define EOWREGEXP		"[^A-Za-z0-9_:-]"
#define EOWREGEXPDOT	"[^A-Za-z0-9_:-.]"

/*!
 * Pixmap used to represent the tabulation
 */
static const char * tabPixmap_img[] = {
/* width height ncolors cpp [x_hot y_hot] */
	"8 8 3 2 0 0",
/* colors */
	"  s none       m none  c none",
	"O s iconColor1 m black c black",
	"X s iconColor2 m black c #D0D0D0",
/* pixels */
	"  X     X       ",
	"    X     X     ",
	"      X     X   ",
	"        X     X ",
	"      X     X   ",
	"    X     X     ",
	"  X     X       ",
	"                ",
};

/*!
 * Pixmap used to represent a space
 */
static const char * spacePixmap_img[] = {
/* width height ncolors cpp [x_hot y_hot] */
	"8 8 3 2 0 0",
/* colors */
	"  s none       m none  c none",
	"O s iconColor1 m black c black",
	"X s iconColor2 m black c #D0D0D0",
/* pixels */
	"                ",
	"                ",
 	"                ",
	"                ",
	"                ",
	"      X         ",
	"      X X       ",
	"                ",
};

/* TextEditor */

TextEditor::TextEditor( QWidget * parent ) : QTextEdit( parent ) { 
	// Setup the main view
	
    m_tabPixmap = QPixmap( tabPixmap_img ); 
	m_spacePixmap = QPixmap( spacePixmap_img ); 
	m_currentLineColor = QColor( 234, 234, 234 );	

    connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( slotCursorPositionChanged()));
    connect( &global, SIGNAL( configChanged() ), this, SLOT( updateFont() ) );

	setAcceptRichText(false);
	updateFont();
}

TextEditor::~TextEditor() {
}

void TextEditor::updateFont() {
	QFont font = global.m_config->config().editor.defaultFormat;
	QFontMetrics fm( font );
	setFont( font );
	setTabStopWidth( fm.width("M") * global.m_config->config().editor.tabulationSize );
}

int TextEditor::currentColumn() {
	QTextCursor tc( textCursor() );
	return tc.columnNumber() + 1;
}

int TextEditor::currentRow() {
	QTextCursor tc( textCursor() );
	return tc.blockNumber() + 1;
}

void TextEditor::keyPressExecute( QKeyEvent * e ) {
	bool isShortcut = ( (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space ); // CTRL+Space
	
	if ( ( e->key() == Qt::Key_Home ) && ( e->modifiers() == Qt::ShiftModifier || e->modifiers() == Qt::NoModifier ) ) {
		key_home( e->modifiers() == Qt::ShiftModifier );
		e->accept();
	} else if( ( e->key() == Qt::Key_Enter ) || ( e->key() == Qt::Key_Return ) ) {
		QTextEdit::keyPressEvent( e );
		key_enter();
		e->accept();
	} else if( isShortcut ) {
		QString snipet = textUnderCursor( textCursor(), true );
		emit needInsertSnipet( snipet );
		e->accept();
	} else
		QTextEdit::keyPressEvent( e );
	m_keyPressExecute = false;
}

void TextEditor::keyPressSkip( QKeyEvent * e ) {
	Q_UNUSED( e );
	m_keyPressExecute = false;
}

void TextEditor::keyPressEvent( QKeyEvent * e ) {
	m_keyPressExecute = true;
	emit execKeyPressEvent( e );
	
	if( m_keyPressExecute ) {
		keyPressExecute( e );
	}
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
	while ( ( i < b.text().size() ) && ( i >= 0 ) && ( b.text().at(i) == ' ' || b.text().at(i) == '\t' ) ) {
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
    while ( (pos>0)  && ( m_plainText.at( pos-1 ).isLetter() || m_plainText.at( pos-1 ).isNumber() || (m_plainText.at( pos-1 ) == QChar('_')) ) )
        pos--;
    cursor.setPosition(pos, QTextCursor::MoveAnchor);

    while ( (pos < m_plainText.length()) && ( m_plainText.at( pos ).isLetter() || m_plainText.at( pos ).isNumber() || (m_plainText.at( pos ) == QChar('_')) ) )
        pos++;
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    setTextCursor( cursor );
}

QString TextEditor::textUnderCursor( const QTextCursor & cursor, bool deleteWord, bool dot ) {
	XINX_ASSERT( ! cursor.isNull() );
	QString expr = EOWREGEXPDOT;
	if( ! dot ) 
		expr = EOWREGEXP;
	
	QTextCursor before ( document()->find ( QRegExp( expr ), cursor, QTextDocument::FindBackward ) );
	QTextCursor after ( document()->find ( QRegExp( expr ), cursor ) );

	QTextCursor tc = cursor;
	
	if( ( ! before.isNull() ) && ( before.block() == tc.block() ) )
		tc.setPosition( before.position(), QTextCursor::MoveAnchor );
	else
		tc.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor ) ;
		
	if( ( ! after.isNull() ) && ( after.block() == tc.block() ) )
		tc.setPosition( after.position() - 1, QTextCursor::KeepAnchor ) ;
	else
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor ) ;
	
	QString selection = tc.selectedText().trimmed();
		
	if( ( ! tc.selectedText().trimmed().isEmpty() ) && deleteWord ) {
		tc.removeSelectedText();
		setTextCursor( tc );
	}
		
	return selection;
}

void TextEditor::insertText( const QString & text ) {
	QString txt = text, left, right;
	QTextCursor cursor = textCursor();
	QTextBlock previous = cursor.block().previous();	
	QString indent = previous.text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) ) + "\t";

	while( txt.contains( "\n" ) ) {
		int index  = txt.indexOf( "\n" );
		QString left = txt.left( index ),
				rigth = txt.mid( index + 1 );
		cursor.insertText( left );
		txt = rigth;
		cursor.insertText( "\n" + indent );
	}
	cursor.insertText( txt + " " );
	setTextCursor( cursor );
}

void TextEditor::printWhiteSpaces( QPainter &p ) {		
	const int contentsY = verticalScrollBar()->value();
	const qreal pageBottom = contentsY + viewport()->height();
	const QFontMetrics fm = QFontMetrics( currentFont() );
	
	for ( QTextBlock block = document()->begin(); block.isValid(); block = block.next() ) {
		QTextLayout* layout = block.layout();
		const QRectF boundingRect = layout->boundingRect();
		QPointF position = layout->position();
		
		if ( position.y() +boundingRect.height() < contentsY )
			continue;
		if ( position.y() > pageBottom )
			break;
		
		const QString txt = block.text();
		const int len = txt.length();
		
		for ( int i=0; i<len; i++) {
			QPixmap *p1 = 0;
			
			if ( txt[i] == ' ' )
				p1 = &m_spacePixmap;
			else if ( txt[i] == '\t' )
				p1 = &m_tabPixmap;
			else 
				continue;
			
			// pixmaps are of size 8x8 pixels
			QTextCursor cursor = textCursor();
			cursor.setPosition( block.position() + i, QTextCursor::MoveAnchor);
			
			QRect r = cursorRect( cursor );
			int x = r.x() + 4;
			int y = r.y() + fm.height() / 2 - 5;
			p.drawPixmap( x, y, *p1 );
		}
	}
}

void TextEditor::slotCursorPositionChanged() {
    if ( m_currentLineColor.isValid() )
        viewport()->update();	
}

void TextEditor::paintEvent ( QPaintEvent * event ) {
    QPainter painter( viewport() );

    if ( global.m_config->config().editor.highlightCurrentLine && m_currentLineColor.isValid() )     {
        QRect r = cursorRect();
        r.setX( 0 );
        r.setWidth( viewport()->width() );
        painter.fillRect( r, QBrush( m_currentLineColor ) );
    }

	if( global.m_config->config().editor.showTabulationAndSpace )
		printWhiteSpaces( painter );
		
    QTextEdit::paintEvent( event );
}

