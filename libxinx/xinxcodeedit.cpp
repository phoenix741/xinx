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

// Xinx header
#include "xinxcodeedit_p.h"
#include "xinxconfig.h"
#include "xinxlanguagefactory.h"
#include "xinxformatscheme.h"

// Qt header
#include <QHBoxLayout>
#include <QAction>
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include <QCompleter>
#include <QMouseEvent>
#include <QAbstractItemView>

// QCodeEdit header
#include <qpanellayout.h>
#include <qdocument.h>
#include <qdocumentsearch.h>
#include <qdocumentline.h>
#include <qlinemarkpanel.h>
#include <qlinenumberpanel.h>
#include <qfoldpanel.h>
#include <qlinechangepanel.h>
#include <qdocument_p.h>
#include <qlanguagedefinition.h>
#include <qlinemarksinfocenter.h>

/*!
 * Definition of the characters that can't be in a word.
 */
#define EOW				"~!@$#%^&*()+{}|\"<>?,/;'[]\\="
#define EOWREGEXP		"[^A-Za-z0-9_:\\-]"
#define EOWREGEXPDOT	"[^A-Za-z0-9_:\\-\\.]"


/* XinxCodeEdit */

XinxCodeEdit::XinxCodeEdit( QWidget * parent ) : QWidget( parent ), m_completer( 0 ) {
	m_editor = new QCodeEdit( false, this );
	setHighlighter( QString() );

	m_editor->editor()->document()->setLineEnding( QDocument::Unix );
	m_editor->editor()->setWindowTitle( "[*]" );
	m_editor->editor()->addInputBinding( this );
	m_editor->editor()->setInputBinding( this );

	QDocumentSearch::Options opt = QDocumentSearch::Silent | QDocumentSearch::HighlightAll;
	m_matchingText = new QDocumentSearch( m_editor->editor(), QString(), opt );

	m_editor->addPanel( new QLineMarkPanel, QCodeEdit::West, true );
	m_editor->addPanel( new QLineNumberPanel, QCodeEdit::West, true );
	m_editor->addPanel( new QFoldPanel, QCodeEdit::West, true );
	m_editor->addPanel( new QLineChangePanel, QCodeEdit::West, true );

	QHBoxLayout * layout = new QHBoxLayout;
	layout->addWidget( m_editor->editor() );
	layout->setSpacing( 0 );
	layout->setMargin( 0 );
	setLayout( layout );

	connect( XINXConfig::self(), SIGNAL( changed() ), this, SLOT( updateHighlighter() ) );
	connect( XINXConfig::self(), SIGNAL( changed() ), this, SLOT( updateFont() ) );
	connect( m_editor->editor(), SIGNAL( cursorPositionChanged() ), this, SLOT( refreshTextHighlighter()));
	connect( m_editor->editor(), SIGNAL( copyAvailable( bool ) ), this, SIGNAL( copyAvailable( bool ) ) );
	connect( m_editor->editor(), SIGNAL( redoAvailable( bool ) ), this, SIGNAL( redoAvailable( bool ) ) );
	connect( m_editor->editor(), SIGNAL( undoAvailable( bool ) ), this, SIGNAL( undoAvailable( bool ) ) );
	connect( m_editor->editor()->document(), SIGNAL(markChanged(QDocumentLineHandle*,int,bool)), this, SLOT(slotMarkChanged(QDocumentLineHandle*,int,bool)) );

	updateFont();
}

XinxCodeEdit::~XinxCodeEdit() {
	m_editor->editor()->setInputBinding( 0 );
	m_editor->editor()->removeInputBinding( this );
	delete m_editor;
}

int XinxCodeEdit::currentColumn() {
	return m_editor->editor()->cursor().columnNumber();
}

int XinxCodeEdit::currentRow() {
	return m_editor->editor()->cursor().lineNumber();
}

QDocument * XinxCodeEdit::document() const {
	return m_editor->editor()->document();
}

QEditor * XinxCodeEdit::editor() const {
	return m_editor->editor();
}

