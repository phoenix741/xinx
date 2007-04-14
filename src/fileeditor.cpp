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

#include "fileeditor.h"
#include "xmlhighlighter.h"
#include "jshighlighter.h"
#include "xmleditor.h"
#include "jseditor.h"
#include "xslproject.h"
#include "xinxconfig.h"

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

/* NumberBar */

class NumberBar : public QWidget {
	Q_OBJECT
public:
	NumberBar( QWidget *parent );
	virtual ~NumberBar();
 
	void setTextEdit( QTextEdit *edit );
	void paintEvent( QPaintEvent *ev );
private:
	QTextEdit *m_edit;
};

NumberBar::NumberBar( QWidget *parent ) : QWidget( parent ), m_edit(0) {
	setFixedWidth( fontMetrics().width( QString("0000") + 10 + 32 ) );
}

NumberBar::~NumberBar() {
}

void NumberBar::setTextEdit( QTextEdit *edit ) {
	this->m_edit = edit;
	connect( m_edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ), this, SLOT( update() ) );
	connect( m_edit->verticalScrollBar(), SIGNAL( valueChanged(int) ), this, SLOT( update() ) );
}

void NumberBar::paintEvent( QPaintEvent * event ) {
	Q_UNUSED( event );
	
	QAbstractTextDocumentLayout *layout = m_edit->document()->documentLayout();
	int contentsY = m_edit->verticalScrollBar()->value();
	qreal pageBottom = contentsY + m_edit->viewport()->height();
	const QFontMetrics fm = fontMetrics();
	const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
	int lineCount = 1;

	QPainter p(this);
 
	for ( QTextBlock block = m_edit->document()->begin(); block.isValid(); block = block.next(), ++lineCount ) {
		const QRectF boundingRect = layout->blockBoundingRect( block );
 
		QPointF position = boundingRect.topLeft();
		if ( position.y() + boundingRect.height() < contentsY ) continue;
		if ( position.y() > pageBottom ) break;
 
		const QString txt = QString::number( lineCount );
		p.drawText( width() - fm.width(txt), qRound( position.y() ) - contentsY + ascent, txt );
	}
}

/* Editor */

class TextEditor : public QTextEdit {
	Q_OBJECT
public:
	TextEditor( QWidget * parent = 0, XSLProject * project = NULL );
	virtual ~TextEditor();

	virtual void commentSelectedText( bool uncomment = false );
	virtual void complete();

	void formatToXML();
	void formatToJS();
	void formatToNothing();
	
	QAbstractItemModel * model();
	
signals:
	void deleteModel();
	void createModel();

protected:
	void keyPressEvent(QKeyEvent *e);
	void parentKeyPressEvent( QKeyEvent * e );
    void mouseDoubleClickEvent( QMouseEvent * event );

private:
	XSLProject * m_project;
	TextProcessor * m_processor;
	QSyntaxHighlighter * m_highlighter;
	
	friend class TextProcessor;
	friend class FileEditor;
};

TextEditor::TextEditor( QWidget * parent, XSLProject * project ) : QTextEdit( parent ), m_project( project ), m_processor( 0 ), m_highlighter( 0 ) { 
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
	formatToNothing();
}

QAbstractItemModel * TextEditor::model() {
	if( m_processor )
		return m_processor->model();
	return NULL;
}

void TextEditor::formatToNothing() {
	emit deleteModel();
	if( m_highlighter ) {
		delete m_highlighter;  
		m_highlighter = NULL;
	}
	if( m_processor ) {
		delete m_processor; 
		m_processor = NULL;
	}
}

void TextEditor::formatToXML() {
	formatToNothing();
	m_highlighter = new XmlHighlighter( document() );
	m_processor = new XMLProcessor( this, m_project, this );
	emit createModel();
}

void TextEditor::formatToJS() {
	formatToNothing();
	m_highlighter = new JsHighlighter( document() );
	m_processor = new JSProcessor( this, m_project, this );
}

void TextEditor::commentSelectedText( bool uncomment ) {
	if( m_processor )
		m_processor->commentSelectedText( uncomment );
}

void TextEditor::complete() {
	if( m_processor )
		m_processor->complete();

}

void TextEditor::keyPressEvent(QKeyEvent *e) {
	if( m_processor ) 
		m_processor->keyPressEvent( e );
	else
		parentKeyPressEvent(e);

}

void TextEditor::parentKeyPressEvent( QKeyEvent * e ) {
	QTextEdit::keyPressEvent( e );
}

