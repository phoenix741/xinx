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
#include "editors/xinxcodeedit.h"
#include "editors/bookmarktexteditorinterface.h"
#include "actions/actioninterface.h"
#include "plugins/xinxpluginsloader.h"
#include "project/xinxproject.h"
#include "core/xinxconfig.h"
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

TextFileEditor::TextFileEditor( XinxCodeEdit * editor, QWidget *parent ) : AbstractEditor( parent ), m_view( editor ), m_eol( DEFAULT_EOL ) {
	initObjects();
}

TextFileEditor::~TextFileEditor() {

}

void TextFileEditor::initObjects() {
	// Set filter event layout.
	if( ! m_view )
		m_view = new XinxCodeEdit( this );
	else
		m_view->setParent( this );

	setContextMenuPolicy( Qt::DefaultContextMenu );
	m_view->setContextMenuPolicy( Qt::NoContextMenu );
	m_view->editor()->setContextMenuPolicy( Qt::NoContextMenu );
	//installEventFilter( this );
	//m_view->installEventFilter( this );

	m_bookmarkInterface = new BookmarkTextEditorInterface( this );
	m_bookmarkInterface->setTextEditor( m_view );

	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)) );
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(selectionAvailable(bool)) );
	connect( m_view, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)) );
	connect( m_view, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)) );

	connect( m_view->editor(), SIGNAL(contentModified(bool)), this, SLOT(setModified(bool)) );
	connect( m_view->document(), SIGNAL(contentsChanged()), this, SIGNAL(contentChanged()) );

	connect( m_view, SIGNAL( searchWord(QString) ), this, SLOT( searchWord(QString) ) );
}

void TextFileEditor::initLayout() {
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( m_view );

	hbox->setMargin( 0 );

	setLayout( hbox );
}

BookmarkEditorInterface * TextFileEditor::bookmarkInterface() {
	return m_bookmarkInterface;
}

BookmarkTextEditorInterface * TextFileEditor::bookmarkTextInterface() {
	return m_bookmarkInterface;
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

void TextFileEditor::loadFromFile( const QString & fileName ) {
	AbstractEditor::loadFromFile( fileName );
	textEdit()->setFilename( fileName );
}

void TextFileEditor::saveToFile( const QString & fileName ) {
	AbstractEditor::saveToFile( fileName );
	textEdit()->setFilename( fileName );
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

	QString textBuffer = text.readAll();
	if( m_eol == MacEndOfLine ) {
		textBuffer.replace( "\r", "\n" );
	}
	m_view->setPlainText( textBuffer );
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
	AbstractEditor::setModified( isModified );
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
	emit message( lastFileName(), tr("Can't indent this type of document"), WARNING_MESSAGE );
	return false;
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

void TextFileEditor::contextMenuEvent( QContextMenuEvent * contextMenuEvent ) {
	QMenu * menu = new QMenu( m_view );

	foreach( XinxPluginElement * e, XinxPluginsLoader::self()->plugins() ) {

		// Si le plugin est activé
		if( e->isActivated() && qobject_cast<IXinxPlugin*>( e->plugin() ) ) {
			XinxAction::MenuList menuList = qobject_cast<IXinxPlugin*>( e->plugin() )->actions();

			// Pour chaque menu
			foreach( const XinxAction::ActionList & aMenu, menuList ) {

				// Pour chaque élemént du menu
				foreach( XinxAction::MenuItem * item, aMenu ) {
					XinxAction::Action* xinxAction = dynamic_cast<XinxAction::Action*>( item );
					if( xinxAction && xinxAction->isInPopupMenu() ) {
						xinxAction->updateActionState();

						menu->addAction( xinxAction->action() );
					}
				}

				menu->addSeparator();
			}
		}
	}
	
	menu->addAction( undoAction() );
	menu->addAction( redoAction() );
	menu->addSeparator();
	menu->addAction( cutAction() );
	menu->addAction( copyAction() );
	menu->addAction( pasteAction() );

	menu->exec( contextMenuEvent->globalPos() );
	delete menu;
}

void TextFileEditor::searchWord( const QString & ) {
	emit message( lastFileName(), tr("Not implemented"), WARNING_MESSAGE );
}

void TextFileEditor::serialize( XinxProjectSessionEditor * data, bool content ) {
	AbstractEditor::serialize( data, content );

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

		AbstractEditor::deserialize( data );
	} else {
		AbstractEditor::deserialize( data );

		if( ! lastFileName().isEmpty() )
			loadFromFile( lastFileName() );
	}
	m_view->setFilename( lastFileName() );

	int bc = data->readProperty( "BookmarkCount" ).toInt();
	for( int i = 0 ; i < bc; i++ ) {
		m_bookmarkInterface->setBookmark( data->readProperty( QString( "Bookmark_%1" ).arg( i ) ).toInt(), true );
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
