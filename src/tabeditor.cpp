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

// Xinx header
#include "tabeditor.h"
#include "editor.h"
#include "texteditor.h"
#include "fileeditor.h"
#include "webserviceseditor.h"
#include "xslproject.h"
#include "runsnipetdialogimpl.h"
#include "globals.h"
#include "snipet.h"
#include "snipetlist.h"

// Qt header
#include <QDir>
#include <QTabBar>
#include <QDragEnterEvent>
#include <QUrl>
#include <QMenu>
#include <QMessageBox>

// Std header
#include <typeinfo>

//
TabEditor::TabEditor( QWidget * parent ) : QTabWidget( parent ), m_refreshAction(0), m_saveAction(0), m_saveAsAction(0), m_closeAction(0), 
	m_clickedItem( -1 ), m_previous(NULL) {
	setAcceptDrops(true);

    tabBar()->installEventFilter(this);
       
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}
//
TabEditor::~TabEditor() {
}
//

bool TabEditor::isFileEditor( Editor * editor ) {
	return dynamic_cast<FileEditor*>(editor) != NULL;
}

Editor * TabEditor::currentEditor() {
	return dynamic_cast<Editor*>( currentWidget() );
}

Editor * TabEditor::editor( int index ) {
	return dynamic_cast<Editor*>( widget( index ) );
}

Editor * TabEditor::editor( const QString & filename ) {
	for( int i = 0; i < count(); i++ ) {
		if( ( isFileEditor( editor( i ) ) ) && ( static_cast<FileEditor*>( editor(i) )->getFileName() == QDir::fromNativeSeparators( filename ) ) ) 
			return editor(i);
	}
	return NULL;
}

void TabEditor::newFileEditor( Editor * editor ) {
	QString title = editor->getTitle();
	if( editor->isModified() )
		title += "*";
		
	int index = addTab( editor, title );
	setTabIcon( index, editor->icon() );
	setCurrentIndex( index );
	emit currentChanged( currentIndex() );  
}

Editor * TabEditor::newFileEditor( const QString & fileName ) {
	Editor * ed;
	if( QDir::match( "*.fws", fileName ) ) {
		ed = new WebServicesEditor( this );
	} else {
		ed = new FileEditor( this );	
	}
	newFileEditor( ed );
	return ed;
}

Editor * TabEditor::newFileEditorTxt( const QString & suffix ) {
	Editor * editor = new FileEditor( this, suffix );
	newFileEditor( editor );
	return editor;
}

Editor * TabEditor::newFileEditorWS() {
	Editor * editor = new WebServicesEditor( this );
	newFileEditor( editor );
	return editor;
}

Editor * TabEditor::loadFileEditor( const QString & fileName ) {
	Editor * ed = editor( fileName );
	if( ! ed ) {
		ed = newFileEditor( fileName );	
		
		static_cast<FileEditor*>( ed )->loadFile( fileName );
		
		setTabText( currentIndex(), ed->getTitle() );
	}
	setCurrentWidget( ed );
	dynamic_cast<QWidget*>( parent() )->activateWindow();
	emit currentChanged( currentIndex() );  
	return ed;
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

void TabEditor::indent() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->indent();
	}
}

void TabEditor::unindent() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->indent( true );
	}
}

void TabEditor::autoindent() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->autoIndent();
	}
}


void TabEditor::complete() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->complete();
	}
}

void TabEditor::highlightWord() {
	if( currentEditor() && isFileEditor( currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( currentEditor() );
		editor->callTextHighlighter();
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
	emit fileDragged();
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
		emit modelChanged( NULL );
	}
	
	if( m_previous ) {
		m_previous->disconnect( this );
		if( isFileEditor( m_previous ) )
			qobject_cast<FileEditor*>( m_previous )->textEdit()->disconnect( this );
		m_previous = NULL;
	}
}

void TabEditor::slotModifiedChange( bool changed ) {
	if( changed ) 
		setTabText ( currentIndex(), tr("%1*").arg( currentEditor()->getTitle() ) );
	else
		setTabText ( currentIndex(), tr("%1").arg( currentEditor()->getTitle() ) );
}

void TabEditor::slotCursorPositionChanged() {
	Editor * editor = currentEditor();
	emit setEditorPosition( qobject_cast<FileEditor*>( editor )->textEdit()->currentRow(), qobject_cast<FileEditor*>( editor )->textEdit()->currentColumn() );
}

void TabEditor::slotCurrentTabChanged( int index ) {
	Q_UNUSED( index );
	
	if( m_previous ) {
		m_previous->disconnect( this );
		if( isFileEditor( m_previous ) )
			qobject_cast<FileEditor*>( m_previous )->textEdit()->disconnect( this );
	}
	
	Editor * editor = currentEditor();
	m_previous = editor;
	
	emit copyAvailable( editor->canCopy() );
	emit pasteAvailable( editor->canPaste() );
	emit undoAvailable( editor->canUndo() );
	emit redoAvailable( editor->canRedo() );
	
	connect( editor, SIGNAL(modificationChanged(bool)), this, SLOT(slotModifiedChange(bool)) );
	connect( editor, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)) );
	connect( editor, SIGNAL(pasteAvailable(bool)), this, SIGNAL(pasteAvailable(bool)) );	
	connect( editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)) );	
	connect( editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)) );	

	connect( editor, SIGNAL(modelUpdated(QAbstractItemModel*)), this, SIGNAL(modelChanged(QAbstractItemModel*)) );
	
	if( isFileEditor( editor ) ) {
		emit textAvailable( true );
		emit setEditorPosition( qobject_cast<FileEditor*>( editor )->textEdit()->currentRow(), qobject_cast<FileEditor*>( editor )->textEdit()->currentColumn() );
		
		connect( editor, SIGNAL( selectionAvailable(bool) ), this, SIGNAL( hasTextSelection(bool) ) );
		connect( qobject_cast<FileEditor*>( editor )->textEdit(), SIGNAL( cursorPositionChanged() ), this, SLOT( slotCursorPositionChanged() ) );
		connect( qobject_cast<FileEditor*>( editor )->textEdit(), SIGNAL( needInsertSnipet(QString) ), this, SLOT( slotNeedInsertSnipet(QString) ) );
		
	} else {
		emit textAvailable( false );
		emit hasTextSelection( false );
	}
	emit modelChanged( editor->model() );
}

