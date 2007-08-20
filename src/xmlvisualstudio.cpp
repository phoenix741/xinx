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
#include "snipetdialog.h"
#include "runsnipetdialogimpl.h"

#include "xmlvisualstudio.h"

#include "aboutdialogimpl.h"

#include "xinxconfig.h"
#include "snipetlist.h"

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

	global.m_completionContents   = new Completion();
	global.m_snipetList = new SnipetList();

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
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(newFile()), this, SLOT(on_m_newAct_triggered()) );
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(openFile(QString)), this, SLOT(open(QString)) );
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(saveAllFile()), this, SLOT(on_m_saveAllAct_triggered()) );
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(closeAllFile()), this, SLOT(on_m_closeAllAct_triggered()) );
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(openProject(QString)), this, SLOT(openProject(QString)) );
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(closeProject()), this, SLOT(on_m_closeProjectAct_triggered()) );
	
	connect( m_tabEditors, SIGNAL(fileDragged()), this, SLOT(updateActions()) );
	connect( m_tabEditors, SIGNAL(closeTab(int)), this, SLOT(slotCloseFile(int)) );
	connect( m_tabEditors, SIGNAL(refreshTab(int)), this, SLOT(slotRefreshFile(int)) );
	connect( m_tabEditors, SIGNAL(saveTab(int)), this, SLOT(saveEditor(int)) );
	connect( m_tabEditors, SIGNAL(saveAsTab(int)), this, SLOT(saveEditorAs(int)) );

	connect( global.m_snipetList, SIGNAL(listChanged()), this, SLOT(refreshSnipetMenu()) );
}

void XMLVisualStudio::createDockWindows() {
	QAction * action = m_xslContentDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+1") );
	m_windowsMenu->addAction( action ); 
	action = m_projectDirectoryDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+2") );
	m_windowsMenu->addAction( action ); 
	action = m_snipetDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+3") );
	m_windowsMenu->addAction( action ); 
	connect( m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}

void XMLVisualStudio::readSettings() {
	resize( global.m_xinxConfig->size() );
	move( global.m_xinxConfig->position() );
  
	if( ! global.m_xinxConfig->storedMainWindowState().isEmpty() ) {
		restoreState( global.m_xinxConfig->storedMainWindowState() );
	}

	try {
		global.m_completionContents->setPath( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "completion.xnx" ) );
	} catch( ENotCompletionFile ) {
		// TODO
	}
	try {
		global.m_snipetList->loadFromFile( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "template.xnx" ) );
		refreshSnipetMenu();
	} catch( SnipetListException ) {
		// TODO
	}
}

void XMLVisualStudio::writeSettings() {
	try {
		global.m_snipetList->saveToFile( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "template.xnx" ) );
	} catch( SnipetListException ) {
		// TODO
	}

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
	m_createTemplate->setEnabled( m_tabEditors->count() );
	foreach( QAction * act, m_snipetActs ) {
		act->setEnabled( m_tabEditors->count() );
	}
	
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

void XMLVisualStudio::on_m_createTemplate_triggered() {
	assert( m_tabEditors->currentEditor() != NULL );
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( m_tabEditors->currentEditor() );
		QString selectedText = editor->textEdit()->textCursor().selectedText();
		
		Snipet * newSnipet;
		SnipetDialogImpl dlg( selectedText );
		if( dlg.exec() == QDialog::Accepted ) {
			newSnipet = dlg.getSnipet();
			global.m_snipetList->add( newSnipet );
		}
		
	}
}

Q_DECLARE_METATYPE(Snipet*);

void XMLVisualStudio::refreshSnipetMenu() {
	qDeleteAllLater( m_snipetActs ); m_snipetActs.clear();
	qDeleteAllLater( m_snipetCategoryActs.values() ); m_snipetCategoryActs.clear();
	if( global.m_snipetList->count() > 0 ) {
		foreach( QString category, global.m_snipetList->categories() ) {
			QAction * act = new QAction( category, this );
			m_snipetCategoryActs[ category ] = act;
			act->setMenu( new QMenu( this ) );
		}
		for( int i = 0 ; i < global.m_snipetList->count() ; i++ ) {
			Snipet * snipet = global.m_snipetList->at( i );
			QAction * act = new QAction( QIcon( snipet->icon() ), snipet->name(), this );
			m_snipetActs.append( act );
			m_snipetCategoryActs[ snipet->category() ]->menu()->addAction( act );
			act->setData( QVariant::fromValue( snipet ) );
			connect( act, SIGNAL(triggered()), this, SLOT(callSnipetMenu()) );
		}
		m_toolsMenu->insertActions( m_createTemplate, m_snipetCategoryActs.values() );
	}
}

