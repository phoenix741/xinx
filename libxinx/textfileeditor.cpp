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

// xinx header
#include "textfileeditor.h"
#include "syntaxhighlighter.h"
#include "xslproject.h"
#include "xinxconfig.h"
#include "texteditor.h"
#include "numberbar.h"
#include "xinxpluginsloader.h"
#include "borderlayout.h"

// Qt header
#include <QTextEdit>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QPainter>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
#include <QTextDocumentFragment>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QCompleter>
#include <QAbstractItemView>
#include <QTextCodec>

/* TextFileEditor */
//Q_DECLARE_METATYPE( TextFileEditor );

#ifdef Q_WS_X11
#	define DEFAULT_EOL TextFileEditor::UnixEndOfLine
#else
#ifdef Q_WS_MAC
#	define DEFAULT_EOL TextFileEditor::MacEndOfLine
#else
#	define DEFAULT_EOL TextFileEditor::WindowsEndOfLine
#endif
#endif

TextFileEditor::TextFileEditor( TextEditor * editor, QWidget *parent ) : AbstractFileEditor( parent ), m_view( editor ), m_eol( DEFAULT_EOL ) {
	m_commentAction = new QAction( tr("Comment"), this );
	m_commentAction->setEnabled( false );
	connect( m_commentAction, SIGNAL(triggered()), this, SLOT(comment()) );
	connect( this, SIGNAL(copyAvailable(bool)), m_commentAction, SLOT(setEnabled(bool)) );

	m_uncommentAction = new QAction( tr("Uncomment"), this );
	m_uncommentAction->setEnabled( false );
	connect( m_uncommentAction, SIGNAL(triggered()), this, SLOT(uncomment()) );
	connect( this, SIGNAL(copyAvailable(bool)), m_uncommentAction, SLOT(setEnabled(bool)) );

	// Set filter event layout.
	if( ! m_view )
		m_view = new TextEditor( this );
	else
		m_view->setParent( this );

	m_view->setFrameStyle( QFrame::NoFrame );
	m_view->setLineWrapMode(QTextEdit::NoWrap);
	m_view->installEventFilter( this );
	m_view->setContextMenuPolicy( Qt::NoContextMenu );
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)) );
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(selectionAvailable(bool)) );
	connect( m_view, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)) );
	connect( m_view, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)) );

	connect( m_view->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setModified(bool)) );
	connect( m_view, SIGNAL(textChanged()), this, SIGNAL(contentChanged()) );

	connect( m_view, SIGNAL( searchWord(QString) ), this, SLOT( searchWord(QString) ) );

	// Setup the line number pane
	m_numbers = new NumberBar( this );
	m_numbers->setTextEdit( m_view );

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->setSpacing( 0 );
	hbox->setMargin( 0 );
	hbox->addWidget( m_numbers );
	hbox->addWidget( m_view );

	borderLayout()->add( hbox, BorderLayout::Center );
}

TextFileEditor::~TextFileEditor() {

}

void TextFileEditor::slotBookmarkToggled( int line, bool enabled ) {
	Q_UNUSED( enabled );
	Q_UNUSED( line );

	if( enabled ) {
		gotoBookmarkAt( m_numbers->listOfBookmark().indexOf( line ) );
	}
	emit bookmarkModified( 0, bookmarkCount() );
}

QList<int> & TextFileEditor::bookmarks() const {
	return m_numbers->listOfBookmark();
}

void TextFileEditor::setBookmark( int line, bool enabled ) {
	m_numbers->setBookmark( line, enabled );
}

void TextFileEditor::toogledBookmark() {
	m_numbers->setBookmark( m_view->currentRow(), !m_numbers->listOfBookmark().contains( m_view->currentRow() ) );
}

void TextFileEditor::gotoBookmarkAt( int i ) {
	AbstractFileEditor::gotoBookmarkAt( i );
	m_view->gotoLine( m_numbers->listOfBookmark().at( i ) );
}

QString TextFileEditor::bookmarkAt( int i ) {
	QString description = tr( "In editor '%1' at line %2" );
	description = description
					.arg( getTitle() )
					.arg( m_numbers->listOfBookmark().at( i ) );
	return description;
}

int TextFileEditor::bookmarkCount() {
	return m_numbers->listOfBookmark().count();
}

