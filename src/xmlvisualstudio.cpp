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
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QPrintDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QCloseEvent>
#include <QToolButton>
#include <assert.h>

#include "rcs.h"

#include "globals.h"
#include "uniqueapplication.h"

#include "xmlvisualstudio.h"
#include "tabeditor.h"
#include "editor.h"
#include "texteditor.h"
#include "fileeditor.h"
#include "editorcompletion.h"
#include "replacedialogimpl.h"
#include "xsllistview.h"
#include "xslproject.h"
#include "customdialogimpl.h"

#include "xmlvisualstudio.h"

#include "aboutdialogimpl.h"

#include "xinxconfig.h"

/* XMLVisualStudio */

XMLVisualStudio::XMLVisualStudio( QWidget * parent, Qt::WFlags f ) : QMainWindow(parent, f) {
	setupUi(this);

	m_xslModel         = NULL;
	m_sortXslModel     = NULL;
	m_recentSeparator  = NULL;
	m_aboutDialog 	   = NULL;
	
	m_cursorStart 	   = QTextCursor();
	m_cursorEnd		   = QTextCursor();

	m_lastPlace        = QDir::currentPath();

	completionContents   = new Completion();

	m_findDialog       = new ReplaceDialogImpl(this);
	connect( m_findDialog, SIGNAL(find(QString, QString, ReplaceDialogImpl::FindOptions)), this, SLOT(findFirst(QString, QString, ReplaceDialogImpl::FindOptions)) );

	createProjectPart();
	createWebServicesPart();
	
	createDockWindows();
	readSettings();
	createActions();
	createToolBars();
	createStatusBar();
	
	// Connection for auto open
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(hasFileToOpen(QString)), this, SLOT(open(QString)) );
	connect( m_tabEditors, SIGNAL(fileDragged()), this, SLOT(updateActions()) );
	connect( m_tabEditors, SIGNAL(closeTab(int)), this, SLOT(slotCloseFile(int)) );
	connect( m_tabEditors, SIGNAL(refreshTab(int)), this, SLOT(slotRefreshFile(int)) );
	connect( m_tabEditors, SIGNAL(saveTab(int)), this, SLOT(saveEditor(int)) );
	connect( m_tabEditors, SIGNAL(saveAsTab(int)), this, SLOT(saveEditorAs(int)) );
}

void XMLVisualStudio::createDockWindows() {
	QAction * action = m_xslContentDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+1") );
	m_windowsMenu->addAction( action ); 
	action = m_projectDirectoryDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+2") );
	m_windowsMenu->addAction( action ); 
	connect( m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}

void XMLVisualStudio::readSettings() {
	resize( global.m_xinxConfig->size() );
	move( global.m_xinxConfig->position() );
  
	if( ! global.m_xinxConfig->storedMainWindowState().isEmpty() ) {
		restoreState( global.m_xinxConfig->storedMainWindowState() );
	}

	completionContents->setPath( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "completion.cpl" ) );
}

void XMLVisualStudio::writeSettings() {
	global.m_xinxConfig->storeMainWindowState( saveState() );
	
	global.m_xinxConfig->setPosition( pos() );
	global.m_xinxConfig->setSize( size() );
	
	global.m_xinxConfig->save();
}