void XMLVisualStudio::callSnipetMenu() {
	Q_ASSERT( m_tabEditors->currentEditor() != NULL );

	QAction * action = qobject_cast<QAction*>( sender() );
	if( action && TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		Snipet * snipet = action->data().value<Snipet*>();
		
		RunSnipetDialogImpl dlg( snipet );
		if( dlg.exec() ) {
			TextEditor * editor = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
			editor->insertText( dlg.getResult() );
		}		
	}
}

/****************************************************************************************/
/* PROJECT */

#include <QDir>
#include <QDirModel>
#include <QAction>
#include <QFileDialog>
#include <QTimer>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

#include <assert.h>

#include "globals.h"
#include "xmlvisualstudio.h" 
#include "xinxconfig.h"
#include "xslproject.h"
#include "projectpropertyimpl.h"
#include "editor.h"

#include "rcs_cvs.h"
#include "rcslogdialogimpl.h"

#include "commitmessagedialogimpl.h"

#include "flattreeview.h"
#include "dirrcsmodel.h"
#include "iconprojectprovider.h"

/* XMLVisualStudio */

bool XMLVisualStudio::eventFilter( QObject *obj, QEvent *event ) {
	if ( ( obj == m_projectDirectoryTreeView ) && ( event->type() == QEvent::ContextMenu ) ) {
		QMenu *menu = new QMenu( m_projectDirectoryTreeView );
		menu->addAction( m_selectedUpdateFromRCSAct );
		menu->addAction( m_selectedCommitToRCSAct );
		menu->addAction( m_selectedAddToRCSAct );
		menu->addAction( m_selectedRemoveFromRCSAct );
		menu->exec( static_cast<QContextMenuEvent*>(event)->globalPos() );
		delete menu;
	}
	return QObject::eventFilter( obj, event );
}

void XMLVisualStudio::rcsLogTerminated() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		rcs->disconnect();
	}
}

void XMLVisualStudio::createProjectPart() {
	m_lastProjectOpenedPlace = QDir::currentPath();
	m_dirModel = NULL;
	m_flatModel = NULL;
	m_modelTimer = new QTimer( this );
	m_modelTimer->setInterval( 500 );
	connect( m_modelTimer, SIGNAL(timeout()), this, SLOT(filtreChange()) );
	m_projectDirectoryTreeView->header()->hide();
//	m_projectDirectoryTreeView->header()->setResizeMode( QHeaderView::Fixed );
//	m_projectDirectoryTreeView->header()->resizeSection( 0, 1024 );
	m_projectDirectoryTreeView->installEventFilter( this );

	m_rcslogDialog = new RCSLogDialogImpl( this );
	m_updateProjectBtn->setDefaultAction( m_globalUpdateFromRCSAct );
	m_commitProjectBtn->setDefaultAction( m_globalCommitToRCSAct );
	m_flatListBtn->setDefaultAction( m_toggledFlatView );
	
	connect( m_newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()) );
	connect( m_openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()) );
	connect( m_projectPropertyAct, SIGNAL(triggered()), this, SLOT(changeProjectProperty()) );
}

void XMLVisualStudio::setupRecentProjectMenu( QMenu * menu ) {
	
	m_recentSeparator = menu->addSeparator();
	for(int i = 0; i < MAXRECENTFILES; i++) {
		m_recentProjectActs[i] = new QAction( this );
		m_recentProjectActs[i]->setVisible( false );
		menu->addAction( m_recentProjectActs[i] );
		connect( m_recentProjectActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()) );
	}
	
	updateRecentProjects();
}

