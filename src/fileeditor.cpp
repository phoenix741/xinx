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

#include <QtGui>
 
#include "fileeditor.h"
#include "xmlhighlighter.h"
#include "jshighlighter.h"
#include "xmleditor.h"

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
	
	QAbstractItemModel * model();
protected:
	void keyPressEvent(QKeyEvent *e);
	void parentKeyPressEvent( QKeyEvent * e );
    void mouseDoubleClickEvent( QMouseEvent * event );

private:
	XSLProject * m_project;
	TextProcessor * m_processor;
	QSyntaxHighlighter * m_highlighter;
	
	friend class TextProcessor;
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
}

QAbstractItemModel * TextEditor::model() {
	if( m_processor )
		return m_processor->model();
	return NULL;
}

void TextEditor::formatToXML() {
	delete m_highlighter;
	m_highlighter = new XmlHighlighter( document() );
	delete m_processor;
	m_processor = new XMLProcessor( this, m_project, this );
}

void TextEditor::formatToJS() {
	delete m_highlighter;
	m_highlighter = new JsHighlighter( document() );
	delete m_processor;
	m_processor = NULL;
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
}

void FileEditor::loadFile( const QString & fileName ){
	QFile file( fileName );
	if ( ! file.open( QFile::ReadOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XML Visual Studio" ), tr( "Cannot read file %1:\n%2." )
																.arg( fileName )
																.arg( file.errorString() ) );
		return;
	}

	QTextStream in( &file );
	QApplication::setOverrideCursor( Qt::WaitCursor );
	m_view->setPlainText( in.readAll() );
	QApplication::restoreOverrideCursor();

	setFileName( fileName );
}

bool FileEditor::saveFile( const QString & fileName ){
	if( fileName != "" ) setFileName( fileName );
	
	QFile file( getFileName() );
	if ( ! file.open( QFile::WriteOnly | QFile::Text ) ) {
		QMessageBox::warning(this, tr( "XML Visual Studio" ), tr( "Cannot write file %1:\n%2." )
																.arg( fileName )
																.arg( file.errorString() ) );
		return false;
	}

	QTextStream out( &file );
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << m_view->toPlainText();
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
