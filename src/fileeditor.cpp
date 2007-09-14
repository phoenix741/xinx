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
#include "fileeditor.h"
#include "private/p_fileeditor.h"
#include "globals.h"
#include "xmlhighlighter.h"
#include "jshighlighter.h"
#include "xmleditor.h"
#include "jseditor.h"
#include "xslproject.h"
#include "xinxconfig.h"
#include "texteditor.h"
#include "numberbar.h"
#include "filecontentitemmodel.h"

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

/* PrivateFileEditor */

PrivateFileEditor::PrivateFileEditor( FileEditor * parent ) : m_watcher( NULL ), m_isSaving( false ), m_parent( parent ) {
	m_commentAction = new QAction( tr("Comment"), m_parent );
	m_commentAction->setEnabled( false );
	connect( m_commentAction, SIGNAL(triggered()), this, SLOT(comment()) );
	connect( m_parent, SIGNAL(copyAvailable(bool)), m_commentAction, SLOT(setEnabled(bool)) );
	m_uncommentAction = new QAction( tr("Uncomment"), m_parent );
	m_uncommentAction->setEnabled( false );
	connect( m_uncommentAction, SIGNAL(triggered()), this, SLOT(uncomment()) );
	connect( m_parent, SIGNAL(copyAvailable(bool)), m_uncommentAction, SLOT(setEnabled(bool)) );
}

PrivateFileEditor::~PrivateFileEditor() {
	delete m_watcher;
}

void PrivateFileEditor::setWatcher( const QString & path ) {
	if( m_path != path ) {
		delete m_watcher;
		m_path = path;
		m_watcher = new FileWatcher( path );
		connect( m_watcher, SIGNAL(fileChanged()), this, SLOT(fileChanged()) );
	}
}

void PrivateFileEditor::activateWatcher() {
	if( ! m_watcher ) {
		m_watcher = new FileWatcher( m_path );
		connect( m_watcher, SIGNAL(fileChanged()), this, SLOT(fileChanged()) );
	} else
		m_watcher->activate();
}

void PrivateFileEditor::desactivateWatcher() {
	if( m_watcher )
		m_watcher->desactivate();
}

void PrivateFileEditor::fileChanged() {
	if( m_isSaving ) return;
	if( ! global.m_config->config().editor.popupWhenFileModified ) return ;

	m_watcher->desactivate();
	if( QFile( m_path ).exists() && QMessageBox::question( qApp->activeWindow(), tr("Reload page"), tr("The file %1 was modified. Reload the page ?").arg( QFileInfo( m_path ).fileName() ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
		m_parent->loadFile();
	else
		m_parent->setModified( true );
		
	if( ! QFile( m_path ).exists() ) {
		QMessageBox::warning( qApp->activeWindow(), tr("Reload page"), tr("The file %1 was removed.").arg( QFileInfo( m_path ).fileName() ) );
		m_parent->setModified( true );		
	}
	m_watcher->activate();
}

void PrivateFileEditor::comment() {
	m_parent->commentSelectedText();
}

void PrivateFileEditor::uncomment() {
	m_parent->commentSelectedText( true );
}

/* FileEditor */

Q_DECLARE_METATYPE( FileEditor );

FileEditor::FileEditor( TextEditor * textEditor, QWidget *parent ) : Editor( parent ) {
	d = new PrivateFileEditor( this );
	
	// Set filter event layout.
	m_view = textEditor;
	m_view->setFrameStyle( QFrame::NoFrame );
	m_view->setLineWrapMode(QTextEdit::NoWrap);
	m_view->installEventFilter( this );
	m_view->setContextMenuPolicy( Qt::NoContextMenu );
  
	// Setup the line number pane
	m_numbers = new NumberBar( this );
	m_numbers->setTextEdit( m_view );
 
	m_hbox = new QHBoxLayout;
	m_hbox->setSpacing( 0 );
	m_hbox->setMargin( 0 );
	m_hbox->addWidget( m_numbers );
	m_hbox->addWidget( m_view ); 

	m_messageBox = new QWidget( this );

	m_messageErreur = new QLabel( "erreur text" );
	QPushButton * btnClose = new QPushButton( tr("&Close"), this );

	QHBoxLayout * messageBoxLayout = new QHBoxLayout( m_messageBox );
	messageBoxLayout->addWidget( m_messageErreur );
	messageBoxLayout->addWidget( btnClose );

	connect( btnClose, SIGNAL(clicked()), m_messageBox, SLOT(hide()) );


 	m_vbox = new QVBoxLayout( this );
	m_vbox->setSpacing( 0 );
	m_vbox->setMargin( 0 );
	m_vbox->addLayout( m_hbox );
	m_vbox->addWidget( m_messageBox );
	
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)) );
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(selectionAvailable(bool)) );
	connect( m_view, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)) );
	connect( m_view, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)) );

	connect( m_view->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(modificationChanged(bool)) );
	
	connect( m_view, SIGNAL(modelUpdated(QAbstractItemModel*)), this, SIGNAL(modelUpdated(QAbstractItemModel*)) );

	m_messageBox->hide();
}

