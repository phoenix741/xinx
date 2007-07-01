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

#include <QDir>
#include <QTabBar>
#include <QDragEnterEvent>
#include <QUrl>
#include <QMenu>

#include <typeinfo>
#include "tabeditor.h"
#include "editor.h"
#include "xmlfileeditor.h"
#include "jsfileeditor.h"
#include "texteditor.h"
#include "fileeditor.h"
#include "webserviceseditor.h"
#include "xslproject.h"

//
TabEditor::TabEditor( QWidget * parent ) : QTabWidget( parent ), previous(NULL) {
	setAcceptDrops(true);
//	setContextMenuPolicy( Qt::CustomContextMenu );
//	setContextMenuPolicy( Qt::ActionsContextMenu );

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

void TabEditor::newFileEditor( Editor * editor, const QString & icon ) {
	int index = addTab( editor, editor->getTitle() );
	
	setTabIcon( index, QIcon(icon) );
	setCurrentIndex( index );

	emit currentChanged( currentIndex() );  
}

Editor * TabEditor::newFileEditor( const QString & fileName ) {
	Editor * ed;
	QString icon;
	if( QDir::match( "*.fws", fileName ) ) {
		ed = new WebServicesEditor( this );
		icon = ":/typefws.png";
	} else
	if( QDir::match( "*.xsl;*.xslt", fileName ) ) {
		ed = new XSLFileEditor( this );
		icon = ":/typexsl.png";
	} else
	if( QDir::match( "*.xml", fileName ) ) {
		ed = new XSLFileEditor( this );
		icon = ":/typexml.png";
	} else
	if( QDir::match( "*.js", fileName ) ) {
		ed = new JSFileEditor( this );
		icon = ":/typejs.png";
	} else {
		ed = new FileEditor( new TextEditor( this ), this );	
		icon = ":/typeunknown.png";
	}
	newFileEditor( ed, icon );
	return ed;
}


Editor * TabEditor::newFileEditorTxt() {
	Editor * editor = new FileEditor( new TextEditor( this ), this );
	newFileEditor( editor );
	return editor;
}

Editor * TabEditor::newFileEditorXSL() {
	Editor * editor = new XSLFileEditor( this );
	newFileEditor( editor, ":/typexsl.png" );
	return editor;
}

Editor * TabEditor::newFileEditorXML() {
	Editor * editor = new XMLFileEditor( this );
	newFileEditor( editor, ":/typexml.png" );
	return editor;
}

Editor * TabEditor::newFileEditorJS() {
	Editor * editor = new JSFileEditor( this );
	newFileEditor( editor, ":/typejs.png" );
	return editor;
}

Editor * TabEditor::newFileEditorWS() {
	Editor * editor = new WebServicesEditor( this );
	newFileEditor( editor, ":/typefws.png" );
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

	connect( editor, SIGNAL( createModel() ), this, SIGNAL( modelCreated() ) );	
	connect( editor, SIGNAL( deleteModel() ), this, SIGNAL( modelDeleted() ) );	
	
	if( isFileEditor( editor ) ) {
		emit textAvailable( true );
		emit setEditorPosition( qobject_cast<FileEditor*>( editor )->textEdit()->currentRow(), qobject_cast<FileEditor*>( editor )->textEdit()->currentColumn() );
		
		connect( editor, SIGNAL( selectionAvailable(bool) ), this, SIGNAL( hasTextSelection(bool) ) );
		connect( qobject_cast<FileEditor*>( editor )->textEdit(), SIGNAL( cursorPositionChanged() ), this, SLOT( slotCursorPositionChanged() ) );
	} else {
		emit textAvailable( false );
		emit hasTextSelection( false );
	}
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
				connect( menu->addAction(QIcon(":/reload.png"), tr("Refresh")), SIGNAL(triggered()), this, SLOT(slotRefreshAsked()) );
				menu->addSeparator();
				connect( menu->addAction(QIcon(":/filesave.png"), tr("Save")), SIGNAL(triggered()), this, SLOT(slotSaveAsked()) );
				connect( menu->addAction(QIcon(":/filesaveas.png"), tr("Save As ....")), SIGNAL(triggered()), this, SLOT(slotSaveAsAsked()) );
				menu->addSeparator();
				connect( menu->addAction(QIcon(":/fileclose.png"), tr("Close")), SIGNAL(triggered()), this, SLOT(slotCloseAsked()) );
				menu->exec(mouseEvent->globalPos());
				delete menu;
			} else
			if ( ( event->type() == QEvent::MouseButtonDblClick ) && ( mouseEvent->button() == Qt::LeftButton ) ) {
				emit closeTab( m_clickedItem );	
			}
			return true;
		}
	}
	return QTabWidget::eventFilter( obj, event );
}

void TabEditor::slotCloseAsked() {
	emit closeTab( m_clickedItem );	
}

void TabEditor::slotRefreshAsked() {
	emit refreshTab( m_clickedItem );	
}

void TabEditor::slotSaveAsked() {
	emit saveTab( m_clickedItem );	
}

void TabEditor::slotSaveAsAsked() {
	emit saveAsTab( m_clickedItem );	
}
