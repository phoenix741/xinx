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

#include "globals.h"
#include "fileeditor.h"
#include "xmlhighlighter.h"
#include "jshighlighter.h"
#include "xmleditor.h"
#include "jseditor.h"
#include "xslproject.h"
#include "xinxconfig.h"
#include "texteditor.h"
#include "numberbar.h"

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
#include <QFileSystemWatcher>

/* PrivateFileEditor */

class PrivateFileEditor : QObject {
	Q_OBJECT
public:
	PrivateFileEditor( FileEditor * parent );
	~PrivateFileEditor();
	
	bool hasWatcher();
	void setWatcher( const QString & path );
	void activateWatcher();
	void desactivateWatcher();
public slots:
	void fileChanged ( const QString & path );
private:
	QFileSystemWatcher * m_watcher;
	FileEditor * m_parent;
	QString m_path;
};

PrivateFileEditor::PrivateFileEditor( FileEditor * parent ) {
	m_parent = parent;
	m_watcher = new QFileSystemWatcher( this );
}

PrivateFileEditor::~PrivateFileEditor() {
	delete m_watcher;
}

void PrivateFileEditor::setWatcher( const QString & path ) {
	m_path = path;
	activateWatcher();
}

void PrivateFileEditor::activateWatcher() {
	connect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );
	m_watcher->addPath( m_path );
}

void PrivateFileEditor::desactivateWatcher() {
	disconnect( m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)) );
	m_watcher->removePath( m_path );
}

void PrivateFileEditor::fileChanged( const QString & path ) {
	if( global.m_xinxConfig->popupWhenFileModified() && QMessageBox::question( qApp->activeWindow(), tr("Reload page"), tr("The file %1 was modified. Reload the page ?").arg( QFileInfo( path ).fileName() ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
		m_parent->loadFile();
}

/* FileEditor */

FileEditor::FileEditor( TextEditor * textEditor, QWidget *parent ) : Editor( parent ) {
	d = new PrivateFileEditor( this );
	
	m_view = textEditor;
	m_view->setFrameStyle( QFrame::NoFrame );
	m_view->setLineWrapMode(QTextEdit::NoWrap);
	m_view->installEventFilter( this );
  
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
	
	connect( m_view, SIGNAL(deleteModel()), this, SIGNAL(deleteModel()) );
	connect( m_view, SIGNAL(createModel()), this, SIGNAL(createModel()) );

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

void FileEditor::commentSelectedText( bool uncomment ) {
	m_view->commentSelectedText( uncomment );
}

void FileEditor::complete() {
	m_view->complete();
}

bool FileEditor::eventFilter( QObject *obj, QEvent *event ) {
	if ( obj != m_view )
		return QFrame::eventFilter( obj, event );

	if ( event->type() == QEvent::ToolTip ) {
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>( event );

		QTextCursor cursor = m_view->cursorForPosition( helpEvent->pos() );
		cursor.movePosition( QTextCursor::StartOfWord, QTextCursor::MoveAnchor );
		cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );

		QString word = cursor.selectedText();
		emit mouseHover( word );
		emit mouseHover( helpEvent->pos(), word );

		// QToolTip::showText( helpEvent->globalPos(), word ); // For testing
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
	if( ! ( m_fileName.isEmpty() || fileName.isEmpty() ) ) {
		QString prefix = global.m_project->specifPrefix().toLower() + "_";
		bool isOldSpecifiqueFile = QFileInfo( m_fileName ).fileName().startsWith( prefix );
		bool isNewSpecifiqueFile = QFileInfo( fileName ).fileName().startsWith( prefix );

		QString infoFileName = QFileInfo( m_fileName ).fileName();
		QString destName = QDir( QDir( global.m_project->specifPath() ).absoluteFilePath( global.m_xinxConfig->managedFile4Name( infoFileName ).customPath ) ).absoluteFilePath( infoFileName );

		if( (!isOldSpecifiqueFile) && isNewSpecifiqueFile )
			QFile::copy( m_fileName, destName );
	}

	if( ! fileName.isEmpty() ) {
		m_fileName = fileName;
		d->setWatcher( m_fileName );
	}
}

void FileEditor::createBackup( const QString & filename ) {
	if( global.m_xinxConfig->isCreateBackupFile() ){
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

void FileEditor::loadFile( const QString & fileName ){
	if( ! fileName.isEmpty() ) m_fileName = fileName;

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
	m_view->updateModel();

	QApplication::restoreOverrideCursor();
}

bool FileEditor::saveFile( const QString & fileName ){
	d->desactivateWatcher();

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

	QTextStream out ( &file );
	out << m_view->toPlainText();
	file.flush();
	file.close();

	m_view->updateModel();

	d->activateWatcher();

	QApplication::restoreOverrideCursor();

	return true;	
}

void FileEditor::serializeEditor( QDomElement & element, bool content ) {
	element.setAttribute( "class", metaObject()->className() );
	element.setAttribute( "filename", m_fileName );
	element.setAttribute( "position", m_view->textCursor().position() );

	if( content && m_view->document()->isModified() ) {
		element.setAttribute( "ismodified", m_view->document()->isModified() );
		QDomText text = global.m_project->sessionDocument().createTextNode( m_view->toPlainText() );
		element.appendChild( text );
	}
}

void FileEditor::deserializeEditor( const QDomElement & element ) {
	m_fileName = element.attribute( "filename" );
	
	QString plainText;
	QDomNode node = element.firstChild();
	while( ! node.isNull() ) {
		if( node.isText() ) {
			plainText += node.toText().data();
		}
				
		node = node.nextSibling();
	}
	if( ! plainText.isEmpty() ) {
		m_view->setPlainText( plainText );
		m_view->document()->setModified( (bool)(element.attribute( "ismodified" ).toInt()) );
	} else {
		if( !m_fileName.isEmpty() )
			loadFile( m_fileName );
	}

	QTextCursor tc = m_view->textCursor();
	tc.setPosition( element.attribute( "position" ).toInt() );
	m_view->setTextCursor( tc );
	
	d->setWatcher( m_fileName );
}

QAbstractItemModel * FileEditor::model() {
	return m_view->model();
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

#include "fileeditor.moc"