void XinxCodeEdit::slotMarkChanged( QDocumentLineHandle* line, int type, bool enabled ) {
	Q_UNUSED( type );

	emit bookmarkToggled( line->line(), enabled );
}

void XinxCodeEdit::clearBookmark() {
	int bid = QLineMarksInfoCenter::instance()->markTypeId( "bookmark" );
	int mark = document()->findNextMark( bid );
	while( mark != -1 ) {
		emit bookmarkToggled( mark + 1, false );
		document()->line( mark ).removeMark( bid );

		mark = document()->findNextMark( bid, mark + 1 );
	}
}

bool XinxCodeEdit::previousBookmark() {
	int bid = QLineMarksInfoCenter::instance()->markTypeId( "bookmark" );
	int mark = document()->findPreviousMark( bid, currentRow() - 1 );
	if( mark == -1 ) return false;
	gotoLine( mark + 1 );
	return true;
}

bool XinxCodeEdit::nextBookmark() {
	int bid = QLineMarksInfoCenter::instance()->markTypeId( "bookmark" );
	int mark = document()->findNextMark( bid, currentRow() + 1 );
	if( mark == -1 ) return false;
	gotoLine( mark + 1 );
	return true;
}

void XinxCodeEdit::setBookmark( int line, bool enabled ) {
	int bid = QLineMarksInfoCenter::instance()->markTypeId( "bookmark" );
	QDocumentLine documentLine = m_editor->editor()->document()->line( line - 1 );
	if( enabled )
		documentLine.addMark( bid );
	else
		documentLine.removeMark( bid );

	emit bookmarkToggled( line, enabled );
}

QList<int> XinxCodeEdit::listOfBookmark() {
	QList<int> bookmarks;

	int bid = QLineMarksInfoCenter::instance()->markTypeId( "bookmark" );
	int mark = document()->findNextMark( bid );
	while( mark != -1 ) {
		bookmarks.append( mark + 1 );

		mark = document()->findNextMark( bid, mark + 1 );
	}

	return bookmarks;
}

void XinxCodeEdit::setErrors( QList<int> errors ) {
	QList<int> bookmarks;

	int bid = QLineMarksInfoCenter::instance()->markTypeId( "error" );

	// Remove old mark
	int mark = document()->findNextMark( bid );
	while( mark != -1 ) {
		QDocumentLine line = m_editor->editor()->document()->line( mark );
		line.removeMark( bid );

		mark = document()->findNextMark( bid, mark + 1 );
	}

	// Activate new marks
	foreach( mark, errors ) {
		QDocumentLine line = m_editor->editor()->document()->line( mark );
		line.addMark( bid );
	}
}

bool XinxCodeEdit::find( const QString & exp, XinxCodeEdit::FindFlags options ) {
	QDocumentCursor c = find( exp, m_editor->editor()->cursor(), options );
	return ! c.isNull();
}

QDocumentCursor XinxCodeEdit::find ( const QString & subString, const QDocumentCursor & cursor, XinxCodeEdit::FindFlags options ) const {
	QDocumentSearch::Options opt = QDocumentSearch::Silent;
	if( options.testFlag( FindCaseSensitively ) ) opt |= QDocumentSearch::CaseSensitive;
	if( options.testFlag( FindWholeWords ) ) opt |= QDocumentSearch::WholeWords;
	QDocumentSearch searchEngine( m_editor->editor(), subString, opt );

	searchEngine.setCursor( cursor );
	searchEngine.next( options.testFlag( FindBackward ) );
	return searchEngine.cursor();
}

QDocumentCursor XinxCodeEdit::find ( const QRegExp & expr, const QDocumentCursor & cursor, XinxCodeEdit::FindFlags options ) const {
	QDocumentSearch::Options opt = QDocumentSearch::Silent | QDocumentSearch::RegExp | QDocumentSearch::CaseSensitive;
	if( options.testFlag( FindWholeWords ) ) opt |= QDocumentSearch::WholeWords;
	QDocumentSearch searchEngine( m_editor->editor(), expr.pattern(), opt );

	searchEngine.setCursor( cursor );
	searchEngine.next( options.testFlag( FindBackward ) );
	return searchEngine.cursor();
}