bool TextFileEditor::previousBookmark() {
	int line = m_view->currentRow();
	for ( QList<int>::iterator i = m_numbers->listOfBookmark().end() - 1 ; i != m_numbers->listOfBookmark().begin() - 1; i-- ) {
		if( line > *i ) {
			int bookmark = i - m_numbers->listOfBookmark().begin();
			gotoBookmarkAt( bookmark );
			return true;
		}
	}
	return false;
}

bool TextFileEditor::nextBookmark() {
	int line = m_view->currentRow();
	for( QList<int>::iterator i = m_numbers->listOfBookmark().begin(); i != m_numbers->listOfBookmark().end() ; i++ ) {
		if( line < *i ) {
			int bookmark = i - m_numbers->listOfBookmark().begin();
			gotoBookmarkAt( bookmark );
			return true;
		}
	}
	return false;
}

void TextFileEditor::clearAllBookmark() {
	m_numbers->listOfBookmark().clear();
}

TextEditor * TextFileEditor::textEdit() const {
	return m_view;
}

QString TextFileEditor::defaultFileName() const {
	return QString( "noname.txt" );
}

void TextFileEditor::selectAll() {
	textEdit()->selectAll();
}

void TextFileEditor::indent( bool unindent ) {
	QTextCursor tc ( m_view->textCursor() );

	if( tc.selectedText().isEmpty() ) {
		tc.insertText( "\t" );
		m_view->setTextCursor( tc );
		return;
	}

    tc.beginEditBlock();

    int startPos = tc.selectionStart();
    int endPos = tc.selectionEnd();
    if( startPos > endPos ) {
    	int tmp = endPos;
    	endPos = startPos;
    	startPos = tmp;
   	}
    tc.clearSelection();

    tc.setPosition( endPos, QTextCursor::MoveAnchor );
    tc.movePosition( QTextCursor::EndOfLine, QTextCursor::MoveAnchor );
    QTextBlock endBlock = tc.block();
    if( endBlock.position() == endPos )
    	endBlock = endBlock.previous();

    tc.setPosition( startPos, QTextCursor::MoveAnchor );
    tc.movePosition( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
    QTextBlock startBlock = tc.block();

    QTextBlock block = startBlock;
    do {
		tc.setPosition( block.position() );

	    if( ! unindent )
		    tc.insertText( "\t" );
		else
          if ( block.text().count() && (block.text().at(0) == '\t' || block.text().at(0) == ' ') )
			tc.deleteChar();

    	block = block.next();
   	} while( block.isValid() && (( block < endBlock ) || ( block == endBlock )) );

	tc.endEditBlock();
}

void TextFileEditor::loadFromDevice( QIODevice & d ) {
	// Get the EOL of the file.
	char c;
	d.setTextModeEnabled( true );
	d.readLine(); // Load the firstLine
	d.seek( d.pos() - 1 ); d.getChar( &c ); // Read previous char
	if( c == '\r' ) {
		m_eol = MacEndOfLine;
	} else {
		Q_ASSERT( c == '\n' );
		d.seek( d.pos() - 2 ); d.getChar( &c ); // Read previous char
		if( c == '\r' )
			m_eol = WindowsEndOfLine;
		else
			m_eol = UnixEndOfLine;
	}
	d.reset();
	d.setTextModeEnabled( false );

	QTextStream text( &d );
	text.setCodec( codec() );
	m_view->setPlainText( text.readAll() );
}

void TextFileEditor::saveToDevice( QIODevice & d ) {
	if( XINXConfig::self()->config().editor.autoindentOnSaving ) autoIndent();
	m_eol = DEFAULT_EOL; // Don't conserve the EOL
	QTextStream text( &d );
	text.setCodec( codec() ); // Use the real codec on save
	text << m_view->toPlainText();
}

void TextFileEditor::setModified( bool isModified ) {
	m_view->document()->setModified( isModified );
	AbstractFileEditor::setModified( isModified );
}

QAbstractItemModel * TextFileEditor::model()  const {
	return 0;
}

void TextFileEditor::updateModel() {
	// Do nothing
}

QTextCodec * TextFileEditor::codec() const {
	QTextCodec * c = QTextCodec::codecForName( XINXConfig::self()->config().editor.defaultTextCodec.toAscii() );
	if( c ) return c;
	return QTextCodec::codecForLocale();
}

TextFileEditor::EndOfLineType TextFileEditor::eol() const {
	return m_eol;
}

bool TextFileEditor::autoIndent() {
	setMessage( tr("Can't indent this type of document") );
	return false;
}

void TextFileEditor::commentSelectedText( bool uncomment ) {
	try {
		m_view->commentSelectedText( uncomment );
	} catch( XinxException e ) {
		setMessage( e.getMessage() );
	}
}

void TextFileEditor::complete() {
	QTextCursor cursor = textEdit()->textCursor();

	QRect cr = textEdit()->cursorRect();
	QString completionPrefix = textEdit()->textUnderCursor(cursor);

	QCompleter * c = textEdit()->completer();

	if( c ) {
		if( completionPrefix != c->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}

bool TextFileEditor::eventFilter( QObject *obj, QEvent *event ) {
	if ( obj != m_view )
		return QFrame::eventFilter( obj, event );

	if( event->type() == QEvent::ContextMenu ) {
		QContextMenuEvent * contextMenuEvent = static_cast<QContextMenuEvent*>( event );
		QMenu * menu = new QMenu( m_view );

		menu->addAction( m_commentAction );
		menu->addAction( m_uncommentAction );
		menu->addSeparator();
		menu->addAction( undoAction() );
		menu->addAction( redoAction() );
		menu->addSeparator();
		menu->addAction( cutAction() );
		menu->addAction( copyAction() );
		menu->addAction( pasteAction() );

		menu->exec( contextMenuEvent->globalPos() );
		delete menu;
	}

	return false;
}

void TextFileEditor::searchWord( const QString & ) {
	setMessage( tr("Not implemented") );
}

void TextFileEditor::comment() {
	commentSelectedText();
}

void TextFileEditor::uncomment() {
	commentSelectedText( true );
}

NumberBar * TextFileEditor::numbersBar() const {
	return m_numbers;
}

void TextFileEditor::serialize( XSLProjectSessionEditor * data, bool content ) {
	AbstractFileEditor::serialize( data, content );

	data->writeProperty( "Position", QVariant( m_view->textCursor().position() ) );

	if( content && isModified() ) {
		data->writeProperty( "Content", QVariant( m_view->toPlainText() ) );
	}

	int i = 0;
	foreach( int line, m_numbers->listOfBookmark() ) {
		data->writeProperty( QString( "Bookmark_%1" ).arg( i++ ), QVariant( line ) );
	}
	data->writeProperty( "BookmarkCount", QVariant( i ) );
}

void TextFileEditor::deserialize( XSLProjectSessionEditor * data ) {
	int position = 0;
	QString text;

	position  = data->readProperty( "Position" ) .toInt();
	text = data->readProperty( "Content" ).toString();

	int bc = data->readProperty( "BookmarkCount" ).toInt();
	for( int i = 0 ; i < bc; i++ ) {
		setBookmark( data->readProperty( QString( "Bookmark_%1" ).arg( i ) ).toInt(), true );
	}

	if( ! text.isEmpty() ) {
		m_view->setPlainText( text );

		AbstractFileEditor::deserialize( data );
	} else {
		AbstractFileEditor::deserialize( data );

		if( ! lastFileName().isEmpty() )
			loadFromFile( lastFileName() );
	}

	QTextCursor tc = m_view->textCursor();
	tc.setPosition( position );
	m_view->setTextCursor( tc );
}

bool TextFileEditor::canCopy() {
	return ! textEdit()->textCursor().selection().isEmpty();
}

bool TextFileEditor::canPaste() {
	return textEdit()->canPaste();
}

bool TextFileEditor::canUndo() {
	return textEdit()->document()->isUndoAvailable();
}

bool TextFileEditor::canRedo() {
	return textEdit()->document()->isRedoAvailable();
}

void TextFileEditor::undo() {
	textEdit()->undo();
}

void TextFileEditor::redo() {
	textEdit()->redo();
}

void TextFileEditor::cut() {
	textEdit()->cut();
}

void TextFileEditor::copy() {
	textEdit()->copy();
}

void TextFileEditor::paste() {
	textEdit()->paste();
}

QAction * TextFileEditor::commentAction() {
	return m_commentAction;
}

QAction * TextFileEditor::uncommentAction() {
	return m_uncommentAction;
}