void XMLVisualStudio::setupRecentFileMenu( QMenu * menu ) {
	m_recentFileSeparator = menu->addSeparator();
	for(int i = 0; i < MAXRECENTFILES; i++) {
		m_recentFileActs[i] = new QAction( this );
		m_recentFileActs[i]->setVisible( false );
		menu->addAction( m_recentFileActs[i] );
		connect( m_recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()) );
	}
	
	updateRecentFiles();	
}

void XMLVisualStudio::newProject() {
	ProjectPropertyImpl property ( this );
	if( ! property.exec() ) return;
		
	XSLProject* project = new XSLProject( );
	property.saveToProject( project );

	QString filename = QFileDialog::getSaveFileName( this, tr("Save a project"), project->projectPath(), "Projet (*.prj)" );
	if( filename.isEmpty() ) {
		delete project;
		return;
	}
	m_lastProjectOpenedPlace = project->projectPath();

	closeProject( true, global.m_xinxConfig->saveSessionByDefault() );
	project->saveToFile( filename );
	delete project;
	
	openProject( filename );
}

void XMLVisualStudio::openRecentProject() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		openProject( action->data().toString() );	
}

void XMLVisualStudio::openRecentFile() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		open( action->data().toString() );	
}

void XMLVisualStudio::openProject() {
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open a project"), global.m_xinxConfig->xinxProjectPath(), "Projet (*.prj)" );
	if( ! fileName.isEmpty() )
		openProject( fileName );	
}

void XMLVisualStudio::openProject( const QString & filename ) {
	assert( ! filename.isEmpty() );

	if( global.m_project ) 
		closeProject( true, global.m_xinxConfig->saveSessionByDefault() ); 
	else 
		on_m_closeAllAct_triggered();
		
	global.m_xinxConfig->recentProjectFiles().removeAll( filename );

	try {
		global.m_project      	 = new XSLProject( filename );
		m_lastProjectOpenedPlace = QFileInfo( filename ).absolutePath();
		m_lastPlace              = global.m_project->projectPath();

		if( global.m_project->projectType() == XSLProject::SERVICES )
			setWebServicesView( true );

		global.m_xinxConfig->recentProjectFiles().prepend( filename );
     
		while( global.m_xinxConfig->recentProjectFiles().size() > MAXRECENTFILES )
			global.m_xinxConfig->recentProjectFiles().removeLast();

		m_dirModel = new DirRCSModel( DEFAULT_PROJECT_FILTRE, DEFAULT_PROJECT_FILTRE_OPTIONS, QDir::DirsFirst, m_projectDirectoryTreeView );
		m_iconProvider = new IconProjectProvider();
		m_dirModel->setIconProvider( m_iconProvider );

		m_projectDirectoryTreeView->setModel( m_dirModel );
		m_projectDirectoryTreeView->header()->setResizeMode( QHeaderView::Fixed );
		m_projectDirectoryTreeView->header()->resizeSection( 0, 1024 );
		for(int i = 2; i < m_dirModel->columnCount(); i++ )
			m_projectDirectoryTreeView->hideColumn( i );
		m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( global.m_project->projectPath() ) );

		m_tabEditors->setUpdatesEnabled( false );
	
		QDomElement element = global.m_project->sessionNode().firstChildElement( "editor" );
		while( ! element.isNull() ) {
			Editor * editor = m_tabEditors->newFileEditor( element.attribute( "filename" ) );
			editor->deserializeEditor( element );
			
			element = element.nextSiblingElement( "editor" );
		}
		m_tabEditors->setUpdatesEnabled( true );

		setCurrentProject( filename );

	} catch( XSLProjectException e ) {
		QMessageBox::warning( this, tr("Can't open project"), e.getMessage() );
	}
	updateActions();
	updateRecentProjects();
	updateRecentFiles();
	global.emitProjectChanged();
}

void XMLVisualStudio::saveProject() {
	assert( global.m_project != NULL );
	
	global.m_project->saveToFile();
}

void XMLVisualStudio::changeProjectProperty() {
	assert( global.m_project != NULL );
	
	ProjectPropertyImpl property ( this );
	property.loadFromProject( global.m_project );
	if( property.exec() ) {
		property.saveToProject( global.m_project );
		global.emitProjectChanged();
		refreshWebServicesList();
		saveProject();
	}
}