QString XinxCodeEdit::textUnderCursor( const QDocumentCursor & cursor, bool deleteWord, bool dot ) {
	Q_ASSERT( ! cursor.isNull() );
	QString expr = EOWREGEXPDOT;
	if( ! dot )
		expr = EOWREGEXP;

	QDocumentCursor before ( find ( QRegExp( expr ), cursor, XinxCodeEdit::FindBackward ).selectionEnd() );
	QDocumentCursor after ( find ( QRegExp( expr ), cursor ).selectionStart() );

	QDocumentCursor tc = cursor;

	if( ( ! before.isNull() ) && ( before.lineNumber() == tc.lineNumber() ) ) {
		tc.moveTo( before );
	} else
		tc.movePosition( 1, QDocumentCursor::StartOfBlock, QDocumentCursor::MoveAnchor ) ;

	if( ( ! after.isNull() ) && ( after.lineNumber() == tc.lineNumber() ) )
		tc.movePosition( after.position() - (before.position() == -1 ? 0 : before.position()), QDocumentCursor::Right, QDocumentCursor::KeepAnchor ) ;
	else
		tc.movePosition( 1, QDocumentCursor::EndOfBlock, QDocumentCursor::KeepAnchor ) ;

	QString selection = tc.selectedText().trimmed();

	if( ( ! tc.selectedText().trimmed().isEmpty() ) && deleteWord ) {
		tc.removeSelectedText();
		m_editor->editor()->setCursor( tc );
	}

	return selection;
}

void XinxCodeEdit::setTextCursor( const QDocumentCursor & cursor ) {
	m_editor->editor()->setCursor( cursor );
}

QDocumentCursor XinxCodeEdit::textCursor() const {
	return m_editor->editor()->cursor();
}

void XinxCodeEdit::moveCursor( QDocumentCursor::MoveOperation operation, QDocumentCursor::MoveMode mode ) {
	QDocumentCursor c = m_editor->editor()->cursor();
	c.movePosition( 1, operation, mode );
	m_editor->editor()->setCursor( c );
}

QString XinxCodeEdit::selection() const {
	if( m_editor->editor()->cursor().hasSelection() )
		return m_editor->editor()->cursor().selectedText();
	else
		return toPlainText();
}

QString XinxCodeEdit::toPlainText() const {
	return m_editor->editor()->text();
}

void XinxCodeEdit::setPlainText( const QString & text ) {
	m_editor->editor()->setText( text );
	m_editor->editor()->document()->setClean();
	m_editor->editor()->update();
}

void XinxCodeEdit::setSelection( QString text ) {
	if( m_editor->editor()->cursor().hasSelection() ) {
		m_editor->editor()->cursor().beginEditBlock();
		m_editor->editor()->cursor().removeSelectedText();
		m_editor->editor()->cursor().insertText( text );
		m_editor->editor()->cursor().endEditBlock();
	} else {
		m_editor->editor()->cursor().beginEditBlock();
		m_editor->editor()->selectAll();
		m_editor->editor()->cursor().removeSelectedText();
		m_editor->editor()->cursor().insertText( text );
		m_editor->editor()->cursor().endEditBlock();
	}
}

void XinxCodeEdit::setMatchingText( QString text ) {
	if( m_matchingTextString != text ) {
		QDocumentCursor cursor;
		cursor.setSilent( true );
		if( ! text.isEmpty() ) {
			m_matchingText->setCursor( cursor );
			m_matchingText->setSearchText( text );
			m_matchingText->next( false );
		} else {
			delete m_matchingText;
			QDocumentSearch::Options opt = QDocumentSearch::Silent | QDocumentSearch::HighlightAll;
			m_matchingText = new QDocumentSearch( m_editor->editor(), QString(), opt );
		}
		m_matchingTextString = text;
	}
}