void TabEditor::slotNeedInsertSnipet( const QString & snipet ) {
	Snipet * s = global.m_snipetList->indexOf( snipet );
	if( ! s ) 
		return ;
	RunSnipetDialogImpl dlg( s );
	if( ( s->params().count() == 0 ) || dlg.exec() ) {
		TextEditor * textEdit = qobject_cast<TextEditor	*>( sender() );
		textEdit->insertText( dlg.getResult() );
	}		
}

int TabEditor::getClickedTab() {
	int item = m_clickedItem;
	m_clickedItem = -1;
	return item;
}

void TabEditor::setRefreshAction( QAction * action ) {
	m_refreshAction = action;
}

void TabEditor::setSaveAction( QAction * action ) {
	m_saveAction = action;
}

void TabEditor::setSaveAsAction( QAction * action ) {
	m_saveAsAction = action;
}

void TabEditor::setCloseAction( QAction * action ) {
	m_closeAction = action;
}

void TabEditor::setCopyFileNameAction( QAction * action ) {
	m_copyFilenameAction = action;
}

void TabEditor::setCopyPathAction( QAction * action ) {
	m_copyPathAction = action;
}


bool TabEditor::eventFilter( QObject *obj, QEvent *event ) {
	if ( obj==tabBar() ) {
		if ( ( ( event->type() == QEvent::MouseButtonPress ) && ( static_cast<QMouseEvent *>(event)->button() == Qt::RightButton ) ) || ( event->type() == QEvent::MouseButtonDblClick ) ) {
			m_clickedItem = -1;
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			for (int i=0; i<tabBar()->count(); i++) {
				if ( tabBar()->tabRect(i).contains( mouseEvent->pos() ) ) {
					m_clickedItem = i;
					break;
				}
			}
			if( m_clickedItem == -1 ) return QTabWidget::eventFilter( obj, event );
			
			if ( ( event->type() == QEvent::MouseButtonPress ) && ( mouseEvent->button() == Qt::RightButton ) ) {
				QMenu *menu = new QMenu( this );
				menu->addAction( m_refreshAction );
				menu->addSeparator();
				menu->addAction( m_saveAction );
				menu->addAction( m_saveAsAction );
				menu->addSeparator();
				menu->addAction( m_closeAction );
				menu->addSeparator();
				menu->addAction( m_copyFilenameAction );
				menu->addAction( m_copyPathAction );
				menu->exec(mouseEvent->globalPos());
				delete menu;
			} else
			if ( ( event->type() == QEvent::MouseButtonDblClick ) && ( mouseEvent->button() == Qt::LeftButton ) ) {
				m_closeAction->trigger();	
			}
			return true;
		}
	}
	return QTabWidget::eventFilter( obj, event );
}

void TabEditor::bookmark() {
	currentEditor()->toogledBookmark();
}

void TabEditor::nextBookmark() {
	if( ( ! currentEditor()->nextBookmark() ) && ( currentIndex() < count() ) ) {
		int index = currentIndex();
		do { index ++ ; } while( ( index < count() ) && ( editor( index )->bookmarkCount() == 0 ) );
		if( index < count() ) {
			setCurrentIndex( index );
			currentEditor()->firstBookmark();
		}
	}
}

void TabEditor::previousBookmark() {
	if( ( ! currentEditor()->previousBookmark() ) && ( currentIndex() > 0 ) ) {
		int index = currentIndex();
		do { index -- ; } while( ( index >= 0 ) && ( editor( index )->bookmarkCount() == 0 ) );
		if( index >= 0 ) {
			setCurrentIndex( index );
			currentEditor()->lastBookmark();
		}
	}
}

void TabEditor::clearAllBookmark() {
	for( int i = 0; i < count() ; i++ ) {
		editor( i )->clearAllBookmark();
	}
}

