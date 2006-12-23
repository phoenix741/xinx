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
 
#include "tabeditor.h"
#include "editor.h"
#include "texteditor.h"

#include <QtGui>

TabEditor::TabEditor(QWidget * parent) : QTabWidget(parent), prec(NULL) {
  setAcceptDrops(true);
       
  connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}

Editor * TabEditor::newTab(const QString & title) {
  Editor * textEdit = new Editor( this );
  textEdit->textEdit()->setAcceptRichText(false);
  
  int index = addTab(textEdit, title);
  
  setTabIcon( index, QIcon(":/images/doc.png") );
  setCurrentIndex( index );

  slotCurrentTabChanged( currentIndex() );  

  return textEdit;	
}

void TabEditor::updateTitle(int i) {
  if(! currentEditor()->getCurrentFile().isEmpty())
    setTabText ( i, strippedName( currentEditor()->getCurrentFile() ) );
  else
    setTabText ( i, "untilted.xsl" );
}


void TabEditor::loadTab(const QString & fileName) {
  Editor * textEdit = newTab( strippedName(fileName) );
  textEdit->loadFile( fileName );
  updateTitle( currentIndex() );
}

void TabEditor::copy() {
  currentEditor()->textEdit()->copy();
}
  
void TabEditor::cut() {
  currentEditor()->textEdit()->cut();
}

void TabEditor::paste() {
  currentEditor()->textEdit()->paste();
}

void TabEditor::undo() {
  currentEditor()->textEdit()->undo();
}

void TabEditor::redo() {
  currentEditor()->textEdit()->redo();
}
    
void TabEditor::selectAll() {
  currentEditor()->textEdit()->selectAll();
}

void TabEditor::duplicateCurrentLine() {
  currentEditor()->duplicateCurrentLine();
}

void TabEditor::moveLineUp() {
  currentEditor()->moveLineUp();
}

void TabEditor::moveLineDown() {
  currentEditor()->moveLineDown();
}

void TabEditor::upperSelectedText() {
  currentEditor()->uploSelectedText(true);
}

void TabEditor::lowerSelectedText() {
  currentEditor()->uploSelectedText(false);
}

void TabEditor::commentSelectedText() {
  currentEditor()->commentSelectedText();
}

void TabEditor::uncommentSelectedText() {
  currentEditor()->commentSelectedText(true);
}

void TabEditor::complete() {
  currentEditor()->textEdit()->complete();
}

   
void TabEditor::slotDocumentWasModified() {
  QString document = "untilted.xsl";
  if(! currentEditor()->getCurrentFile().isEmpty() )
  	document = strippedName( currentEditor()->getCurrentFile() );
  setTabText ( currentIndex(), tr("%1*").arg( document ) );
}

void TabEditor::slotCurrentTabChanged(int index) {
  if(prec) prec->disconnect();
  Editor * ed = (Editor*)widget(index);
  
  emit copyAvailable(false);
  emit undoAvailable(ed->textEdit()->document()->isUndoAvailable());
  emit redoAvailable(ed->textEdit()->document()->isRedoAvailable());
  	
  connect( ed->textEdit()->document(), SIGNAL( contentsChanged() ), this, SLOT( slotDocumentWasModified() ) );
  connect( ed->textEdit(), SIGNAL( copyAvailable(bool) ), this, SIGNAL( copyAvailable(bool) ) );
  connect( ed->textEdit(), SIGNAL( copyAvailable(bool) ), this, SIGNAL( copyAvailable(bool) ) );	
  connect( ed->textEdit(), SIGNAL( undoAvailable(bool) ), this, SIGNAL( undoAvailable(bool) ) );	
  connect( ed->textEdit(), SIGNAL( redoAvailable(bool) ), this, SIGNAL( redoAvailable(bool) ) );	
}

Editor * TabEditor::currentEditor() const {
  return (Editor*)currentWidget();	
}

Editor * TabEditor::editor(int index) const {
  return (Editor*)widget( index );
}
  
QString TabEditor::strippedName(const QString &fullFileName) {
  return QFileInfo(fullFileName).fileName();
}

void TabEditor::dragEnterEvent(QDragEnterEvent *event) {
  const QMimeData *mimeData = event->mimeData();
  if (mimeData->hasUrls()) {
    setBackgroundRole(QPalette::Highlight);
    event->acceptProposedAction();
  }
}
void TabEditor::dropEvent(QDropEvent *event) {
  const QMimeData *mimeData = event->mimeData();
  const QList<QUrl> & urls = mimeData->urls();
  if (mimeData->hasUrls()) {
    for(int i = 0; i < urls.size(); i++) {
      if((!urls.at(i).toLocalFile().isEmpty()))
        loadTab( urls.at(i).toLocalFile() );
   	}
	 
    setBackgroundRole(QPalette::NoRole);
    event->acceptProposedAction(); 
  }
}

