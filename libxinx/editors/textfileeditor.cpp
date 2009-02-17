/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// xinx header
#include "editors/textfileeditor.h"
#include "core/xinxproject.h"
#include "core/xinxconfig.h"
#include "editors/xinxcodeedit.h"
#include "plugins/xinxpluginsloader.h"
#include "borderlayout.h"

// Qt header
#include <QScrollBar>
#include <QPainter>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QApplication>
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

#ifdef Q_WS_X11
#	define DEFAULT_EOL TextFileEditor::UnixEndOfLine
#else
#ifdef Q_WS_MAC
#	define DEFAULT_EOL TextFileEditor::MacEndOfLine
#else
#	define DEFAULT_EOL TextFileEditor::WindowsEndOfLine
#endif
#endif

TextFileEditor::TextFileEditor( XinxCodeEdit * editor, QWidget *parent ) : AbstractFileEditor( parent ), m_view( editor ), m_eol( DEFAULT_EOL ) {
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
		m_view = new XinxCodeEdit( this );
	else
		m_view->setParent( this );

	m_view->installEventFilter( this );
	m_view->editor()->setContextMenuPolicy( Qt::NoContextMenu );
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)) );
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(selectionAvailable(bool)) );
	connect( m_view, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)) );
	connect( m_view, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)) );

	connect( m_view->editor(), SIGNAL(contentModified(bool)), this, SLOT(setModified(bool)) );
	connect( m_view->document(), SIGNAL(contentsChanged()), this, SIGNAL(contentChanged()) );

	connect( m_view, SIGNAL( searchWord(QString) ), this, SLOT( searchWord(QString) ) );

	borderLayout()->addWidget( m_view, BorderLayout::Center );
}

TextFileEditor::~TextFileEditor() {

}

void TextFileEditor::slotBookmarkToggled( int line, bool enabled ) {
	Q_UNUSED( enabled );
	Q_UNUSED( line );

	if( enabled ) {
		gotoBookmarkAt( m_view->listOfBookmark().indexOf( line ) );
	}
	emit bookmarkModified( 0, bookmarkCount() );
}

QList<int> TextFileEditor::bookmarks() const {
	return m_view->listOfBookmark();
}

void TextFileEditor::setBookmark( int line, bool enabled ) {
	m_view->setBookmark( line, enabled );
}

void TextFileEditor::toogledBookmark() {
	m_view->setBookmark( m_view->currentRow(), !m_view->listOfBookmark().contains( m_view->currentRow() ) );
}

void TextFileEditor::gotoBookmarkAt( int i ) {
	m_view->gotoLine( m_view->listOfBookmark().at( i ) );
}

QString TextFileEditor::bookmarkAt( int i ) {
	QString description = tr( "In editor '%1' at line %2" );
	description = description.arg( getTitle() ).arg( m_view->listOfBookmark().at( i ) );
	return description;
}

int TextFileEditor::bookmarkCount() {
	return m_view->listOfBookmark().count();
}

bool TextFileEditor::previousBookmark() {
	return m_view->previousBookmark();
}

bool TextFileEditor::nextBookmark() {
	return m_view->nextBookmark();
}

void TextFileEditor::clearAllBookmark() {
	m_view->clearBookmark();
}

XinxCodeEdit * TextFileEditor::textEdit() const {
	return m_view;
}

QString TextFileEditor::defaultFileName() const {
	return tr( "noname.txt" );
}

void TextFileEditor::selectAll() {
	textEdit()->editor()->selectAll();
}

void TextFileEditor::loadFromDevice( QIODevice & d ) {
	// Get the EOL of the file.
	char c;
	while( d.getChar( &c ) ) {
		if( c == '\r' ) {
			m_eol = MacEndOfLine;
			if( d.getChar( &c ) && ( c == '\n' ) ) {
				m_eol = WindowsEndOfLine;
			}
			break;
		} else if( c == '\n' ) {
			m_eol = UnixEndOfLine;
			break;
		}
	}

	d.reset();

	QTextStream text( &d );
	text.setCodec( codec() );
	m_view->setPlainText( text.readAll() );
}

void TextFileEditor::saveToDevice( QIODevice & d ) {
	if( XINXConfig::self()->config().editor.autoindentOnSaving ) autoIndent();
	m_eol = DEFAULT_EOL; // Don't conserve the EOL
	d.setTextModeEnabled( true );
	QTextStream text( &d );
	text.setCodec( codec() ); // Use the real codec on save
	text << m_view->toPlainText();
}

void TextFileEditor::setModified( bool isModified ) {
	m_view->setModified( isModified );
	AbstractFileEditor::setModified( isModified );
}

QAbstractItemModel * TextFileEditor::model()  const {
	return 0;
}

void TextFileEditor::initSearch( SearchOptions & options ) {
	m_cursorStart = textEdit()->textCursor();
	m_cursorEnd   = QDocumentCursor();

	QDocumentCursor selectionStart = m_cursorStart.selectionStart(),
	    		selectionEnd   = m_cursorStart.selectionEnd();

	if( options.testFlag( SEARCH_FROM_START ) ) {
		if( ! options.testFlag( BACKWARD ) )
			m_cursorStart.movePosition( 1, QDocumentCursor::Start, QDocumentCursor::MoveAnchor );
		else
			m_cursorStart.movePosition( 1, QDocumentCursor::End, QDocumentCursor::MoveAnchor );
		options &= ~ SearchOptions( SEARCH_FROM_START );
	} else if( options.testFlag( ONLY_SELECTION ) && ! options.testFlag( BACKWARD ) ) {
		m_cursorStart.moveTo( selectionStart );
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.movePosition( selectionEnd.position() - selectionStart.position(), QDocumentCursor::Right, QDocumentCursor::MoveAnchor );
	} else if( options.testFlag( ONLY_SELECTION ) && options.testFlag( BACKWARD ) ) {
		m_cursorStart.moveTo( selectionEnd );
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.movePosition( selectionEnd.position() - selectionStart.position(), QDocumentCursor::Left, QDocumentCursor::MoveAnchor );
	} else if( options.testFlag( BACKWARD ) ) {
		m_cursorStart.moveTo( selectionStart );
	}

	textEdit()->setTextCursor( m_cursorStart );
}