void XinxCodeEdit::setCompleter( QCompleter * completer ) {
	if( completer != m_completer ) {
		if( m_completer ) m_completer->disconnect( this );
		completer->setWidget( m_editor->editor() );
		completer->setCompletionMode( QCompleter::PopupCompletion );
		completer->setCaseSensitivity( Qt::CaseInsensitive );
		completer->setCompletionRole( Qt::DisplayRole );
		connect( completer, SIGNAL(activated(const QModelIndex &)), this, SLOT(insertCompletion(const QModelIndex &)) );
		m_completer = completer;
	}
};

bool XinxCodeEdit::canPaste() {
	const QMimeData *d = QApplication::clipboard()->mimeData();
	return d && d->hasText();
}

void XinxCodeEdit::print( QPrinter * printer ) const {
	Q_UNUSED( printer );
	//! \todo implement the print method
}

void XinxCodeEdit::setModified( bool modified ) {
	if( ! modified )
		document()->setClean();
	else {
		//! \todo Find a solution to setModified(true)
		qDebug( "Call of setModified( true ) but not managed" );
	}
}

void XinxCodeEdit::setTabStopWidth( int width ) {
	m_editor->editor()->document()->setTabStop( width );
}

int XinxCodeEdit::tabStopWidth() const {
	return m_editor->editor()->document()->tabStop();
}

void XinxCodeEdit::setReadOnly( bool readonly ) {
	m_editor->editor()->setFlag( QEditor::ReadOnly, readonly );
}

bool XinxCodeEdit::isReadOnly() const {
	return m_editor->editor()->flag( QEditor::ReadOnly );
}

void XinxCodeEdit::setHighlighter( const QString & highlighter ) {
	setHighlighter( highlighter, XINXConfig::self() );
}

void XinxCodeEdit::setHighlighter( const QString & highlighter, XINXConfig * config ) {
	if( highlighter.isEmpty() ) {
		document()->setFormatScheme( config->languageFactory()->defaultFormatScheme() );
		config->languageFactory()->setLanguage( m_editor->editor(), "None" );
		return;
	}

	QFormatScheme * scheme = config->scheme( highlighter );
	if( ! scheme )
		scheme = config->languageFactory()->defaultFormatScheme();
	document()->setFormatScheme( scheme );

	config->languageFactory()->setLanguage( m_editor->editor(), highlighter );
}

void XinxCodeEdit::updateHighlighter() {
	document()->setFormatScheme( document()->formatScheme() );
}

void XinxCodeEdit::updateFont() {
	QFont font = XINXConfig::self()->config().editor.defaultFormat;
	QFontMetrics fm( font );
	QDocument::setFont( font );
	QDocument::setTabStop( XINXConfig::self()->config().editor.tabulationSize );

	if( XINXConfig::self()->config().editor.showTabulationAndSpace )
		QDocument::setShowSpaces( QDocument::ShowTrailing | QDocument::ShowLeading | QDocument::ShowTabs );
	else
		QDocument::setShowSpaces( QDocument::ShowNone );
}