void XMLVisualStudio::on_m_closeProjectAct_triggered() {
	closeProject( true, false );
}

void XMLVisualStudio::on_m_closeProjectSessionAct_triggered() {
	closeProject( true, true );
}

void XMLVisualStudio::closeProject( bool closeAll, bool saveSession ) {
	if( ! global.m_project ) return;		
		
	global.m_project->clearSessionNode();
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		QDomElement node = global.m_project->sessionDocument().createElement( "editor" );
		m_tabEditors->editor( i )->serializeEditor( node, saveSession );
		global.m_project->sessionNode().appendChild( node );
	}
	saveProject();

	m_filtreLineEdit->setText( "" );
	m_modelTimer->stop();
	m_toggledFlatView->setChecked( false );

	if( closeAll && ( ! saveSession ) ) on_m_closeAllAct_triggered(); else
	if( closeAll ) {
		for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) {
			Editor * ed = m_tabEditors->editor( i );
			m_tabEditors->removeTab( i );	
			delete ed;
		}
		updateActions();
	}

	m_projectDirectoryTreeView->setModel( NULL );
	delete m_dirModel;
	m_dirModel = NULL;
	setWebServicesView( false );

	delete global.m_project;
	global.m_project = NULL;

	updateActions();
	updateRecentFiles();
	setCurrentProject( "" );
	global.emitProjectChanged();
}

void XMLVisualStudio::updateRecentProjects() {
	int numRecentFiles = qMin( global.m_xinxConfig->recentProjectFiles().size(), MAXRECENTFILES );

	for( int i = 0; i < numRecentFiles; i++ ) {
		QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( global.m_xinxConfig->recentProjectFiles()[i] ).fileName() );
		m_recentProjectActs[i]->setText( text );
		m_recentProjectActs[i]->setData( global.m_xinxConfig->recentProjectFiles()[i] );
		m_recentProjectActs[i]->setVisible( true );
	}
	
	for( int j = numRecentFiles; j < MAXRECENTFILES; j++ )
		m_recentProjectActs[j]->setVisible(false);

	m_recentSeparator->setVisible( numRecentFiles > 0 );
}

void XMLVisualStudio::updateRecentFiles() {
	int numRecentFiles;
	if( global.m_project ) {
		numRecentFiles = qMin( global.m_project->lastOpenedFile().size(), MAXRECENTFILES );

		for( int i = 0; i < numRecentFiles; i++ ) {
			QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( global.m_project->lastOpenedFile()[i] ).fileName() );
			m_recentFileActs[i]->setText( text );
			m_recentFileActs[i]->setData( global.m_project->lastOpenedFile()[i] );
			m_recentFileActs[i]->setVisible( true );
		}
	} else 
		numRecentFiles = 0;
	
	for( int j = numRecentFiles; j < MAXRECENTFILES; j++ )
		m_recentFileActs[j]->setVisible(false);

	m_recentFileSeparator->setVisible( numRecentFiles > 0 );
}

void XMLVisualStudio::filtreChange() {
	Q_ASSERT( m_dirModel );
	m_modelTimer->stop();
	
	QString filtre = m_filtreLineEdit->text();
	if( filtre.isEmpty() ) {
		m_dirModel->setNameFilters( DEFAULT_PROJECT_FILTRE );
		m_toggledFlatView->setChecked( false );
	} else {
		QString extention = QFileInfo( filtre ).suffix();
		QString filename = QFileInfo( filtre ).fileName();
		if( extention.isEmpty() )
			m_dirModel->setNameFilters( 
				QStringList() 
					<< QString("*%1*.xsl").arg( filtre )
					<< QString("*%1*.js").arg( filtre )
					<< QString("*%1*.xml").arg( filtre )
					<< QString("*%1*.fws").arg( filtre )
				);
		else
			m_dirModel->setNameFilters( QStringList() << QString( "*%1*" ).arg( filename ) );
		m_toggledFlatView->setChecked( true );
	}
}

