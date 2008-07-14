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
#include "xinxconfig.h"
#include "exceptions.h"
#include "numberbar.h"
#include "syntaxhighlighter.h"

// Qt header
#include <QTextBlock>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QScrollBar>
#include <QTextLayout>
#include <QPainter>
#include <QPixmap>
#include <QCompleter>
#include <QAbstractItemView>

/*!
 * Definition of the characters that can't be in a word.
 */
#define EOW				"~!@$#%^&*()+{}|\"<>?,/;'[]\\="
#define EOWREGEXP		"[^A-Za-z0-9_:\\-]"
#define EOWREGEXPDOT	"[^A-Za-z0-9_:\\-\\.]"

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

TextEditor::TextEditor( QWidget * parent ) : QTextEdit( parent ), m_syntaxhighlighter( 0 ), m_completer( 0 ) {
	// Setup the main view

    m_tabPixmap = QPixmap( tabPixmap_img );
	m_spacePixmap = QPixmap( spacePixmap_img );
	m_currentLineColor = QColor( 234, 234, 234 );

	connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( slotCursorPositionChanged()));
	connect( XINXConfig::self(), SIGNAL( changed() ), this, SLOT( updateHighlighter() ) );
	connect( XINXConfig::self(), SIGNAL( changed() ), this, SLOT( updateFont() ) );

	setAcceptRichText(false);
	//setAcceptDrops(false);
	updateFont();
}

TextEditor::~TextEditor() {
}

QString TextEditor::selection() const {
	if( textCursor().hasSelection() )
		return textCursor().selectedText();
	else
		return toPlainText();
}

void TextEditor::setSelection( QString text ) {
	if( textCursor().hasSelection() )
		return textCursor().insertText( text );
	else
		return setPlainText( text );
}

void TextEditor::setCompleter( QCompleter * completer ) {
	if( completer != m_completer ) {
		if( m_completer ) m_completer->disconnect( this );
		completer->setWidget( this );
		completer->setCompletionMode( QCompleter::PopupCompletion );
		completer->setCaseSensitivity( Qt::CaseInsensitive );
		completer->setCompletionRole( Qt::DisplayRole );
		connect( completer, SIGNAL(activated(const QModelIndex &)), this, SLOT(insertCompletion(const QModelIndex &)) );
		m_completer = completer;
	}
};


void TextEditor::updateFont() {
	QFont font = XINXConfig::self()->config().editor.defaultFormat;
	QFontMetrics fm( font );
	setFont( font );
	setTabStopWidth( fm.width("M") * XINXConfig::self()->config().editor.tabulationSize );
}

int TextEditor::currentColumn() {
	QTextCursor tc( textCursor() );
	return tc.columnNumber() + 1;
}

int TextEditor::currentRow() {
	QTextCursor tc( textCursor() );
	return tc.blockNumber() + 1;
}

void TextEditor::commentSelectedText( bool /* uncomment */ ) {
	throw XinxException( tr("Can't comment this type of document") );
}

void TextEditor::localKeyPressExecute( QKeyEvent * e ) {
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
}

void TextEditor::keyPressEvent( QKeyEvent * e ) {
	QCompleter * c = completer();

	if( c && c->popup()->isVisible() ) {
		// The following keys are forwarded by the completer to the widget
		switch( e->key() ) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
		default:
			break;
		}
	}

	bool isShortcut = ( ( e->modifiers() & Qt::ControlModifier ) && e->key() == Qt::Key_E ); // CTRL+E
	if( !c || !isShortcut )
		localKeyPressExecute( e );

	processKeyPress( e );

	const bool ctrlOrShift = e->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier );
	if( !c || ( ctrlOrShift && e->text().isEmpty() ) )
		return;

	static QString eow( EOW ); // end of word
	bool hasModifier = ( e->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ) );// && !ctrlOrShift;
	QString completionPrefix = textUnderCursor( textCursor() );

	if( !isShortcut && ( hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains( e->text().right(1) ) ) ) {
		c->popup()->hide();
		return;
	}

	if( completionPrefix != c->completionPrefix() ) {
		c->setCompletionPrefix( completionPrefix );
		c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
	}

	QRect cr = cursorRect();
	cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
	c->complete( cr ); // popup it up!*/
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

void TextEditor::setHighlighter( SyntaxHighlighter * highlighter ) {
	if( m_syntaxhighlighter != highlighter ) {
		m_syntaxhighlighter = highlighter;
	}
}

void TextEditor::updateHighlighter() {
	if( m_syntaxhighlighter )
		m_syntaxhighlighter->rehighlight();
}

void TextEditor::refreshTextHighlighter() {
	if( XINXConfig::self()->config().editor.autoHighlight )
		setMatchingText( textUnderCursor( textCursor(), false, false ) );
}

