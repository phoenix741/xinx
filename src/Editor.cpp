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
 
#include "Editor.h"
#include "xmlhighlighter.h"

NumberBar::NumberBar( QWidget *parent )
          : QWidget( parent ), edit(0), m_stopLine(-1), m_currentLine(-1), m_bugLine(-1) {
  setFixedWidth( fontMetrics().width( QString("0000") + 10 + 32 ) );
  stopMarker = QPixmap( ":/images/nogood.png" );
  currentMarker = QPixmap( ":/images/next.png" );
  bugMarker = QPixmap( ":/images/bug.png" );
}

NumberBar::~NumberBar() {
}

void NumberBar::setCurrentLine( int lineno ) {
  m_currentLine = lineno;
}

void NumberBar::setStopLine( int lineno ) {
  m_stopLine = lineno;
}

void NumberBar::setBugLine( int lineno ) {
  m_bugLine = lineno;
}

int NumberBar::currentLine() const {
  return m_currentLine;
}

int NumberBar::stopLine() const {
  return m_stopLine;
}

int NumberBar::bugLine() const {
  return m_bugLine;
}

void NumberBar::setTextEdit( QTextEdit *edit ) {
  this->edit = edit;
  connect( edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ), this, SLOT( update() ) );
  connect( edit->verticalScrollBar(), SIGNAL( valueChanged(int) ), this, SLOT( update() ) );
}

void NumberBar::paintEvent( QPaintEvent * ) {
  QAbstractTextDocumentLayout *layout = edit->document()->documentLayout();
  int contentsY = edit->verticalScrollBar()->value();
  qreal pageBottom = contentsY + edit->viewport()->height();
  const QFontMetrics fm = fontMetrics();
  const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
  int lineCount = 1;

  QPainter p(this);
 
  bugRect = QRect();
  stopRect = QRect();
  currentRect = QRect();
 
  for ( QTextBlock block = edit->document()->begin(); block.isValid(); block = block.next(), ++lineCount ) {
 
    const QRectF boundingRect = layout->blockBoundingRect( block );
 
    QPointF position = boundingRect.topLeft();
    if ( position.y() + boundingRect.height() < contentsY )
      continue;
    if ( position.y() > pageBottom )
      break;
 
    const QString txt = QString::number( lineCount );
    p.drawText( width() - fm.width(txt), qRound( position.y() ) - contentsY + ascent, txt );
 
    // Bug marker
    if ( m_bugLine == lineCount ) {
      p.drawPixmap( 1, qRound( position.y() ) - contentsY, bugMarker );
      bugRect = QRect( 1, qRound( position.y() ) - contentsY, bugMarker.width(), bugMarker.height() );
    }
 
    // Stop marker
    if ( m_stopLine == lineCount ) {
      p.drawPixmap( 1, qRound( position.y() ) - contentsY, stopMarker );
      stopRect = QRect( 1, qRound( position.y() ) - contentsY, stopMarker.width(), stopMarker.height() );
    }
 
    // Current line marker
    if ( m_currentLine == lineCount ) {
      p.drawPixmap( 1, qRound( position.y() ) - contentsY, currentMarker );
      currentRect = QRect( 1, qRound( position.y() ) - contentsY, currentMarker.width(), currentMarker.height() );
    }
  }
}

bool NumberBar::event( QEvent *event ) {
  if ( event->type() == QEvent::ToolTip ) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

    if ( stopRect.contains( helpEvent->pos() ) ) {
      QToolTip::showText( helpEvent->globalPos(), "Stop Here" );
    } else if ( currentRect.contains( helpEvent->pos() ) ) {
      QToolTip::showText( helpEvent->globalPos(), "Current Line" );
    } else if ( bugRect.contains( helpEvent->pos() ) ) {
      QToolTip::showText( helpEvent->globalPos(), "Error Line" );
    }
  }
 
  return QWidget::event(event);
}

Editor::Editor( QWidget *parent ) : QFrame( parent ), m_curFile("") {
  setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  setLineWidth( 2 );
  
  // Setup the main view
  QFont font;
  font.setFamily("Monospace");
  font.setFixedPitch(true);
  font.setPointSize(10);

  view = new QTextEdit( this );
  view->setFrameStyle( QFrame::NoFrame );
  view->installEventFilter( this );
  view->setLineWrapMode(QTextEdit::NoWrap);
  view->setFont(font);
  view->setTabStopWidth( 15 );
  new XmlHighlighter(view->document());
  
  connect( view->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(textChanged(int,int,int)) );
  
  // Setup the line number pane
  numbers = new NumberBar( this );
  numbers->setTextEdit( view );
 
  box = new QHBoxLayout( this );
  box->setSpacing( 0 );
  box->setMargin( 0 );
  box->addWidget( numbers );
  box->addWidget( view ); 
}

void Editor::setCurrentLine( int lineno ) {
  numbers->setCurrentLine( lineno );
  textChanged( 0, 0, 1 );
}

void Editor::duplicateCurrentLine() {
  QTextCursor cursor(view->textCursor());
  cursor.beginEditBlock();
  QTextBlock currentBlock = cursor.block();
  cursor.movePosition(QTextCursor::StartOfLine);
  cursor.insertText(currentBlock.text() + "\n");
  cursor.endEditBlock();
}