void XMLVisualStudio::on_m_filtreLineEdit_textChanged( QString filtre ) {
	Q_UNUSED( filtre );
	Q_ASSERT( m_dirModel );

	m_modelTimer->stop();
	m_modelTimer->start();
}

void XMLVisualStudio::on_m_projectDirectoryTreeView_doubleClicked( QModelIndex index ) {
	QModelIndex idx = index;
	if( m_flatModel ) 
		idx = qobject_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
	if( ! m_dirModel->fileInfo( idx ).isDir() )
		open( m_dirModel->filePath( idx ) );
}

void XMLVisualStudio::on_m_globalUpdateFromRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();
		rcs->update( QStringList() << global.m_project->projectPath() );
		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_globalCommitToRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		CommitMessageDialogImpl dlg;
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();

		dlg.setFilesOperation( rcs->operations( QStringList() << global.m_project->projectPath() ) );
		if( ! dlg.exec() ) return ;
		QString message = dlg.messages();

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();

		rcs->commit( dlg.filesOperation(), message );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedUpdateFromRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();
		
		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list )
			paths << m_dirModel->filePath( index );
		rcs->update( paths );
		
		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedCommitToRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		CommitMessageDialogImpl dlg;
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();

		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list )
			paths << m_dirModel->filePath( index );

		dlg.setFilesOperation( rcs->operations( paths ) );
		if( ! dlg.exec() ) return ;
		QString message = dlg.messages();

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();

		rcs->commit( dlg.filesOperation(), message );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedAddToRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();

		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list )
			paths << m_dirModel->filePath( index );
		rcs->add( paths );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::addRCSFile( const QString & filename ) {
	if( ! m_dirModel ) return ;
	RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();

	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();
		rcs->add( QStringList() << filename );
		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedRemoveFromRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();
		m_rcslogDialog->show();

		m_rcslogDialog->init();

		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list ) {
			paths << m_dirModel->filePath( index );
			m_dirModel->remove( index );
		}
		rcs->remove( paths );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_toggledFlatView_toggled( bool value ) {
	if( value ) {
		m_flatModel = new FlatModel( m_dirModel, m_dirModel->index( global.m_project->projectPath() ) );
		m_projectDirectoryTreeView->setModel( m_flatModel );
		m_projectDirectoryTreeView->setRootIndex( QModelIndex() );
		for(int i = 1; i < m_flatModel->columnCount(); i++ )
			m_projectDirectoryTreeView->hideColumn( i );
		m_projectDirectoryTreeView->setRootIsDecorated( false );
	} else  {
		m_projectDirectoryTreeView->setModel( m_dirModel );
		m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( global.m_project->projectPath() ) );
		for(int i = 1; i < m_dirModel->columnCount(); i++ )
			m_projectDirectoryTreeView->hideColumn( i );
		m_projectDirectoryTreeView->setRootIsDecorated( true );
		delete m_flatModel;
		m_flatModel = NULL;
	}
}

/****************************************************************************************/
/* SEARCH */

#include <assert.h> 

#include <QTextEdit>
#include <QMessageBox>

#include "xmlvisualstudio.h"
#include "texteditor.h"
#include "fileeditor.h"
 