void TextEditor::callTextHighlighter() {
	setMatchingText( textUnderCursor( textCursor(), false, false ) );
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

void TextEditor::mousePressEvent ( QMouseEvent * event ) {
	if( ( event->type() == QEvent::MouseButtonPress ) && ( dynamic_cast<QMouseEvent*>( event )->button() == Qt::LeftButton ) && ( event->modifiers() == Qt::ControlModifier ) ) {
		QTextEdit::mousePressEvent( event );
		emit searchWord( textUnderCursor( textCursor(), false ) );
	} else
		QTextEdit::mousePressEvent( event );
}

QString TextEditor::textUnderCursor( const QTextCursor & cursor, bool deleteWord, bool dot ) {
	Q_ASSERT( ! cursor.isNull() );
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

void TextEditor::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = textCursor();
	QCompleter * c = completer();

	QString completion = c->completionModel()->data( index ).toString(),
			prefix     = c->completionPrefix();

	textUnderCursor( tc, true );
	tc.insertText( completion );


	setTextCursor( tc );
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

void TextEditor::setMatchingText( QString text ) {
	if( text != m_matchedText ) {
		m_matchedText = text;
		viewport()->update();
	}
}

void TextEditor::printWhiteSpaces( QPainter &p ) {
	const int contentsY = verticalScrollBar()->value();
	const qreal pageBottom = contentsY + viewport()->height();

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
			QTextCursor cursor = textCursor();
			cursor.setPosition( block.position() + i, QTextCursor::MoveAnchor );
			const QFontMetrics fm = QFontMetrics( cursor.blockCharFormat().font() );
			QRect r = cursorRect( cursor );

			/* Highlight text */
			if( (!m_matchedText.isEmpty() ) && ( txt[i] == m_matchedText[0] ) && ( txt.mid( i, m_matchedText.length() ) == m_matchedText ) ){
				QRect highlightZone = r;
				highlightZone.setLeft( highlightZone.left() + highlightZone.width() / 2 );
				highlightZone.setWidth( fm.width( m_matchedText ) );
				p.fillRect( highlightZone, XINXConfig::self()->config().editor.highlightWord );
			}

			if( XINXConfig::self()->config().editor.showTabulationAndSpace ) {
				QPixmap *p1 = 0;

				if ( txt[i] == ' ' )
					p1 = &m_spacePixmap;
				else if ( txt[i] == '\t' )
					p1 = &m_tabPixmap;
				else
					continue;

				// pixmaps are of size 8x8 pixels

				int x = r.x() + 4;
				int y = r.y() + fm.height() / 2 - 5;
				p.drawPixmap( x, y, *p1 );
			}
		}
	}
}

void TextEditor::slotCursorPositionChanged() {
    if ( m_currentLineColor.isValid() )
        viewport()->update();
	refreshTextHighlighter();
}

void TextEditor::paintEvent ( QPaintEvent * event ) {
    QPainter painter( viewport() );

    if ( XINXConfig::self()->config().editor.highlightCurrentLine && m_currentLineColor.isValid() )     {
        QRect r = cursorRect();
        r.setX( 0 );
        r.setWidth( viewport()->width() );
        painter.fillRect( r, QBrush( m_currentLineColor ) );
    }

	if( XINXConfig::self()->config().editor.showTabulationAndSpace || (! m_matchedText.isEmpty() ) )
		printWhiteSpaces( painter );

	painter.end();
    QTextEdit::paintEvent( event );
}

void TextEditor::gotoLine( int line ) {
	if( line == currentRow() ) return;

	QTextCursor cursor = textCursor();
	cursor.movePosition( QTextCursor::Start );
	cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, line - 1 );

	setTextCursor( cursor );
}

void TextEditor::duplicateLines() {
	QTextCursor cursor( textCursor() );

	cursor.beginEditBlock();
	QTextBlock currentBlock = cursor.block();
	cursor.movePosition( QTextCursor::StartOfLine );
	cursor.insertText( currentBlock.text() + "\n" );
	cursor.endEditBlock();
}

void TextEditor::moveLineUp() {
	QTextCursor cursor( textCursor() );
	cursor.beginEditBlock();
	QTextBlock currentBlock = cursor.block();
	QString text = currentBlock.text();
	QTextBlock precBlock = currentBlock.previous();

	cursor.select( QTextCursor::BlockUnderCursor );
	cursor.removeSelectedText();

	cursor.setPosition( precBlock.position() );
	cursor.insertText( text + "\n" );

	cursor.endEditBlock();
	moveCursor( QTextCursor::PreviousBlock );
}

void TextEditor::moveLineDown() {
	QTextCursor cursor( textCursor() );
	cursor.beginEditBlock();
	cursor.movePosition( QTextCursor::NextBlock );

	QTextBlock currentBlock = cursor.block();
	QString text = currentBlock.text();
	QTextBlock precBlock = currentBlock.previous();

	cursor.select( QTextCursor::BlockUnderCursor );
	cursor.removeSelectedText();

	cursor.setPosition( precBlock.position() );
	cursor.insertText( text + "\n" );

	cursor.endEditBlock();
}

void TextEditor::upperSelectedText() {
	uploSelectedText( true );
}

void TextEditor::lowerSelectedText() {
	uploSelectedText( false );
}

void TextEditor::uploSelectedText( bool upper ) {
	QTextCursor cursor( textCursor() );

	int startPos = cursor.selectionStart();
	int endPos   = cursor.selectionEnd();
	QString text = cursor.selectedText();

	if(upper)
		text = text.toUpper();
	else
		text = text.toLower();

	cursor.beginEditBlock();

	cursor.insertText( text );

	cursor.setPosition( startPos, QTextCursor::MoveAnchor );
	cursor.setPosition( endPos, QTextCursor::KeepAnchor );

	cursor.endEditBlock();

	setTextCursor( cursor );
}