bool TextFileEditor::find( const QString & text, SearchOptions options ) {
	QDocumentCursor finded, tc = m_cursorStart;

	if( options.testFlag( BACKWARD ) )
		tc.moveTo( tc.selectionStart() );
	else
		tc.moveTo( tc.selectionEnd() );

	XinxCodeEdit::FindFlags flags;
	if( options.testFlag( BACKWARD ) ) flags ^= XinxCodeEdit::FindBackward;
	if( options.testFlag( MATCH_CASE ) ) flags ^= XinxCodeEdit::FindCaseSensitively;
	if( options.testFlag( WHOLE_WORDS ) ) flags ^= XinxCodeEdit::FindWholeWords;

	if( options.testFlag( REGULAR_EXPRESSION ) ) {
		finded = textEdit()->find( QRegExp( text ), tc, flags );
	} else {
		finded = textEdit()->find( text, tc, flags );
	}

	if( ! finded.isNull() ) {
		if( options.testFlag( ONLY_SELECTION  ) ) {
			if( (! options.testFlag( BACKWARD )) && ( m_cursorEnd < finded ) ) {
				return false;
			} else if( options.testFlag( BACKWARD ) && ( finded < m_cursorEnd ) ) {
				return false;
			}
		}

		m_cursorStart = finded;
		textEdit()->setTextCursor( finded );
	}

	return !finded.isNull();
}

void TextFileEditor::replace( const QString & from, const QString & to, SearchOptions options ) {
	Q_ASSERT( ! m_cursorStart.isNull() );

	if( ! options.testFlag( REGULAR_EXPRESSION ) ) {
		m_cursorStart.removeSelectedText();
		m_cursorStart.insertText( to );
		return;
	}

	QRegExp	expression( from );
	expression.indexIn( m_cursorStart.selectedText() );
	QStringList list = expression.capturedTexts();
	QString result( to );
	int index = 1;

	QStringList::iterator it = list.begin();
	while (it != list.end()) {
		result = result.replace(QString("\\%1").arg(index), *it);
		it++; index++;
	}

	m_cursorStart.removeSelectedText();
	m_cursorStart.insertText( result );

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
	QDocumentCursor cursor = textEdit()->textCursor();

	QString completionPrefix = textEdit()->textUnderCursor(cursor);

	QCompleter * c = textEdit()->completer();

	if( c ) {
		if( completionPrefix != c->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}

		int x, y, h, w;
		QPoint pos = textEdit()->editor()->mapFromContents( textEdit()->textCursor().documentPosition() );
		textEdit()->editor()->getPanelMargins( &x, &y, &h, &w );
		QRect cr( pos.x() + x, pos.y() + textEdit()->document()->fontMetrics().height(), 1, 1 );

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

void TextFileEditor::serialize( XinxProjectSessionEditor * data, bool content ) {
	AbstractFileEditor::serialize( data, content );

	data->writeProperty( "Position", QVariant( m_view->textCursor().position() ) );

	if( content && isModified() ) {
		data->writeProperty( "Content", QVariant( m_view->toPlainText() ) );
	}

	int i = 0;
	foreach( int line, m_view->listOfBookmark() ) {
		data->writeProperty( QString( "Bookmark_%1" ).arg( i++ ), QVariant( line ) );
	}
	data->writeProperty( "BookmarkCount", QVariant( i ) );
}

void TextFileEditor::deserialize( XinxProjectSessionEditor * data ) {
	int position = 0;
	QString text;

	position  = data->readProperty( "Position" ) .toInt();
	text = data->readProperty( "Content" ).toString();

	if( ! text.isEmpty() ) {
		m_view->setPlainText( text );

		AbstractFileEditor::deserialize( data );
	} else {
		AbstractFileEditor::deserialize( data );

		if( ! lastFileName().isEmpty() )
			loadFromFile( lastFileName() );
	}

	int bc = data->readProperty( "BookmarkCount" ).toInt();
	for( int i = 0 ; i < bc; i++ ) {
		setBookmark( data->readProperty( QString( "Bookmark_%1" ).arg( i ) ).toInt(), true );
	}

	QDocumentCursor tc( textEdit()->editor()->document() );
	tc.movePosition( position, QDocumentCursor::Right );
	m_view->setTextCursor( tc );
}

bool TextFileEditor::canCopy() {
	return textEdit()->textCursor().hasSelection();
}

bool TextFileEditor::canPaste() {
	return textEdit()->canPaste();
}

bool TextFileEditor::canUndo() {
	return textEdit()->editor()->canUndo();
}

bool TextFileEditor::canRedo() {
	return textEdit()->editor()->canRedo();
}

void TextFileEditor::undo() {
	textEdit()->editor()->undo();
}

void TextFileEditor::redo() {
	textEdit()->editor()->redo();
}

void TextFileEditor::cut() {
	textEdit()->editor()->cut();
}

void TextFileEditor::copy() {
	textEdit()->editor()->copy();
}

void TextFileEditor::paste() {
	textEdit()->editor()->paste();
}

QAction * TextFileEditor::commentAction() {
	return m_commentAction;
}

QAction * TextFileEditor::uncommentAction() {
	return m_uncommentAction;
}