void XMLVisualStudio::on_m_searchNextAct_triggered() {
	assert( m_tabEditors->currentEditor() );
	
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
		QTextDocument * document = textEdit->document();
	
		bool continuer = true;
	
		while( continuer ) {
			continuer = false;

			m_cursorStart = textEdit->textCursor();
		
			bool selectionOnly = ( m_findOptions.searchExtend == ReplaceDialogImpl::FindOptions::SEARCHSELECTION );
			bool backwardSearch = ( m_findOptions.searchDirection == ReplaceDialogImpl::FindOptions::SEARCHUP ) || m_searchInverse;
			
			if( backwardSearch ) 
				m_cursorStart.setPosition( m_cursorStart.selectionStart() );
			else
				m_cursorStart.setPosition( m_cursorStart.selectionEnd() );
	
			QTextDocument::FindFlags flags;
			if( backwardSearch ) flags ^= QTextDocument::FindBackward;
			if( m_findOptions.matchCase ) flags ^= QTextDocument::FindCaseSensitively;	
			if( m_findOptions.wholeWords ) flags ^= QTextDocument::FindWholeWords;
		
			if( ! m_cursorStart.isNull() ) {
				if( m_findOptions.regularExpression ) {
					m_cursorStart = document->find( QRegExp( m_findExpression ), m_cursorStart, flags );
				} else {
					m_cursorStart = document->find( m_findExpression, m_cursorStart, flags );
				}
			}
	
			if( m_cursorStart.isNull() || ( selectionOnly && ( ( !backwardSearch && m_cursorEnd < m_cursorStart ) || ( backwardSearch && m_cursorStart < m_cursorEnd ) ) ) ) {
				if( m_findOptions.toReplace && m_yesToAllReplace ) {
					QMessageBox::information( this, 
								tr("Search/Replace"), 
								tr("%1 occurences of '%2' replaced.").arg( m_nbFindedText ).arg( m_findExpression ), 
								QMessageBox::Ok);
				} else {
					QMessageBox::StandardButton ret = QMessageBox::warning( this, 
								tr("Search/Replace"), 
								tr("%1 occurences of '%2' %3. Return to the beginning of the document ?").arg( m_nbFindedText ).arg( m_findExpression ).arg( m_findOptions.toReplace ? tr("replaced") : tr("finded") ), 
								QMessageBox::Yes | QMessageBox::No);
							
					if( ret == QMessageBox::Yes ) {
						m_findOptions.searchExtend = ReplaceDialogImpl::FindOptions::SEARCHALL;
						m_cursorStart = textEdit->textCursor();
						if( ! backwardSearch ) 
							m_cursorStart.movePosition( QTextCursor::Start );
						else
							m_cursorStart.movePosition( QTextCursor::End );
						m_cursorEnd = QTextCursor();
						
						continuer = true;
					}
				}
				m_nbFindedText = 0;
			} else {
				m_nbFindedText++;
			
				textEdit->setTextCursor( m_cursorStart );

				if( m_findOptions.toReplace ) {
					QMessageBox::StandardButton ret = QMessageBox::Yes;
			

					if(! m_yesToAllReplace) {
	 					QMessageBox messageBoxQuestion( QMessageBox::Question, tr("Application"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, this );
						messageBoxQuestion.setWindowModality( Qt::NonModal );
						messageBoxQuestion.show();
						while( messageBoxQuestion.isVisible() ) qApp->processEvents();
						ret = messageBoxQuestion.standardButton( messageBoxQuestion.clickedButton() );
					}

					switch(ret) {
					case QMessageBox::Yes: 	
						// Replace chaine
						m_cursorStart.insertText( ReplaceDialogImpl::replaceStr( m_findOptions, m_findExpression, m_replaceExpression, m_cursorStart.selectedText() ) );
		
						continuer = true;
						break;
					case QMessageBox::YesToAll: 	
						m_cursorStart.insertText( ReplaceDialogImpl::replaceStr( m_findOptions, m_findExpression, m_replaceExpression, m_cursorStart.selectedText() ) );
						m_yesToAllReplace = true;
						continuer = true;
						break;
					case QMessageBox::No: 	
						continuer = true;
						break;
					default : // Do nothing else
						;
					}
				}
			}
			if( ! m_cursorStart.isNull() )
				textEdit->setTextCursor( m_cursorStart );
		}
	}
	m_yesToAllReplace = false;
}

void XMLVisualStudio::on_m_searchPreviousAct_triggered() {
	m_searchInverse = true;
	on_m_searchNextAct_triggered();
	m_searchInverse = false;
}