void XMLVisualStudio::createActions() {
	QMenu * newMenu = new QMenu( this );
	newMenu->addAction( m_newStylesheetFileAct );
	newMenu->addAction( m_newXMLFileAct );
	newMenu->addAction( m_newJavascriptFileAct );
	newMenu->addAction( m_newWebServicesFileAct );
	
	m_newAct->setMenu( newMenu );
	QToolButton * btn = qobject_cast<QToolButton*>(m_fileToolBar->widgetForAction( m_newAct ));
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
	
	// Recent project file
	QMenu * recentProjectMenu = new QMenu( this );
	recentProjectMenu->addAction( m_openProjectAct );
	QMenu * recentFileMenu = new QMenu( this );
	recentFileMenu->addAction( m_openAct );
	
	m_recentProjectAct->setMenu( recentProjectMenu );
	connect( m_recentProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	
	btn = qobject_cast<QToolButton*>(m_projectToolBar->widgetForAction( m_recentProjectAct ));
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
	
	m_recentFileAct->setMenu( recentFileMenu );
	connect( m_recentFileAct, SIGNAL(triggered()), this, SLOT(on_m_openAct_triggered()) );

	btn = qobject_cast<QToolButton*>(m_fileToolBar->widgetForAction( m_recentFileAct ));
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );

	setupRecentProjectMenu( recentProjectMenu );
	setupRecentFileMenu( recentFileMenu );

	connect( m_tabEditors, SIGNAL(modelCreated()), this, SLOT(slotModelCreated()) );
	connect( m_tabEditors, SIGNAL(modelDeleted()), this, SLOT(slotModelDeleted()) );
	connect( m_tabEditors, SIGNAL(setEditorPosition(int,int)), this, SLOT(setEditorPosition(int,int)));	
	
	// Undo
	connect(m_undoAct, SIGNAL(triggered()), m_tabEditors, SLOT(undo()));
	m_undoAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(undoAvailable(bool)), m_undoAct, SLOT(setEnabled(bool)));	

	// Redo
	connect(m_redoAct, SIGNAL(triggered()), m_tabEditors, SLOT(redo()));
	m_redoAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(redoAvailable(bool)), m_redoAct, SLOT(setEnabled(bool)));	

	// Cut
	connect(m_cutAct, SIGNAL(triggered()), m_tabEditors, SLOT(cut()));
	m_cutAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_cutAct, SLOT(setEnabled(bool)));
	
	// Copy
	connect(m_copyAct, SIGNAL(triggered()), m_tabEditors, SLOT(copy()));
	m_copyAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_copyAct, SLOT(setEnabled(bool)));	

	// Paste
	connect(m_pasteAct, SIGNAL(triggered()), m_tabEditors, SLOT(paste()));
	m_pasteAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(pasteAvailable(bool)), m_pasteAct, SLOT(setEnabled(bool)));	
  
	// Select All
	connect(m_selectAllAct, SIGNAL(triggered()), m_tabEditors, SLOT(selectAll()));
	m_selectAllAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_selectAllAct, SLOT(setEnabled(bool)));	
  
	// Duplicate Line
	connect(m_duplicateLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(duplicateCurrentLine()));
	m_duplicateLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_duplicateLineAct, SLOT(setEnabled(bool)));	
  
	// Move Line Up
	connect(m_moveUpLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineUp()));
	m_moveUpLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_moveUpLineAct, SLOT(setEnabled(bool)));	
  
	// Move Line Down
	connect(m_moveDownLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineDown()));
	m_moveDownLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_moveDownLineAct, SLOT(setEnabled(bool)));	

	// Upper Case
	connect(m_upperTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(upperSelectedText()));
	m_upperTextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_upperTextAct, SLOT(setEnabled(bool)));	

	// Lower Case
	connect(m_lowerTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(lowerSelectedText()));
	m_lowerTextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_lowerTextAct, SLOT(setEnabled(bool)));	

	// Comment
	connect(m_commentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(commentSelectedText()));
	m_commentLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_commentLineAct, SLOT(setEnabled(bool)));	

	// Uncomment
	connect(m_uncommentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(uncommentSelectedText()));
	m_uncommentLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_uncommentLineAct, SLOT(setEnabled(bool)));	

	// Complete  
	/* Don't need, process by the editor directly */ 
	connect(m_completeAct, SIGNAL(triggered()), m_tabEditors, SLOT(complete()));
	m_completeAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_completeAct, SLOT(setEnabled(bool)));	
	
	// Search 
	m_searchAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_searchAct, SLOT(setEnabled(bool)));	
	
	// Search next/previous
	m_searchNextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_searchNextAct, SLOT(setEnabled(bool)));	
	m_searchPreviousAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_searchPreviousAct, SLOT(setEnabled(bool)));	

	// Replace
	m_replaceAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_replaceAct, SLOT(setEnabled(bool)));	
  
  	// About Qt
	connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	// Previous/Next Tab
	m_previousTabAct->setShortcut( QKeySequence::PreviousChild );
	m_nextTabAct->setShortcut( QKeySequence::NextChild );

	// Indent	
	connect(m_indentAct, SIGNAL(triggered()), m_tabEditors, SLOT(indent()));
	m_indentAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_indentAct, SLOT(setEnabled(bool)));	

	// Unindent	
	connect(m_unindentAct, SIGNAL(triggered()), m_tabEditors, SLOT(unindent()));
	m_unindentAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_unindentAct, SLOT(setEnabled(bool)));	

	updateActions();
	updateRecentFiles();
}

