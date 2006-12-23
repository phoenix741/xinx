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
 
#include "editor.h"
#include "texteditor.h"

NumberBar::NumberBar( QWidget *parent )
          : QWidget( parent ), m_edit(0) {
  setFixedWidth( fontMetrics().width( QString("0000") + 10 + 32 ) );
}

NumberBar::~NumberBar() {
}

void NumberBar::setTextEdit( QTextEdit *edit ) {
  this->m_edit = edit;
  connect( m_edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ), this, SLOT( update() ) );
  connect( m_edit->verticalScrollBar(), SIGNAL( valueChanged(int) ), this, SLOT( update() ) );
}

void NumberBar::paintEvent( QPaintEvent * ) {
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
    if ( position.y() + boundingRect.height() < contentsY )
      continue;
    if ( position.y() > pageBottom )
      break;
 
    const QString txt = QString::number( lineCount );
    p.drawText( width() - fm.width(txt), qRound( position.y() ) - contentsY + ascent, txt );

  }
}

/* Editor */

Editor::Editor( QWidget *parent ) : QFrame( parent ), m_curFile("") {
  setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  setLineWidth( 2 );
  
  view = new StudioTextEdit( this );
  view->setFrameStyle( QFrame::NoFrame );
  view->setLineWrapMode(QTextEdit::NoWrap);
  view->installEventFilter( this );
  
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

void Editor::uploSelectedText(bool upper) {
  QTextCursor cursor(view->textCursor());
  
  int startPos = cursor.selectionStart();
  int endPos   = cursor.selectionEnd();
  QString text = cursor.selectedText();
  
  if(upper)
    text = text.toUpper();
  else
    text = text.toLower();
  
  cursor.beginEditBlock();

  cursor.insertText(text);  
  
  cursor.setPosition(startPos, QTextCursor::MoveAnchor);
  cursor.setPosition(endPos, QTextCursor::KeepAnchor);

  cursor.endEditBlock();

  view->setTextCursor(cursor);
}

void Editor::commentSelectedText(bool uncomment) {
  QTextCursor cursor(view->textCursor());
 
  QTextCursor cursorStart(view->textCursor());
  cursorStart.setPosition(cursor.selectionStart());
  bool isStartCommented = view->isCodeCommented(cursorStart);

  QTextCursor cursorEnd(view->textCursor());
  cursorEnd.setPosition(cursor.selectionEnd());
  bool isEndCommented =  view->isCodeCommented(cursorEnd);

  QString text = cursor.selectedText();
  text = text.replace("<!--", "");
  text = text.replace("-->", "");
  
  cursor.beginEditBlock();

  cursor.removeSelectedText();
  if(! ( isStartCommented ^ uncomment ) ) {
    // Comment  	
    if(! uncomment)
      cursor.insertText("<!--");  
    else
      cursor.insertText("-->");  
  }
  cursor.insertText(text);  
  if(! ( isEndCommented ^ uncomment )) {
    // End the comment  	
    if(! uncomment)
      cursor.insertText("-->");  
    else
      cursor.insertText("<!--");  
  }

  cursor.endEditBlock();
}

void Editor::textChanged( int pos, int removed, int added ) {
  Q_UNUSED( pos );
 
  if ( removed == 0 && added == 0 )
    return;

  /*
     Il faut savoir si on édite un noeud, un paramètre, la valeur du paramètre, ou du texte.
     Si on édite un noeud : affichage de la completion sur les noeud
     Si on édite un paramêtre : affichage de la completion sur les paramêtre pour un noeud donné
     Si on édite une valeur : Edition d'un XPath
     Si on édite du texte : peut rien faire ....
  */
  
  if( added == 1 ) {
     	
  }
  
  QString text = QString("Balise : %1\nNoeud: %2(%5)\nParam: %3\nValeur: %4").arg( view->isEditBalise( view->textCursor() ) )
  																		 .arg( view->isEditNode( view->textCursor() ) )
  																		 .arg( view->isEditParam( view->textCursor() ) )
  																		 .arg( view->isEditValue( view->textCursor() ) )
  																		 .arg( view->nodeName( view->textCursor() ) );


  setToolTip(text);
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

/*

  int startPos = cursor.selectionStart();
  int endPos = cursor.selectionEnd();
  QTextBlock startBlock = view->document()->findBlock(startPos);
  QTextBlock endBlock = view->document()->findBlock(endPos);

  cursor.beginEditBlock();
  
  QTextBlock block = startBlock;
  cursor.setPosition(startPos);

  while ( ! (endBlock < block) ) {
  	QString text = block.text();
    if (text.isEmpty()) continue;

    if(upper)
	  text = text.toUpper();
	else
	  text = text.toLower();

    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    cursor.insertText(text);
    cursor.movePosition(QTextCursor::NextBlock);
    block = cursor.block();
  }
   
  cursor.endEditBlock();
*/