FileEditor::~FileEditor() {
	delete d;
}

void FileEditor::setMessage( QString message ) {
	m_messageErreur->setText( message );
	if( message.isEmpty() )
		m_messageBox->hide();
	else
		m_messageBox->show();
}


TextEditor * FileEditor::textEdit() const { 
	return static_cast<TextEditor*>( m_view );
}

void FileEditor::selectAll() {
	textEdit()->selectAll();
}

void FileEditor::duplicateCurrentLine() {
	QTextCursor cursor( m_view->textCursor() );
	cursor.beginEditBlock();
	QTextBlock currentBlock = cursor.block();
	cursor.movePosition( QTextCursor::StartOfLine );
	cursor.insertText( currentBlock.text() + "\n" );
	cursor.endEditBlock();
}

void FileEditor::moveLineUp() {
	QTextCursor cursor( m_view->textCursor() );
	cursor.beginEditBlock();
	QTextBlock currentBlock = cursor.block();
	QString text = currentBlock.text();
	QTextBlock precBlock = currentBlock.previous();

	cursor.select( QTextCursor::BlockUnderCursor );
	cursor.removeSelectedText();

	cursor.setPosition( precBlock.position() );
	cursor.insertText( text + "\n" );

	cursor.endEditBlock();
	m_view->moveCursor( QTextCursor::PreviousBlock );
}