void XMLVisualStudio::updateActions() {
	/* Project action */
	m_saveProjectAct->setEnabled( global.m_project != NULL );
	m_closeProjectAct->setEnabled( global.m_project != NULL );
	m_closeProjectSessionAct->setEnabled( global.m_project != NULL );
	m_projectPropertyAct->setEnabled( global.m_project != NULL );
	
	m_globalUpdateFromRCSAct->setEnabled( (global.m_project != NULL) && (global.m_project->projectRCS() != XSLProject::NORCS) );
	m_globalCommitToRCSAct->setEnabled( (global.m_project != NULL) && (global.m_project->projectRCS() != XSLProject::NORCS) );
	
	m_toggledFlatView->setEnabled( global.m_project != NULL );
	m_filtreLineEdit->setEnabled( global.m_project != NULL );

	/* Files */
	m_saveAct->setEnabled( m_tabEditors->count() );
	m_saveAsAct->setEnabled( m_tabEditors->count() );
	m_saveAllAct->setEnabled( m_tabEditors->count() );
	m_closeAct->setEnabled( m_tabEditors->count() );
	m_closeAllAct->setEnabled( m_tabEditors->count() );
	m_printAct->setEnabled( m_tabEditors->count() );
	m_previousTabAct->setEnabled( m_tabEditors->count() );
	m_nextTabAct->setEnabled( m_tabEditors->count() );
	
	if( m_tabEditors->count() == 0 ) {
		if( m_sortXslModel ) delete m_sortXslModel;
		m_xslModel = NULL;
		m_sortXslModel = NULL;
		m_xslContentTreeView->setModel( NULL );
	}
}

void XMLVisualStudio::createToolBars() {
	QToolButton * closeTab = new QToolButton(m_tabEditors);
	closeTab->setDefaultAction(m_closeAct);
	m_tabEditors->setCornerWidget(closeTab);
}

void XMLVisualStudio::createStatusBar() {
	m_editorPosition = new QLabel( "000x000" );
	statusBar()->addPermanentWidget( m_editorPosition );
	setEditorPosition( 1, 1 );
	
	statusBar()->showMessage(tr("Ready"), 2000);
}

void XMLVisualStudio::setEditorPosition( int line, int column ) {
	m_editorPosition->setText( QString("   %1 x %2   ").arg(line, 3, 10, QLatin1Char('0')).arg(column, 3, 10, QLatin1Char('0')) );
}


/* Actions */

void XMLVisualStudio::on_m_newAct_triggered() {
	if( global.m_project && ( global.m_project->projectType() == XSLProject::SERVICES ) ) 
		on_m_newWebServicesFileAct_triggered();
	else
		on_m_newStylesheetFileAct_triggered();
}


void XMLVisualStudio::on_m_newStylesheetFileAct_triggered() {
	m_tabEditors->newFileEditorXSL();
	updateActions();
}

void XMLVisualStudio::on_m_newXMLFileAct_triggered() {
	m_tabEditors->newFileEditorXML();
	updateActions();
}

