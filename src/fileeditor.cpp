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
#include "globals.h"
#include "syntaxhighlighter.h"
#include "xslproject.h"
#include "xinxconfig.h"
#include "texteditor.h"
#include "numberbar.h"
#include "filecontentitemmodel.h"
#include "xinxpluginsloader.h"
#include "filewatcher.h"

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

/* Define */

#define EOW			"~!@$#%^&*()+{}|\"<>?,/;'[]\\="

/* FileEditor */

Q_DECLARE_METATYPE( FileEditor );

FileEditor::FileEditor( QWidget *parent, const QString & suffix ) : Editor( parent ), m_syntaxhighlighter( NULL ), m_watcher( NULL ), 
	m_prettyPrinterPlugin( qMakePair( (IPluginPrettyPrint*)NULL, QString() ) ), m_extendedEditorPlugin( qMakePair( (IPluginExtendedEditor*)NULL, QString() ) ), m_isSaving( false ), 
	m_object( NULL ), m_element( NULL ), m_model( NULL ) {

	m_commentAction = new QAction( tr("Comment"), this );
	m_commentAction->setEnabled( false );
	connect( m_commentAction, SIGNAL(triggered()), this, SLOT(comment()) );
	connect( this, SIGNAL(copyAvailable(bool)), m_commentAction, SLOT(setEnabled(bool)) );
	
	m_uncommentAction = new QAction( tr("Uncomment"), this );
	m_uncommentAction->setEnabled( false );
	connect( m_uncommentAction, SIGNAL(triggered()), this, SLOT(uncomment()) );
	connect( this, SIGNAL(copyAvailable(bool)), m_uncommentAction, SLOT(setEnabled(bool)) );

	// Set filter event layout.
	m_view = new TextEditor( this );
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
    connect( &global, SIGNAL( configChanged() ), this, SLOT( updateHighlighter() ) );
    
    connect( m_view, SIGNAL( cursorPositionChanged() ), this, SLOT( refreshTextHighlighter() ));
	connect( m_view, SIGNAL( execKeyPressEvent(QKeyEvent*) ), this, SLOT(keyPressEvent(QKeyEvent*)) );

	m_messageBox->hide();
	
	setSuffix( suffix );

	m_keyTimer = new QTimer();
	m_keyTimer->setSingleShot( true );
	m_keyTimer->setInterval( 1000 );
	connect( m_keyTimer, SIGNAL(timeout()), this, SLOT(updateModel()) );
}

FileEditor::~FileEditor() {
	clearSuffix();
	delete m_object;
	delete m_model; 
	delete m_element; 
	delete m_watcher;
	delete m_syntaxhighlighter;
}

void FileEditor::setMessage( QString message ) {
	m_messageErreur->setText( message );
	if( message.isEmpty() )
		m_messageBox->hide();
	else
		m_messageBox->show();
}

void FileEditor::slotBookmarkToggled( int line, bool enabled ) {
	Q_UNUSED( enabled );
	Q_UNUSED( line );
	
	if( enabled ) {
		gotoBookmarkAt( m_numbers->listOfBookmark().indexOf( line ) );
	}
	emit bookmarkModified( 0, bookmarkCount() );
}

QList<int> & FileEditor::bookmarks() const {
	return m_numbers->listOfBookmark(); 
}

void FileEditor::setBookmark( int line, bool enabled ) {
	m_numbers->setBookmark( line, enabled );
}

void FileEditor::toogledBookmark() {
	m_numbers->setBookmark( m_view->currentRow(), !m_numbers->listOfBookmark().contains( m_view->currentRow() ) );
}

void FileEditor::gotoBookmarkAt( int i ) {
	Editor::gotoBookmarkAt( i );
	gotoLine( m_numbers->listOfBookmark().at( i ) );
}

QString FileEditor::bookmarkAt( int i ) {
	QString description = tr( "In editor '%1' at line %2" );
	description = description
					.arg( getTitle() )
					.arg( m_numbers->listOfBookmark().at( i ) );
	return description;
}

int FileEditor::bookmarkCount() {
	return m_numbers->listOfBookmark().count();
}