void XinxCodeEdit::insertText( const QString & text ) {
	QString txt = text, left, right;
	QDocumentCursor cursor = textCursor();
	QDocumentLine previous = cursor.line().previous();
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

void XinxCodeEdit::insertCompletion( const QModelIndex& index ) {
	QDocumentCursor tc = textCursor();
	QCompleter * c = completer();

	QString completion = c->completionModel()->data( index ).toString(),
		prefix     = c->completionPrefix();

	textUnderCursor( tc, true );
	tc = textCursor();
	tc.insertText( completion );

	setTextCursor( tc );
}

void XinxCodeEdit::gotoLine( int line ) {
	QDocumentCursor cursor( m_editor->editor()->document(), line - 1 );
	m_editor->editor()->setCursor( cursor );
}

void XinxCodeEdit::duplicateLines() {
	QDocumentCursor cursor( textCursor() );
	QString text;
	if( cursor.hasSelection() )
		text = cursor.selectedText();
	else
		text = cursor.line().text();

	if( (!cursor.hasSelection()) && cursor.line() == cursor.selectionStart().line() && cursor.line() == cursor.selectionEnd().line() )
		text += "\n";


	QDocumentCursor selStart = cursor.selectionStart();
	cursor.beginEditBlock();
	cursor.moveTo( selStart );
	if( text.contains( '\n' ) )
		cursor.movePosition( 0, QDocumentCursor::StartOfLine );
	cursor.insertText( text );
	cursor.movePosition( text.length(), QDocumentCursor::Right, QDocumentCursor::KeepAnchor );
	cursor.endEditBlock();
	setTextCursor( cursor );
}

void XinxCodeEdit::moveLineUp() {
	QDocumentCursor cursor( textCursor() );

	int selectionLength = 0;
	QDocumentCursor selectionStart 		= cursor.selectionStart(),
					selectionEnd   		= cursor.selectionEnd();
	selectionStart.setAutoUpdated( false );
	selectionEnd.setAutoUpdated( false );

	if( selectionEnd.isValid() ) selectionLength = cursor.selectedText().length();

	if( selectionEnd.columnNumber() == 0 ) selectionEnd.movePosition( 1, QDocumentCursor::Left );
	if( selectionStart.lineNumber() == 0 ) return; // No move when we are at the first line

	cursor.beginEditBlock();

	QString text;
	QDocumentLine currentLine = selectionStart.line();
	cursor.moveTo( currentLine, 0 );
	do {
		text += currentLine.text() + "\n";
		cursor.movePosition( 1, QDocumentCursor::NextLine, QDocumentCursor::KeepAnchor );

		currentLine = currentLine.next();
	} while( currentLine.isValid() && ( currentLine <= selectionEnd.line() ) );

	cursor.removeSelectedText();

	cursor.movePosition( 1, QDocumentCursor::PreviousLine, QDocumentCursor::MoveAnchor );
	cursor.movePosition( 1, QDocumentCursor::StartOfLine, QDocumentCursor::MoveAnchor );
	cursor.insertText( text );

	cursor.moveTo( selectionStart.lineNumber() - 1, selectionStart.columnNumber() );
	if( selectionLength > 0 )
		cursor.movePosition( selectionLength, QDocumentCursor::Right, QDocumentCursor::KeepAnchor );

	cursor.endEditBlock();
	setTextCursor( cursor );
}

void XinxCodeEdit::moveLineDown() {
	QDocumentCursor cursor( textCursor() );

	int selectionLength = 0;
	QDocumentCursor selectionStart 		= cursor.selectionStart(),
			selectionEnd   		= cursor.selectionEnd();
	if( selectionEnd.isValid() ) selectionLength = cursor.selectedText().length();

	if( selectionEnd.columnNumber() == 0 ) selectionEnd.movePosition( 1, QDocumentCursor::Left );
	if( ( ( selectionEnd.lineNumber() + 1 ) == m_editor->editor()->document()->lines() ) || ( ( selectionStart.lineNumber() + 1 ) == m_editor->editor()->document()->lines() ) ) return; // No move at end of document

	cursor.beginEditBlock();

	QString text;
	QDocumentLine currentLine = selectionStart.line();
	cursor.moveTo( currentLine, 0 );
	do {
		text += currentLine.text() + "\n";
		cursor.movePosition( 1, QDocumentCursor::NextLine, QDocumentCursor::KeepAnchor );

		currentLine = currentLine.next();
	} while( currentLine.isValid() && ( currentLine <= selectionEnd.line() ) );

	cursor.removeSelectedText();

	if( ( cursor.lineNumber() + 1 ) < m_editor->editor()->document()->lines() ) {
		cursor.movePosition( 1, QDocumentCursor::NextLine, QDocumentCursor::MoveAnchor );
		cursor.movePosition( 1, QDocumentCursor::StartOfLine, QDocumentCursor::MoveAnchor );
	} else {
		cursor.movePosition( 1, QDocumentCursor::EndOfLine, QDocumentCursor::MoveAnchor );
		cursor.insertText( "\n" );
		text.chop( 1 );
	}
	cursor.insertText( text );

	cursor.moveTo( selectionStart.lineNumber() + 1, selectionStart.columnNumber() );
	if( selectionLength > 0 )
		cursor.movePosition( selectionLength, QDocumentCursor::Right, QDocumentCursor::KeepAnchor );

	cursor.endEditBlock();
	setTextCursor( cursor );
}

void XinxCodeEdit::uploSelectedText( bool upper ) {
	QDocumentCursor cursor( textCursor() );

	QDocumentCursor startPos = cursor.selectionStart();
	QDocumentCursor endPos   = cursor.selectionEnd();

	QString text = cursor.selectedText();

	if(upper)
		text = text.toUpper();
	else
		text = text.toLower();

	cursor.beginEditBlock();

	cursor.removeSelectedText();
	cursor.insertText( text );

	cursor.moveTo( startPos );
	cursor.movePosition( endPos.position() - startPos.position(), QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor );

	cursor.endEditBlock();

	setTextCursor( cursor );
}

void XinxCodeEdit::upperSelectedText() {
	uploSelectedText( true );
}

void XinxCodeEdit::lowerSelectedText() {
	uploSelectedText( false );
}

void XinxCodeEdit::indent( bool unindent ) {
	QDocumentCursor tc ( textCursor() );

	if( tc.selectedText().isEmpty() ) {
		tc.insertText( "\t" );
		setTextCursor( tc );
		return;
	}

	tc.beginEditBlock();

	QDocumentCursor selectionStart = tc.selectionStart();
	QDocumentCursor selectionEnd = tc.selectionEnd();
	tc.clearSelection();

	tc.moveTo( selectionEnd );
	tc.movePosition( 1, QDocumentCursor::EndOfLine, QDocumentCursor::MoveAnchor );
	QDocumentLine endLine = tc.line();
	if( endLine.position() == selectionEnd.position() )
		endLine = endLine.previous();

	tc.moveTo( selectionStart );
	tc.movePosition( 1, QDocumentCursor::StartOfLine, QDocumentCursor::MoveAnchor );
	QDocumentLine startLine = tc.line();

	QDocumentLine line = startLine;
	do {
		tc.moveTo( line, 1 );

		if( ! unindent )
			tc.insertText( "\t" );
		else
			if ( line.text().count() && (line.text().at(0) == '\t' || line.text().at(0) == ' ') )
				tc.deleteChar();

		line = line.next();
	} while( line.isValid() && (( line < endLine ) || ( line == endLine )) );

	tc.endEditBlock();
}

void XinxCodeEdit::commentSelectedText( bool uncomment ) {
	Q_UNUSED( uncomment );
	throw XinxException( tr("Can't comment this type of document") );
}

void XinxCodeEdit::refreshTextHighlighter() {
	if( XINXConfig::self()->config().editor.autoHighlight && (!textCursor().isNull()) )
		setMatchingText( textUnderCursor( textCursor(), false, false ) );
}

void XinxCodeEdit::callTextHighlighter() {
	setMatchingText( textUnderCursor( textCursor(), false, false ) );
}

QString XinxCodeEdit::id() const {
	return "xinx";
}

QString XinxCodeEdit::name() const {
	return "XINX Binding";
}

bool XinxCodeEdit::localKeyPressExecute( QKeyEvent * e ) {
	bool isShortcut = ( (e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space ); // CTRL+Space

	if ( ( e->key() == Qt::Key_Home ) && ( e->modifiers() == Qt::ShiftModifier || e->modifiers() == Qt::NoModifier ) ) {
		key_home( e->modifiers() == Qt::ShiftModifier );
		e->accept();
		return false;
	} /*else if( ( e->key() == Qt::Key_Enter ) || ( e->key() == Qt::Key_Return ) ) {
		key_enter();
		e->ignore();
	} */else if( ( e->key() == Qt::Key_Tab ) && ( ( e->modifiers() == Qt::NoModifier ) || ( e->modifiers() == Qt::ShiftModifier ) ) ) {
		// TODO: May be replace by an action of the texteditor...
		indent( e->modifiers() != Qt::NoModifier );
		e->accept();
		return false;
	} else if( isShortcut ) {
		QString snipet = textUnderCursor( textCursor(), true );
		emit needInsertSnipet( snipet );
		e->accept();
		return false;
	}

	return true;
}

void XinxCodeEdit::key_home( bool select ) {
	QDocumentCursor cursor = textCursor();
	int col = cursor.columnNumber();
	cursor.movePosition( 1, QDocumentCursor::StartOfLine, select ? QDocumentCursor::KeepAnchor : QDocumentCursor::MoveAnchor );
	QDocumentCursor cursorStart( cursor );
	QDocumentLine b = cursorStart.line();
	int i = 0;
	while ( ( i < b.text().size() ) && ( i >= 0 ) && ( b.text().at(i) == ' ' || b.text().at(i) == '\t' ) ) {
		cursorStart.movePosition( 1, QDocumentCursor::NextCharacter, select ? QDocumentCursor::KeepAnchor : QDocumentCursor::MoveAnchor );
		i++;
	}
	if( col == cursorStart.columnNumber() )
		setTextCursor( cursor );
	else
		setTextCursor( cursorStart );
}

void XinxCodeEdit::key_enter() {
	QDocumentCursor tc( textCursor() );
	QString indent = tc.line().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );
	tc.insertText( "\n" + indent );
	setTextCursor( tc );
}

bool XinxCodeEdit::keyPressEvent( QKeyEvent * e, QEditor * editor ) {
	Q_UNUSED( editor );
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
			return true; // let the completer do default behavior
		default:
			break;
		}
	}

	bool isShortcut = ( ( e->modifiers() & Qt::ControlModifier ) && e->key() == Qt::Key_E ); // CTRL+E
	bool callParent = false;
	if( !c || !isShortcut )
		callParent = localKeyPressExecute( e );

	if( callParent ) return false;

	postKeyPressEvent( e, editor );

	return true;
}