void TextEditor::mouseDoubleClickEvent( QMouseEvent * event ) {
	QPoint mousePosition = event->pos();
	QString m_plainText = toPlainText();
    QTextCursor cursor = textCursor();
    int pos = cursor.position();
    while ( pos>0  && QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_").contains( m_plainText.at( pos-1 ).toUpper()  ) )
        pos--;
    cursor.setPosition(pos, QTextCursor::MoveAnchor);
    setTextCursor( cursor );
    //
    while ( pos < m_plainText.length()  && QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_").contains( m_plainText.at( pos ).toUpper()  ) )
        pos++;
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    setTextCursor( cursor );
}

/* TextProcessor */

void TextProcessor::parentKeyPressEvent( QKeyEvent * e ) {
	static_cast<TextEditor*>( m_widget )->parentKeyPressEvent( e );
}

/* FileEditor */

FileEditor::FileEditor( QWidget *parent, XSLProject * project ) : Editor( parent, project ) {
	m_view = new TextEditor( this, project );
	m_view->setFrameStyle( QFrame::NoFrame );
	m_view->setLineWrapMode(QTextEdit::NoWrap);
	m_view->installEventFilter( this );
  
	// Setup the line number pane
	m_numbers = new NumberBar( this );
	m_numbers->setTextEdit( m_view );
 
	QHBoxLayout * box = new QHBoxLayout( this );
	box->setSpacing( 0 );
	box->setMargin( 0 );
	box->addWidget( m_numbers );
	box->addWidget( m_view ); 
	
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)) );
	connect( m_view, SIGNAL(copyAvailable(bool)), this, SIGNAL(selectionAvailable(bool)) );
	connect( m_view, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)) );
	connect( m_view, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)) );

	connect( m_view->document(), SIGNAL(modificationChanged(bool)), this, SIGNAL(modificationChanged(bool)) );
	
	connect( m_view, SIGNAL(deleteModel()), this, SIGNAL(deleteModel()) );
	connect( m_view, SIGNAL(createModel()), this, SIGNAL(createModel()) );
}

QTextEdit * FileEditor::textEdit() const { 
	return static_cast<QTextEdit*>( m_view );
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

void FileEditor::setFileName( const QString & name ) {
	m_fileName = QDir::fromNativeSeparators( name );
	if( QDir::match( "*.xml;*.xsl;*.html", m_fileName ) ) 
		m_view->formatToXML();
	else
	if( QDir::match( "*.js", m_fileName ) ) 
		m_view->formatToJS();
	else
		m_view->formatToNothing();
}

void FileEditor::loadFile( const QString & fileName ){
	if( fileName != "" ) setFileName( fileName );

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
	
	if( m_view->m_processor ) {
		m_view->m_processor->updateModel();
	}
	QApplication::restoreOverrideCursor();
}

bool FileEditor::saveFile( const QString & fileName ){
	if( ( ! fileName.isEmpty() ) && ( ! m_fileName.isEmpty() ) && ( fileName != m_fileName ) ) {
		bool isOldSpecifiqueFile = QFileInfo( m_fileName ).fileName().startsWith( m_project->specifPrefix().toLower() + "_" );
		bool isNewSpecifiqueFile = QFileInfo( fileName ).fileName().startsWith( m_project->specifPrefix().toLower() + "_" );
		QString destName = QDir( m_project->specifPath() ).absoluteFilePath( QFileInfo( m_fileName ).fileName() );
		
		if( (!isOldSpecifiqueFile) && isNewSpecifiqueFile )
			QFile::copy( m_fileName, destName );
	}
	if( xinxConfig->isCreateBackupFile() && ( fileName.isEmpty() || ( m_fileName == fileName ) )  ){
		if( QFile::exists( m_fileName + ".bak" ) ) 
			QFile::remove( m_fileName + ".bak" );
		QFile::copy( m_fileName, m_fileName + ".bak" );
	}
	
	if( fileName != "" ) setFileName( fileName );
	
	QFile file( getFileName() );
	if ( ! file.open( QFile::WriteOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XINX" ), tr( "Cannot write file %1:\n%2." )
																.arg( fileName )
																.arg( file.errorString() ) );
		return false;
	}

	QTextStream out( &file );
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << m_view->toPlainText();

	if( m_view->m_processor ) {
		m_view->m_processor->updateModel();
	}

	QApplication::restoreOverrideCursor();

	return true;	
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