bool FileEditor::previousBookmark() {
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

bool FileEditor::nextBookmark() {
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

void FileEditor::clearAllBookmark() {
	m_numbers->listOfBookmark().clear();
}

TextEditor * FileEditor::textEdit() const { 
	return m_view;
}

QTextEdit * FileEditor::qTextEdit() const {
	return m_view;
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
	if( ! m_prettyPrinterPlugin.first ) {
		QString message; // For error
		int line, column;
		int position = textEdit()->textCursor().position();
		QString result = m_prettyPrinterPlugin.first->prettyPrint( m_prettyPrinterPlugin.second, textEdit()->toPlainText(), &message, &line, &column );
		
		if( message.isEmpty() ) {
			textEdit()->selectAll();
			QTextCursor cursor = textEdit()->textCursor();
			cursor.beginEditBlock();
			cursor.removeSelectedText();
			cursor.insertText( result );
			cursor.endEditBlock();

			cursor = textEdit()->textCursor();
			cursor.setPosition( position );
			textEdit()->setTextCursor( cursor );
		} else {
			setMessage( tr("Parse error line %1 column %2 : %3").arg( line ).arg( column ).arg( message ) );
		}
	} else {
		setMessage( tr("Can't indent this type of document") );
	}
}

void FileEditor::commentSelectedText( bool uncomment ) {
	if( m_extendedEditorPlugin.first ) {
		m_extendedEditorPlugin.first->commentSelectedText( m_extendedEditorPlugin.second, this, uncomment );
	} else
		setMessage( tr("Can't comment this type of document") );
}

void FileEditor::complete() {
	if( m_extendedEditorPlugin.first ) {	
		QTextCursor cursor = textEdit()->textCursor();
		
		QRect cr = textEdit()->cursorRect();
		QString completionPrefix = textEdit()->textUnderCursor(cursor);
	
		QCompleter * c = m_extendedEditorPlugin.first->completer( m_extendedEditorPlugin.second, this );
	
		if( c ) {
			if( completionPrefix != c->completionPrefix() ) {
			    c->setCompletionPrefix( completionPrefix );
				c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
			}
			cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
			c->complete( cr );
		}
	}
}

void FileEditor::keyPressEvent( QKeyEvent * e ) {
	if(!e->text().isEmpty()) {
		m_keyTimer->start();
	}
	
	if( ( !m_extendedEditorPlugin.first ) || ( global.m_config->config().editor.completionLevel == 0 ) ) {
		textEdit()->keyPressExecute( e );
		return;
	}
		
	QCompleter * c = m_extendedEditorPlugin.first->completer( m_extendedEditorPlugin.second, this );
	
	if (c && c->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
		case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
			e->ignore();
			textEdit()->keyPressSkip( e );
			return; // let the completer do default behavior
		default:
            break;
        }
     }

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if( !c || !isShortcut )
		textEdit()->keyPressExecute( e );
	else
		textEdit()->keyPressSkip( e );
				
	m_extendedEditorPlugin.first->keyPress( m_extendedEditorPlugin.second, this, e );
	
	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

	static QString eow( EOW ); // end of word
	bool hasModifier = (e->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ));// && !ctrlOrShift;
	QString completionPrefix = textEdit()->textUnderCursor( textEdit()->textCursor() );

	if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
		c->popup()->hide();
		return;
	}

	if (completionPrefix != c->completionPrefix()) {
		c->setCompletionPrefix(completionPrefix);
		c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
	}

	QRect cr = textEdit()->cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
}

bool FileEditor::eventFilter( QObject *obj, QEvent *event ) {
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
		return m_suffix;
	else
		return QFileInfo( m_fileName ).suffix();
}

QIcon FileEditor::icon() const {
	QIcon icon = global.m_pluginsLoader->iconOfSuffix( getSuffix() );
	if( ! icon.isNull() )
		return icon;
	else
		return Editor::icon();
}

void FileEditor::clearSuffix() {
	delete m_syntaxhighlighter; m_syntaxhighlighter = NULL;
	m_prettyPrinterPlugin  = qMakePair( (IPluginPrettyPrint*)NULL, QString() );
	m_extendedEditorPlugin = qMakePair( (IPluginExtendedEditor*)NULL, QString() );
	delete m_object; m_object = NULL;
	delete m_model; m_model = NULL;
	delete m_element; m_element = NULL;
}