void XinxCodeEdit::postKeyPressEvent( QKeyEvent * e, QEditor * editor ) {
	QCompleter * c = completer();
	bool isShortcut = ( ( e->modifiers() & Qt::ControlModifier ) && e->key() == Qt::Key_E ); // CTRL+E

	processKeyPress( e );

	const bool ctrlOrShift = e->modifiers() & ( Qt::ControlModifier | Qt::ShiftModifier );
	if( !c || ( ctrlOrShift && e->text().isEmpty() ) )
		return ;

	static QString eow( EOW ); // end of word
	bool hasModifier = ( e->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ) );// && !ctrlOrShift;
	QString completionPrefix = textUnderCursor( textCursor() );

	if( !isShortcut && ( hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains( e->text().right(1) ) ) ) {
		c->popup()->hide();
		return ;
	}

	if( completionPrefix != c->completionPrefix() ) {
		c->setCompletionPrefix( completionPrefix );
		c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
	}

	int x, y, h, w;
	QPoint pos = m_editor->editor()->mapFromContents( textCursor().documentPosition() );
	m_editor->editor()->getPanelMargins( &x, &y, &h, &w );
	QRect cr( pos.x() + x, pos.y() + document()->fontMetrics().height(), 1, 1 );
	cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
	c->complete( cr ); // popup it up!
}

void XinxCodeEdit::postMousePressEvent( QMouseEvent *event, QEditor * editor ) {
	Q_UNUSED( editor );
	if( ( event->type() == QEvent::MouseButtonPress ) && ( dynamic_cast<QMouseEvent*>( event )->button() == Qt::LeftButton ) && ( event->modifiers() == Qt::ControlModifier ) ) {
		QMetaObject::invokeMethod( this, "searchWord", Qt::QueuedConnection, Q_ARG( QString, textUnderCursor( textCursor(), false ) ) );
	}
}