void XMLVisualStudio::on_m_newJavascriptFileAct_triggered() {
	m_tabEditors->newFileEditorJS();
	updateActions();
}

void XMLVisualStudio::on_m_newWebServicesFileAct_triggered() {
	if( global.m_webServices->size() == 0 ) {
		QMessageBox::warning( this, tr("WebServices"), tr("No WebServices can be found. Please update WebServices list to continue.") );
		return;
	}
	m_tabEditors->newFileEditorWS();
	updateActions();
}

void XMLVisualStudio::on_m_openAct_triggered() {
	QStringList selectedFiles = QFileDialog::getOpenFileNames( this, tr("Open text file"), m_lastPlace, global.m_xinxConfig->dialogFilters().join(";;") );
	
	m_tabEditors->setUpdatesEnabled( false );
	foreach( QString filename, selectedFiles ) {
		m_lastPlace = QFileInfo( filename ).absolutePath();
		open( filename );
	}
	m_tabEditors->setUpdatesEnabled( true );
}

void XMLVisualStudio::on_m_saveAct_triggered() {
	assert( m_tabEditors->currentIndex() != -1 );
	saveEditor( m_tabEditors->currentIndex() );
}

void XMLVisualStudio::on_m_saveAsAct_triggered() {
	assert( m_tabEditors->currentIndex() != -1 );
	saveEditorAs( m_tabEditors->currentIndex() );
}

void XMLVisualStudio::on_m_saveAllAct_triggered() {
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		saveEditor(i); 	
	}
}

void XMLVisualStudio::on_m_printAct_triggered() {
	assert( m_tabEditors->currentEditor() != NULL );
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( m_tabEditors->currentEditor() );
		QTextDocument *document = editor->textEdit()->document();
		QPrinter printer;

		QPrintDialog *dlg = new QPrintDialog( &printer, this );
		if ( dlg->exec() != QDialog::Accepted ) return;

		document->print( &printer );
  
		delete dlg;

		statusBar()->showMessage( tr("Printed"), 2000 );
	}
}

void XMLVisualStudio::on_m_closeAct_triggered() {
	int index = m_tabEditors->currentIndex();
	
	assert( index >= 0 );

	slotCloseFile( index );
}

void XMLVisualStudio::on_m_closeAllAct_triggered() {
	for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) {
		if ( ! maybeSave( i ) ) {
			return;
		} else {
			Editor * ed = m_tabEditors->editor( i );
			m_tabEditors->removeTab( i );	
			delete ed;
		}
	}
	
	updateActions();
}

void XMLVisualStudio::on_m_aboutAct_triggered() {
	if( ! m_aboutDialog ) 
		m_aboutDialog = new AboutDialogImpl( this );
	m_aboutDialog->exec();
}

void XMLVisualStudio::on_m_xslContentTreeView_doubleClicked( QModelIndex index ) {
	assert( m_tabEditors->currentEditor() != NULL );
	
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		QModelIndex mappingIndex = m_sortXslModel->mapToSource( index );
		
		struct FileContentItemModel::struct_file_content data = m_xslModel->data( mappingIndex, Qt::UserRole ).value<FileContentItemModel::struct_file_content>();
		int line = data.line;

		if( ! data.filename.isEmpty() )
			open( data.filename );
		
		// Deplace to rigth line.
		QTextEdit * ed = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
		QTextCursor cursor = ed->textCursor();
		cursor.movePosition( QTextCursor::Start );
		cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, line - 1 );
		ed->setTextCursor( cursor );
		ed->setFocus( Qt::OtherFocusReason );
	}
}

void XMLVisualStudio::on_m_nextTabAct_triggered() {
	m_tabEditors->setCurrentIndex( ( m_tabEditors->currentIndex() + 1 ) % m_tabEditors->count() );
}

void XMLVisualStudio::on_m_previousTabAct_triggered() {
	m_tabEditors->setCurrentIndex( ( m_tabEditors->currentIndex() - 1 ) % m_tabEditors->count() );
}