void FileEditor::setSuffix( const QString & suffix ) {
	if( ( suffix != m_suffix ) && !suffix.isEmpty() ) {
		clearSuffix();
		
		m_syntaxhighlighter = new SyntaxHighlighter( global.m_pluginsLoader->highlighterOfSuffix( suffix ), m_view );
		m_prettyPrinterPlugin = global.m_pluginsLoader->prettyPrinterOfSuffix( suffix );
		m_extendedEditorPlugin = global.m_pluginsLoader->extendedEditorOfSuffix( suffix );
		
		if( m_extendedEditorPlugin.first ) {
			m_element = m_extendedEditorPlugin.first->createModelData( m_extendedEditorPlugin.second, this );
			m_extendedEditorPlugin.first->createCompleter( m_extendedEditorPlugin.second, this );
			if( m_element ) {
				m_model = new FileContentItemModel( m_element, this );
				Q_ASSERT( dynamic_cast<FileContentParser*>( m_element ) );
				dynamic_cast<FileContentParser*>( m_element )->loadFromContent( textEdit()->toPlainText() );
			}
		} 

		m_suffix = suffix;
	} else if( suffix.isEmpty() ) {
		clearSuffix();
	}
}

void FileEditor::setFileName( const QString & fileName ) {
	QString suffix = QFileInfo( fileName ).suffix();
	
	if( ( suffix != m_suffix ) && !suffix.isEmpty() ) {
		clearSuffix();
	}
	
	if( ! fileName.isEmpty() ) {
		m_fileName = fileName;
		setWatcher( m_fileName );
	}
	
	setSuffix( suffix );
}

void FileEditor::createBackup( const QString & filename ) {
	if( global.m_config->config().editor.createBackupFile ){
		if( QFile::exists( filename + ".bak" ) ) 
			QFile::remove( filename + ".bak" );
		QFile::copy( filename, filename + ".bak" );
	}
}

void FileEditor::desactivateWatcher() {
	if( m_watcher )
		m_watcher->desactivate();
}

void FileEditor::activateWatcher() {
	if( ! m_watcher ) {
		m_watcher = new FileWatcher( m_path );
		connect( m_watcher, SIGNAL(fileChanged()), this, SLOT(fileChanged()) );
	} else
		m_watcher->activate();
}

void FileEditor::setWatcher( const QString & path ) {
	if( m_path != path ) {
		delete m_watcher;
		m_path = path;
		m_watcher = new FileWatcher( path );
		connect( m_watcher, SIGNAL(fileChanged()), this, SLOT(fileChanged()) );
	}
}