void Editor::moveLineUp() {
  QTextCursor cursor(view->textCursor());
  cursor.beginEditBlock();
  QTextBlock currentBlock = cursor.block();
  QString text = currentBlock.text();
  QTextBlock precBlock = currentBlock.previous();

  cursor.select(QTextCursor::BlockUnderCursor);
  cursor.removeSelectedText();

  cursor.setPosition(precBlock.position());
  cursor.insertText(text + "\n");

  cursor.endEditBlock();
  view->moveCursor(QTextCursor::PreviousBlock);
}

void Editor::moveLineDown() {
  QTextCursor cursor(view->textCursor());
  cursor.beginEditBlock();
  cursor.movePosition(QTextCursor::NextBlock);

  QTextBlock currentBlock = cursor.block();
  QString text = currentBlock.text();
  QTextBlock precBlock = currentBlock.previous();

  cursor.select(QTextCursor::BlockUnderCursor);
  cursor.removeSelectedText();

  cursor.setPosition(precBlock.position());
  cursor.insertText(text + "\n");

  cursor.endEditBlock();
}

void Editor::upperSelectedText() {
	
}

void Editor::lowerSelectedText() {
	
}

void Editor::commentSelectedText() {
	
}

void Editor::uncommentSelectedText() {
	
}

void Editor::setStopLine( int lineno ) {
  numbers->setStopLine( lineno );
}

void Editor::setBugLine( int lineno ) {
  numbers->setBugLine( lineno );
}

int Editor::currentLine() const {
  return numbers->currentLine();
}

int Editor::stopLine() const {
  return numbers->stopLine();
}

int Editor::bugLine() const {
  return numbers->bugLine();
}

QString Editor::text() const {
  return view->toPlainText ();
}

void Editor::setText( const QString &text ) {
  view->setPlainText(text);
}

void Editor::textChanged( int pos, int removed, int added ) {
  Q_UNUSED( pos );
 
  if ( removed == 0 && added == 0 )
    return;

  QTextBlock block = highlight.block();
  QTextBlockFormat fmt = block.blockFormat();
  QColor bg = view->palette().base().color();
  fmt.setBackground( bg );
  highlight.setBlockFormat( fmt );

  int lineCount = 1;
  for ( QTextBlock block = view->document()->begin(); block.isValid(); block = block.next(), ++lineCount ) {

    if ( lineCount == numbers->currentLine() ) {
      fmt = block.blockFormat();
      QColor bg = view->palette().color(QPalette::Highlight).light( 175 );
      fmt.setBackground( bg );

      highlight = QTextCursor( block );
      highlight.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
      highlight.setBlockFormat( fmt );

      break;
    }
  }
}

bool Editor::eventFilter( QObject *obj, QEvent *event ) {
  if ( obj != view )
    return QFrame::eventFilter(obj, event);

  if ( event->type() == QEvent::ToolTip ) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

    QTextCursor cursor = view->cursorForPosition( helpEvent->pos() );
    cursor.movePosition( QTextCursor::StartOfWord, QTextCursor::MoveAnchor );
    cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );

    QString word = cursor.selectedText();
    emit mouseHover( word );
    emit mouseHover( helpEvent->pos(), word );

    // QToolTip::showText( helpEvent->globalPos(), word ); // For testing
  }

  return false;
}

const QString & Editor::getCurrentFile() const {
	return m_curFile;
}

void Editor::setCurrentFile(const QString & name){
	m_curFile = name;
}

void Editor::loadFile(const QString &fileName){
  QFile file(fileName);
  if (! file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("XML Visual Studio"),
                               tr("Cannot read file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
    return;
  }

  setCurrentFile(fileName);

  QTextStream in(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  view->setPlainText(in.readAll());
  QApplication::restoreOverrideCursor();
}

bool Editor::saveFile(const QString &fileName){
  if(fileName != "") setCurrentFile(fileName);
	
  QFile file(m_curFile);
  if (! file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("XML Visual Studio"),
                               tr("Cannot write file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
    return false;
  }

  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  out << view->toPlainText();
  QApplication::restoreOverrideCursor();

  return true;	
}



/*
void MainWindow::onContentChange(int position, int charsRemoved, int charsAdded)
{
    QString contenu = editor->toPlainText();
    QTextCursor cursor(editor->textCursor());
    QRegExp expression("<([A-Za-z0-9]*)>");
    expression.setMinimal(true);
    int positionT = 0;
        int pos = cursor.position();

    if(expression.lastIndexIn(contenu, pos) != -1)
    {
        int whoIsChar = contenu.indexOf(">", position);
        positionT = contenu.lastIndexOf(expression.cap(), position)+expression.matchedLength()-1;
        * Si :
        * - ">" a ete trouve
        *
        * - aucun caractere n'est effacer (sans ca, il ajoute des balises
        * a chaque appuie sur la touche d'effacement
        *
        * - la position du curseur est superieur a la position de ">"
        * 
        * - le nombre de caractere ajoute vaut 1 (ce qui permet le copier-coller
        * de balise sans qu'une nouvelle balise soit ajoutee) 

        if(whoIsChar > -1 && charsRemoved == 0 && pos > whoIsChar && charsAdded < 2 && position == positionT )
        {
            QString insertion = "</"+expression.cap(1)+">";
            cursor.insertText(insertion); // insere le texte </balise> par ex
            cursor.setPosition(cursor.anchor() - insertion.length()); // le place avant la balise fermante inseree
            editor->setTextCursor(cursor); // Reactualise le curseur pour qu'il soit visible
        }
    }
}        
*/