void XMLVisualStudio::on_m_customApplicationAct_triggered() {
	CustomDialogImpl * custom = new CustomDialogImpl( this );
	writeSettings();
	custom->loadFromConfig( global.m_xinxConfig );
	if( custom->exec() ) {
		custom->saveToConfig( global.m_xinxConfig );
		global.m_xinxConfig->save();	
		readSettings();	
	}
	
	delete custom;
}

void XMLVisualStudio::closeEvent( QCloseEvent *event ) {
	if( ! ( global.m_project && global.m_xinxConfig->saveSessionByDefault()) ) {
		for( int i = 0; i < m_tabEditors->count(); i++ ) {
			if ( ! maybeSave( i ) ) {
				event->ignore();
				return;
			}
		}
	}
	closeProject( false, global.m_xinxConfig->saveSessionByDefault() );
	
	writeSettings();
	
	event->accept();
}

void XMLVisualStudio::open( const QString & filename ) {
	assert( ! filename.isEmpty() );

	if( global.m_project ) {
		global.m_project->lastOpenedFile().removeAll( filename );
		global.m_project->lastOpenedFile().prepend( filename );
     
		while( global.m_project->lastOpenedFile().size() > MAXRECENTFILES )
			global.m_project->lastOpenedFile().removeLast();
	}

	m_tabEditors->loadFileEditor( filename );
	updateRecentFiles();
	updateActions();
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void XMLVisualStudio::slotModelDeleted() {
	m_xslContentTreeView->setModel( NULL );

	if( m_sortXslModel ) {
		delete m_sortXslModel;
		m_sortXslModel = NULL;
		m_xslModel = NULL;
	}
}

void XMLVisualStudio::slotModelCreated() {
	m_xslModel = m_tabEditors->currentEditor( )->model();
	if( m_xslModel ) {
		m_sortXslModel = new QSortFilterProxyModel( this );
		m_sortXslModel->setSourceModel( m_xslModel );
		m_xslContentTreeView->setModel( m_sortXslModel );
	}
}

void XMLVisualStudio::slotCurrentTabChanged( int tab ) {
	assert( tab < m_tabEditors->count() );
	assert( 0   < m_tabEditors->count() );

	slotModelDeleted();
	slotModelCreated();
}

void XMLVisualStudio::slotCloseFile( int index ) {
	if( maybeSave( index ) ) {
		slotModelDeleted();
		Editor * ed = m_tabEditors->editor( index );
		m_tabEditors->removeTab( index );
		delete ed;
	}
	
	updateActions();
}

void XMLVisualStudio::slotRefreshFile( int index ) {
	assert( index >= 0 );
	
	if( TabEditor::isFileEditor( m_tabEditors->editor( index ) ) ) {
		FileEditor * ed = static_cast<FileEditor*>( m_tabEditors->editor( index ) );
		bool act = ! ed->isModified();
		
		if( ! act ) {
			QMessageBox::StandardButton ret;
			ret = QMessageBox::question(this, tr("Application"),
										tr("The document %1 has been modified.\n"
										"Do you really want refresh this?").arg( ed->getTitle() ),
										QMessageBox::Yes | QMessageBox::No);
			act = ( ret == QMessageBox::Yes );
		}
		if( act ) {
			ed->loadFile();
			ed->setModified( false );
		}
	}
}

bool XMLVisualStudio::maybeSave( int index ) {
	assert( index >= 0 );
	assert( index < m_tabEditors->count() );
	
	if ( m_tabEditors->editor( index )->isModified() ) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("Application"),
									tr("The document %1 has been modified.\n"
									"Do you want to save your changes?").arg( m_tabEditors->editor(index)->getTitle() ),
									QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save) {
			saveEditor(index);
		} else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void XMLVisualStudio::saveEditor( int index ) {
	assert( index >= 0 );
	assert( index < m_tabEditors->count() );
	
	if ( ! m_tabEditors->editor( index )->hasName() ) {
		on_m_saveAsAct_triggered();
	} else {
		QString fileName = dynamic_cast<FileEditor*>( m_tabEditors->editor(index) )->getFileName();
		if( global.m_xinxConfig->isAlertWhenStdFile() && global.m_project && (! QFileInfo( fileName ).fileName().startsWith( global.m_project->specifPrefix().toLower() + "_" ) ) && global.m_xinxConfig->managedFile4Name( fileName ).canBeCustomize ) {
			QMessageBox::StandardButton res = QMessageBox::warning( this, tr("Save standard XSL"), tr("You're being to save standard file, do you whant make it specifique"), QMessageBox::Yes | QMessageBox::No );
			if( res == QMessageBox::Yes )
				saveEditorAs( index );
			else
				dynamic_cast<FileEditor*>( m_tabEditors->editor( index ) )->saveFile();
		} else
			dynamic_cast<FileEditor*>( m_tabEditors->editor( index ) )->saveFile();
	}
	
	m_tabEditors->editor( index )->setModified( false );

	statusBar()->showMessage( tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
}

void XMLVisualStudio::saveEditorAs( int index ) {
	assert( index >= 0 );
	assert( index < m_tabEditors->count() );

	QString fileName    = qobject_cast<FileEditor*>( m_tabEditors->editor(index) )->getFileName(),
			fileSuffix  = qobject_cast<FileEditor*>( m_tabEditors->editor(index) )->getSuffix(),
			specifPath,
			filter = global.m_xinxConfig->dialogFilter( fileSuffix ),
			newFileName;

	struct XINXConfig::managedFile customFile = global.m_xinxConfig->managedFile4Suffix( fileSuffix );

	if( global.m_project ) 
		specifPath = QDir( global.m_project->specifPath() ).absoluteFilePath( customFile.customPath );
	else
	 	specifPath = m_lastPlace;
 	

	if( fileName.isEmpty() ) {
		if( global.m_project ) 
			newFileName = QDir( specifPath ).
				absoluteFilePath( global.m_project->specifPrefix().toLower() + "_" );
		else
			newFileName = specifPath;
	} else {
		bool isCustomized = 
			global.m_project && QFileInfo( fileName ).fileName().startsWith( global.m_project->specifPrefix().toLower() );
		if( global.m_project && customFile.canBeCustomize && (!isCustomized) ) {
			newFileName = QDir( specifPath ).
				absoluteFilePath( global.m_project->specifPrefix().toLower() + "_" + QFileInfo( fileName ).fileName() );
		} else {
			newFileName = fileName;
		}
	}
	
	fileName = QFileDialog::getSaveFileName( this, tr("Save text file"), newFileName, global.m_xinxConfig->dialogFilters().join(";;"), &filter );
	
	if( fileName.isEmpty() ) return ;

	m_lastPlace = QFileInfo( fileName ).absolutePath();

	dynamic_cast<FileEditor*>( m_tabEditors->editor(index) )->saveFile( fileName );
	m_tabEditors->editor( index )->setModified( true );
	m_tabEditors->editor( index )->setModified( false );
	
	if( (global.m_project != NULL) && (global.m_project->projectRCS() != XSLProject::NORCS) ) {
		QMessageBox::StandardButton res = QMessageBox::question( this, tr("Add file"), tr("Do you want add this file to the repository ?"), QMessageBox::Yes | QMessageBox::No );
		if( res == QMessageBox::Yes ) 
			addRCSFile( fileName );
	}

	statusBar()->showMessage(tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
}

void XMLVisualStudio::setCurrentProject( const QString & filename ) {
	Q_UNUSED( filename );
	if( ! global.m_project )
		setWindowTitle( tr("XINX") );
	else {
		setWindowTitle( tr("%1 - %2").arg( global.m_project->projectName() ).arg( tr("XINX") ) );
	}
}