void FileEditor::moveLineDown() {
	QTextCursor cursor( m_view->textCursor() );
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

void FileEditor::uploSelectedText( bool upper ) {
	QTextCursor cursor( m_view->textCursor() );
  
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

	m_view->setTextCursor( cursor );
}

void FileEditor::indent( bool unindent ) {
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

void FileEditor::autoIndent() {
	setMessage( tr("Can't indent this type of document") );
}

void FileEditor::commentSelectedText( bool uncomment ) {
	m_view->commentSelectedText( uncomment );
}

void FileEditor::complete() {
	m_view->complete();
}

bool FileEditor::eventFilter( QObject *obj, QEvent *event ) {
	if ( obj != m_view )
		return QFrame::eventFilter( obj, event );

	/*if ( event->type() == QEvent::ToolTip ) {
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>( event );

		QTextCursor cursor = m_view->cursorForPosition( helpEvent->pos() );
		cursor.movePosition( QTextCursor::StartOfWord, QTextCursor::MoveAnchor );
		cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );

		QString word = cursor.selectedText();
		emit mouseHover( word );
		emit mouseHover( helpEvent->pos(), word );

		//QToolTip::showText( helpEvent->globalPos(), word ); // For testing
	}*/
	if( event->type() == QEvent::ContextMenu ) {
		QContextMenuEvent * contextMenuEvent = static_cast<QContextMenuEvent*>( event );
		QMenu * menu = new QMenu( m_view );

		menu->addAction( d->m_commentAction );
		menu->addAction( d->m_uncommentAction );
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

QString FileEditor::getTitle() const {
	if( ! m_fileName.isEmpty() )
		return QFileInfo( getFileName() ).fileName();
	else
		return tr( "noname" );
}

bool FileEditor::hasName() const {
	return ! m_fileName.isEmpty();
}

const QString & FileEditor::getFileName() const {
	return m_fileName;
}

QString FileEditor::getSuffix() const { 
	if( m_fileName.isEmpty() )
		return "txt";
	else
		return QFileInfo( m_fileName ).suffix();
}


void FileEditor::setFileName( const QString & fileName ) {
	if( ! fileName.isEmpty() ) {
		m_fileName = fileName;
		d->setWatcher( m_fileName );
	}
}

void FileEditor::createBackup( const QString & filename ) {
	if( global.m_config->config().editor.createBackupFile ){
		if( QFile::exists( filename + ".bak" ) ) 
			QFile::remove( filename + ".bak" );
		QFile::copy( filename, filename + ".bak" );
	}
}

void FileEditor::desactivateWatcher() {
	d->desactivateWatcher();
}

void FileEditor::activateWatcher() {
	d->activateWatcher();
}

void FileEditor::setIsSaving( bool value ) {
	d->m_isSaving = value;
	if( value ) {
		d->desactivateWatcher();
	} else {
		d->activateWatcher();
	}
	qApp->processEvents();
}


void FileEditor::loadFile( const QString & fileName ){
	if( ! fileName.isEmpty() ) setFileName( fileName );

	QFile file( getFileName() );
	if ( ! file.open( QFile::ReadOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XINX" ), tr( "Cannot read file %1:\n%2." )
																.arg( getFileName() )
																.arg( file.errorString() ) );
		return;
	}


	QTextStream in( &file );
	QApplication::setOverrideCursor( Qt::WaitCursor );
	m_view->setPlainText( in.readAll() );
	updateModel();
	
	setModified( false );
	QApplication::restoreOverrideCursor();
}

bool FileEditor::saveFile( const QString & fileName ){
	setIsSaving( true );

	if( ( fileName == m_fileName ) || fileName.isEmpty() ) createBackup( m_fileName ); 
	if( ! fileName.isEmpty() ) setFileName( fileName );
	
	QFile file( getFileName() );
	if ( ! file.open( QFile::WriteOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XINX" ), tr( "Cannot write file %1:\n%2." )
																.arg( fileName )
																.arg( file.errorString() ) );
		d->activateWatcher();
		return false;
	}
	QApplication::setOverrideCursor(Qt::WaitCursor);
	
	if( global.m_config->config().editor.autoindentOnSaving )
		autoIndent();
	QTextStream out ( &file );
	out << m_view->toPlainText();
	file.flush();
	file.close();

	updateModel();


	setModified( false );
	emit modificationChanged( false );

	setIsSaving( false );
	QApplication::restoreOverrideCursor();

	return true;	
}

void FileEditor::serialize( QDataStream & stream, bool content ) {
	Editor::serialize( stream, content );
	stream << m_fileName;
	stream << (int)m_view->textCursor().position();
	stream << (int)m_view->document()->isModified();
	stream << (int)(content && m_view->document()->isModified());
	if( content && m_view->document()->isModified() ) {
		stream << m_view->toPlainText();
	}
}

void FileEditor::deserialize( QDataStream & stream ) {
	int position;
	int content, isModified;
	QString text;
	
	Editor::deserialize( stream );
	stream >> m_fileName;
	stream >> position;
	stream >> isModified;
	stream >> content;
	if( content ) {
		stream >> text;
		m_view->setPlainText( text );

		d->setWatcher( m_fileName );
		updateModel();
	} else {
		if( ! m_fileName.isEmpty() )
			loadFile( m_fileName );
	}
	m_view->document()->setModified( isModified );

	QTextCursor tc = m_view->textCursor();
	tc.setPosition( position );
	m_view->setTextCursor( tc );
}

QAbstractItemModel * FileEditor::model() {
	return m_view->model();
}

void FileEditor::updateModel() {
	try {
		m_view->updateModel();
		setMessage("");
	} catch( FileContentException e ) {
		setMessage( tr("%1 at %2").arg( e.getMessage() ).arg( e.getLine() ) );
	}
}

bool FileEditor::canCopy() {
	return ! textEdit()->textCursor().selection().isEmpty();
}

bool FileEditor::canPaste() {
	return textEdit()->canPaste();
}

bool FileEditor::canUndo() {
	return textEdit()->document()->isUndoAvailable();
}

bool FileEditor::canRedo() {
	return textEdit()->document()->isRedoAvailable();
}

bool FileEditor::isModified() {
	return textEdit()->document()->isModified();
}

void FileEditor::setModified( bool modified ) {
	textEdit()->document()->setModified( modified );
}

void FileEditor::undo() {
	textEdit()->undo();
}

void FileEditor::redo() {
	textEdit()->redo();
}

void FileEditor::cut() {
	textEdit()->cut();	
}

void FileEditor::copy() {
	textEdit()->copy();
}

void FileEditor::paste() {
	textEdit()->paste();
}

QAction * FileEditor::commentAction() {
	return d->m_commentAction;
}

QAction * FileEditor::uncommentAction() {
	return d->m_uncommentAction;
}