void XMLVisualStudio::findFirst(const QString & chaine, const QString & dest, const struct ReplaceDialogImpl::FindOptions & options) {
	if( ! TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) 
		return; // TODO : Error

	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;
	m_nbFindedText		= 0;
	m_searchInverse		= false;

	bool selectionOnly = ( m_findOptions.searchExtend == ReplaceDialogImpl::FindOptions::SEARCHSELECTION );
	bool backwardSearch = ( m_findOptions.searchDirection == ReplaceDialogImpl::FindOptions::SEARCHUP );

	TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();

	m_cursorStart = textEdit->textCursor();
	m_cursorEnd   = QTextCursor();

	int selectionStart = m_cursorStart.selectionStart(),
	    selectionEnd = m_cursorStart.selectionEnd();

	if( m_findOptions.searchFromStart ) {
		m_cursorStart.movePosition( QTextCursor::Start, QTextCursor::MoveAnchor );
		m_findOptions.searchFromStart = false;
	} else 
	if( selectionOnly && ! backwardSearch ) {
		m_cursorStart.setPosition( selectionStart, QTextCursor::MoveAnchor );
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.setPosition( selectionEnd, QTextCursor::MoveAnchor );
	} else
	if( selectionOnly && backwardSearch ) {
		m_cursorStart.setPosition( selectionEnd, QTextCursor::MoveAnchor );
		m_cursorEnd   = m_cursorStart;
		m_cursorEnd.setPosition( selectionStart, QTextCursor::MoveAnchor );
	} else
	if( backwardSearch ) {
		m_cursorStart.setPosition( selectionStart, QTextCursor::MoveAnchor );
	}
	
	textEdit->setTextCursor( m_cursorStart );
	
	on_m_searchNextAct_triggered();
}

void XMLVisualStudio::on_m_searchAct_triggered() {
	TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(false);
	m_findDialog->exec();
}

void XMLVisualStudio::on_m_replaceAct_triggered() {
	TextEditor * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(true);
	m_findDialog->exec();
}

/****************************************************************************************/
/* WEBSERVICES */

#include <QMessageBox>

#include <assert.h>

#include "globals.h"
#include "xmlvisualstudio.h" 
#include "texteditor.h"
#include "fileeditor.h"
#include "xslproject.h"
#include "webservices.h"
#include "serviceresultdialogimpl.h"
#include "webserviceseditor.h"

void XMLVisualStudio::createWebServicesPart() {
	global.m_webServices = new WebServicesList();	
}

void XMLVisualStudio::setWebServicesView( bool enabled ) {
	if( enabled ) {
		refreshWebServicesList();
	} else {
		qDeleteAll( *(global.m_webServices) );
		global.m_webServices->clear();
	}
		
	m_webServicesMenu->setEnabled( enabled );
	m_refreshWebServicesListAct->setEnabled( enabled );
	m_callWebServicesAct->setEnabled( enabled );
}

void XMLVisualStudio::on_m_refreshWebServicesListAct_triggered() {
	refreshWebServicesList();
}

void XMLVisualStudio::refreshWebServicesList() {
	qDeleteAll( *(global.m_webServices) );
	global.m_webServices->clear();

	if( global.m_project ) {
		foreach( QString link, global.m_project->serveurWeb() ) {
			WebServices * ws = new WebServices( link, this );
			global.m_webServices->append( ws );
			ws->askWSDL( this );
			connect( ws, SIGNAL(soapResponse(QHash<QString,QString>,QHash<QString,QString>,QString,QString)), this, SLOT(webServicesReponse(QHash<QString,QString>,QHash<QString,QString>,QString,QString)) );
		}
		global.emitWebServicesChanged();
	}
}

void XMLVisualStudio::webServicesReponse( QHash<QString,QString> query, QHash<QString,QString> response, QString errorCode, QString errorString ) {
	if( ! ( errorString.isEmpty() && errorCode.isEmpty() ) ) {
		QMessageBox::warning( this, tr("WebServices Error"), tr("Web services has error code %1 : %2").arg( errorCode ).arg( errorString ) );
	} else {
		ServiceResultDialogImpl * dlg = new ServiceResultDialogImpl( this );
		dlg->setInputStreamText( query );
		dlg->setOutputStreamText( response );
		dlg->show();
	}
}

void XMLVisualStudio::on_m_callWebServicesAct_triggered() {
	assert( m_tabEditors->currentEditor() != NULL );
	assert( global.m_project );
	
	WebServicesEditor * editor = dynamic_cast<WebServicesEditor*>(m_tabEditors->currentEditor());
	if( editor ) {
		editor->service()->call( editor->operation(), editor->values() );
	}
}