void FileEditor::fileChanged() {
	if( m_isSaving ) return;
	if( ! global.m_config->config().editor.popupWhenFileModified ) return ;

	m_watcher->desactivate();
	if( QFile( m_path ).exists() && QMessageBox::question( qApp->activeWindow(), tr("Reload page"), tr("The file %1 was modified. Reload the page ?").arg( QFileInfo( m_path ).fileName() ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
		loadFile();
	else
		setModified( true );
		
	if( ! QFile( m_path ).exists() ) {
		QMessageBox::warning( qApp->activeWindow(), tr("Reload page"), tr("The file %1 was removed.").arg( QFileInfo( m_path ).fileName() ) );
		setModified( true );		
	}
	m_watcher->activate();
}

void FileEditor::comment() {
	commentSelectedText();
}

void FileEditor::uncomment() {
	commentSelectedText( true );
}

void FileEditor::setIsSaving( bool value ) {
	m_isSaving = value;
	if( value ) {
		desactivateWatcher();
	} else {
		activateWatcher();
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
		activateWatcher();
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

void FileEditor::serialize( XSLProjectSessionEditor * data, bool content ) {
	Editor::serialize( data, content );
	data->writeProperty( "FileName", QVariant( m_fileName ) );
	data->writeProperty( "Position", QVariant( m_view->textCursor().position() ) );
	data->writeProperty( "Modified", QVariant( isModified() ) );
	
	if( content && m_view->document()->isModified() ) {
		data->writeProperty( "Content", QVariant( m_view->toPlainText() ) );
	}
	
	int i = 0;
	foreach( int line, m_numbers->listOfBookmark() ) {
		data->writeProperty( QString( "Bookmark_%1" ).arg( i++ ), QVariant( line ) );
	}
	data->writeProperty( "BookmarkCount", QVariant( i ) );
}

void FileEditor::deserialize( XSLProjectSessionEditor * data ) {
	int position = 0;
	bool isModified = false;
	QString text;
	
	Editor::deserialize( data );
	
	m_fileName = data->readProperty( "FileName" ).toString();
	setSuffix( QFileInfo( m_fileName ).suffix() );

	position  = data->readProperty( "Position" ) .toInt();
	isModified = data->readProperty( "Modified" ).toBool();
	text = data->readProperty( "Content" ).toString();

	int bc = data->readProperty( "BookmarkCount" ).toInt();
	for( int i = 0 ; i < bc; i++ ) {	
		setBookmark( data->readProperty( QString( "Bookmark_%1" ).arg( i ) ).toInt(), true );
	}

	if( ! text.isEmpty() ) {
		m_view->setPlainText( text );
		setModified( isModified );

		setWatcher( m_fileName );
		updateModel();
	} else {
		if( ! m_fileName.isEmpty() )
			loadFile( m_fileName );
	}

	QTextCursor tc = m_view->textCursor();
	tc.setPosition( position );
	m_view->setTextCursor( tc );
}

QAbstractItemModel * FileEditor::model() {
	return m_model;
}

void FileEditor::updateModel() {
	FileContentParser * parser = dynamic_cast<FileContentParser*>( m_element );
	if( ! parser ) return;
	try {
		parser->loadFromContent( textEdit()->toPlainText() ); 
		setMessage("");
	} catch( FileContentException e ) {
		setMessage( tr("%1 at %2").arg( e.getMessage() ).arg( e.getLine() ) );
	}
}

FileContentElement * FileEditor::importModelData( FileContentElement * parent, QString & filename, int line ) {
	QString suffix = QFileInfo( filename ).suffix();
	QString absPath = QString();
	bool finded = false;
	QStringList searchList;
	
	if( ! parent->filename().isEmpty() )
		searchList << QFileInfo( parent->filename() ).absolutePath();
	
	if( global.m_project )
		searchList += global.m_project->processedSearchPathList();
	
	foreach( QString path, searchList ) {
		absPath = QDir( path ).absoluteFilePath( filename );
		if( QFile::exists( absPath ) ) {
			finded = true;
			break;
		}
	}

	if( finded )
		filename = absPath;
	
	QPair<IPluginExtendedEditor*,QString> plugin = global.m_pluginsLoader->extendedEditorOfSuffix( suffix );
	if( plugin.first ) {
		FileContentElement * element = plugin.first->createModelData( plugin.second, this, parent, filename, line );
		Q_ASSERT( dynamic_cast<FileContentParser*>( element ) );
		if( !finded ) filename = QString();
		return element;
	} else {
		FileContentElement * element = new FileContentElement( parent, filename, line );
		if( !finded ) filename = QString();
		return element;
	}
}

FileContentElement * FileEditor::modelData() const {
	return m_element;
}

int FileEditor::level() const {
	return global.m_config->config().editor.completionLevel;
}

QString FileEditor::textUnderCursor( const QTextCursor & cursor, bool deleteWord ) {
	return textEdit()->textUnderCursor( cursor, deleteWord, true );
}

QObject * FileEditor::object() const {
	return m_object;
}

void FileEditor::setObject( QObject * object ) {
	m_object = object;
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
	bool needEmit = modified != isModified();

	m_view->document()->setModified( modified );
	
	if( needEmit )
		emit modificationChanged( modified );
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
	return m_commentAction;
}

QAction * FileEditor::uncommentAction() {
	return m_uncommentAction;
}

void FileEditor::updateHighlighter() {
	if( m_syntaxhighlighter ) 
		m_syntaxhighlighter->rehighlight();
}

void FileEditor::refreshTextHighlighter() {
	if( m_syntaxhighlighter && global.m_config->config().editor.autoHighlight ) 
		m_syntaxhighlighter->setHighlightText( m_view->textUnderCursor( m_view->textCursor(), false, false ) );
}

void FileEditor::callTextHighlighter() {
	if( m_syntaxhighlighter ) 
		m_syntaxhighlighter->setHighlightText( m_view->textUnderCursor( m_view->textCursor(), false, false ) );
}

void FileEditor::gotoLine( int line ) {
	if( line == m_view->currentRow() ) return;
	QTextCursor cursor = m_view->textCursor();
	cursor.movePosition( QTextCursor::Start );
	cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, line - 1 );
	m_view->setTextCursor( cursor );
}

