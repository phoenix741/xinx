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

#include <typeinfo>
#include "tabeditor.h"
#include "editor.h"
#include "fileeditor.h"

#include <QtGui>
#include <QDir>
//
TabEditor::TabEditor( QWidget * parent ) : QTabWidget( parent ), previous(NULL) {
	setAcceptDrops(true);
       
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}
//
TabEditor::~TabEditor() {
}
//

bool TabEditor::isFileEditor( Editor * editor ) {
	return typeid(*editor) == typeid(FileEditor);
}

Editor * TabEditor::currentEditor() {
	return dynamic_cast<Editor*>( currentWidget() );
}

Editor * TabEditor::editor( int index ) {
	return dynamic_cast<Editor*>( widget( index ) );
}

Editor * TabEditor::editor( const QString & filename ) {
	for( int i = 0; i < count(); i++ ) {
		if( ( isFileEditor( editor( i ) ) ) && ( static_cast<FileEditor*>( editor(i) )->getFileName() == filename ) ) 
			return editor(i);
	}
	return NULL;
}

void TabEditor::newFileEditor() {
	FileEditor * editor = new FileEditor( this );
	int index = addTab( editor, editor->getTitle() );
	
	setTabIcon( index, QIcon(":/images/doc.png") );
	setCurrentIndex( index );

	emit currentChanged( currentIndex() );  
}

void TabEditor::loadFileEditor( const QString & fileName, XSLProject * project ) {
	Editor * ed = editor( fileName );
	if( ! ed ) {
		ed = new FileEditor( this, project );
		
		static_cast<FileEditor*>( ed )->loadFile( fileName );
		
		int index = addTab( ed, ed->getTitle() );
		setTabIcon( index, QIcon(":/images/doc.png") );
	}
	setCurrentWidget( ed );
	dynamic_cast<QWidget*>( parent() )->activateWindow();
	emit currentChanged( currentIndex() );  
}

void TabEditor::copy() {
	Editor * editor = currentEditor();
	if( editor && editor->canCopy() ) 
		editor->copy();
}
	
void TabEditor::cut() {
	Editor * editor = currentEditor();
	if( editor && editor->canCopy() ) 
		editor->cut();
}

void TabEditor::paste() {
	Editor * editor = currentEditor();
	if( editor && editor->canPaste() ) 
		editor->paste();
}
  
void TabEditor::undo() {
	Editor * editor = currentEditor();
	if( editor && editor->canUndo() ) 
		editor->undo();	
}
	
void TabEditor::redo() {
	Editor * editor = currentEditor();
	if( editor && editor->canRedo() ) 
		editor->redo();
}
  
void TabEditor::selectAll() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->selectAll();
	}
}
  
void TabEditor::duplicateCurrentLine() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->duplicateCurrentLine();
	}
}

void TabEditor::moveLineUp() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->moveLineUp();
	}
}
	
void TabEditor::moveLineDown() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->moveLineDown();
	}
}

void TabEditor::upperSelectedText() {
	if( currentEditor() && isFileEditor( currentEditor() ) && currentEditor()->canCopy() ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->uploSelectedText();
	}
}
	
void TabEditor::lowerSelectedText() {
	if( currentEditor() && isFileEditor( currentEditor() ) && currentEditor()->canCopy() ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->uploSelectedText( false );
	}
}

void TabEditor::commentSelectedText() {
	if( currentEditor() && isFileEditor( currentEditor() ) && currentEditor()->canCopy() ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->commentSelectedText();
	}
}

void TabEditor::uncommentSelectedText() {
	if( currentEditor() && isFileEditor( currentEditor() ) && currentEditor()->canCopy() ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->commentSelectedText( true );
	}
}

void TabEditor::complete() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->complete();
	}
}

void TabEditor::dragEnterEvent( QDragEnterEvent *event ) {
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasUrls()) {
		setBackgroundRole(QPalette::Highlight);
		event->acceptProposedAction();
	}
}

void TabEditor::dropEvent( QDropEvent *event ) {
	const QMimeData *mimeData = event->mimeData();
	const QList<QUrl> & urls = mimeData->urls();
	if (mimeData->hasUrls()) {
		for(int i = 0; i < urls.size(); i++) {
			if((!urls.at(i).toLocalFile().isEmpty()))
				loadFileEditor( urls.at(i).toLocalFile() );
		}
	 
		setBackgroundRole(QPalette::NoRole);
		event->acceptProposedAction(); 
	}
}

void TabEditor::tabRemoved ( int index ) {
	Q_UNUSED( index );
	
	if( count() == 0 ) {
		emit copyAvailable( false );
		emit pasteAvailable( false );
		emit undoAvailable( false );
		emit redoAvailable( false );
		emit textAvailable( false );
		emit hasTextSelection( false );
	}
}

void TabEditor::slotModifiedChange( bool changed ) {
	if( changed ) 
		setTabText ( currentIndex(), tr("%1*").arg( currentEditor()->getTitle() ) );
	else
		setTabText ( currentIndex(), tr("%1").arg( currentEditor()->getTitle() ) );
}

void TabEditor::slotCurrentTabChanged( int index ) {
	Q_UNUSED( index );
	
	if( previous ) previous->disconnect();
	
	Editor * editor = currentEditor();
	
	emit copyAvailable( editor->canCopy() );
	emit pasteAvailable( editor->canPaste() );
	emit undoAvailable( editor->canUndo() );
	emit redoAvailable( editor->canRedo() );
	
	connect( editor, SIGNAL( modificationChanged(bool) ), this, SLOT( slotModifiedChange(bool) ) );
	connect( editor, SIGNAL( copyAvailable(bool) ), this, SIGNAL( copyAvailable(bool) ) );
	connect( editor, SIGNAL( pasteAvailable(bool) ), this, SIGNAL( pasteAvailable(bool) ) );	
	connect( editor, SIGNAL( undoAvailable(bool) ), this, SIGNAL( undoAvailable(bool) ) );	
	connect( editor, SIGNAL( redoAvailable(bool) ), this, SIGNAL( redoAvailable(bool) ) );	
	
	if( isFileEditor( editor ) ) {
		emit textAvailable( true );
		
		connect( editor, SIGNAL( selectionAvailable(bool) ), this, SIGNAL( hasTextSelection(bool) ) );
	} else {
		emit textAvailable( false );
		emit hasTextSelection( false );
	}
}

