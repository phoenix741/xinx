/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *f
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
#include "mainformimpl.h"
#include "private/p_mainformimpl.h"
#include "xinxconfig.h"
#include "xslproject.h"
#include "snipet.h"
#include "snipetlist.h"
#include "snipetdialog.h"
#include "fileeditor.h"
#include "texteditor.h"
#include "aboutdialogimpl.h"
#include "customdialogimpl.h"
#include "editorcompletion.h"
#include "projectpropertyimpl.h"
#include "webservices.h"
#include "serviceresultdialogimpl.h"
#include "webserviceseditor.h"
#include "rcs.h"
#include "commitmessagedialogimpl.h"
#include "uniqueapplication.h"
#include "runsnipetdialogimpl.h"

// Qt header
#include <QKeySequence>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>

/* PrivateMainformImpl */

PrivateMainformImpl::PrivateMainformImpl( MainformImpl * parent ) : m_lastProjectOpenedPlace( QDir::currentPath() ), m_lastPlace( QDir::currentPath() ), m_parent( parent ) {
	createTabEditorButton();
	createSubMenu();
	createDockWidget();
	createStatusBar();
	createShortcut();
	createActions();
	createFindReplace();
	createDialogs();
	createSnipet();
	connectDbus();
	updateActions();
	updateRecentFiles();
	updateRecentProjects();
}

PrivateMainformImpl::~PrivateMainformImpl() {
	
}

void PrivateMainformImpl::createDockWidget() {
	m_contentDock = new FileContentDockWidget( tr("File Content"), m_parent );
	m_contentDock->setObjectName( "m_contentDock" );
	m_parent->addDockWidget( Qt::LeftDockWidgetArea, m_contentDock );
	QAction * action = m_contentDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+1") );
	m_parent->m_windowsMenu->addAction( action ); 
	
	m_projectDock = new ProjectDirectoryDockWidget( tr("Project Directory"), m_parent );
	m_projectDock->setObjectName( "m_projectDock" );
	m_projectDock->setGlobalUpdateAction( m_parent->m_globalUpdateFromRCSAct );
	m_projectDock->setGlobalCommitAction( m_parent->m_globalCommitToRCSAct );
	m_projectDock->setSelectedUpdateAction( m_parent->m_selectedUpdateFromRCSAct );
	m_projectDock->setSelectedCommitAction( m_parent->m_selectedCommitToRCSAct );
	m_projectDock->setSelectedAddAction( m_parent->m_selectedAddToRCSAct );
	m_projectDock->setSelectedRemoveAction( m_parent->m_selectedRemoveFromRCSAct );
	m_projectDock->setToggledViewAction( m_parent->m_toggledFlatView );
	m_parent->addDockWidget( Qt::LeftDockWidgetArea, m_projectDock );
	action = m_projectDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+2") );
	m_parent->m_windowsMenu->addAction( action ); 
}

void PrivateMainformImpl::createSubMenu() {
	// New sub menu
	QMenu * newMenu = new QMenu( m_parent );
	newMenu->addAction( m_parent->m_newStylesheetFileAct );
	newMenu->addAction( m_parent->m_newXMLFileAct );
	newMenu->addAction( m_parent->m_newJavascriptFileAct );
	newMenu->addAction( m_parent->m_newWebServicesFileAct );
	
	m_parent->m_newAct->setMenu( newMenu );
	QToolButton * btn = qobject_cast<QToolButton*>( m_parent->m_fileToolBar->widgetForAction( m_parent->m_newAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
		
	// Recent project menu
	QMenu * recentProjectMenu = new QMenu( m_parent );
	recentProjectMenu->addAction( m_parent->m_openProjectAct );
	setupRecentMenu( recentProjectMenu, m_recentSeparator, m_recentProjectActs );
	m_parent->m_recentProjectAct->setMenu( recentProjectMenu );
	btn = qobject_cast<QToolButton*>( m_parent->m_projectToolBar->widgetForAction(m_parent-> m_recentProjectAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
	
	// Recent file menu
	QMenu * recentFileMenu = new QMenu( m_parent );
	recentFileMenu->addAction( m_parent->m_openAct );
	setupRecentMenu( recentFileMenu, m_recentFileSeparator, m_recentFileActs );
	m_parent->m_recentFileAct->setMenu( recentFileMenu );
	btn = qobject_cast<QToolButton*>( m_parent->m_fileToolBar->widgetForAction( m_parent->m_recentFileAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
		
	// Close project sub menu
	QMenu * closeProjectMenu = new QMenu( m_parent );
	closeProjectMenu->addAction( m_parent->m_closeProjectWithSessionAct );
	closeProjectMenu->addAction( m_parent->m_closeProjectNoSessionAct );
	
	m_parent->m_closeProjectAct->setMenu( closeProjectMenu );
	btn = qobject_cast<QToolButton*>( m_parent->m_projectToolBar->widgetForAction( m_parent->m_closeProjectAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
}

void PrivateMainformImpl::createActions() {
	/* FILE */

	// New
	connect( m_parent->m_newAct, SIGNAL(triggered()), m_parent, SLOT(newDefaultFile()) );
	connect( m_parent->m_newStylesheetFileAct, SIGNAL(triggered()), m_parent, SLOT(newStylesheetFile()) );
	connect( m_parent->m_newXMLFileAct, SIGNAL(triggered()), m_parent, SLOT(newXmlDataFile()) );
	connect( m_parent->m_newWebServicesFileAct, SIGNAL(triggered()), m_parent, SLOT(newWebservicesFile()) );
	connect( m_parent->m_newJavascriptFileAct, SIGNAL(triggered()), m_parent, SLOT(newJavascriptFile()) );
	
	// Open
	connect( m_parent->m_openAct, SIGNAL(triggered()), this, SLOT(openFile()) );
	connect( m_parent->m_recentFileAct, SIGNAL(triggered()), this, SLOT(openFile()) );
	connect( m_contentDock, SIGNAL(open(QString,int)), this, SLOT(openFile(QString,int)) );
	connect( m_projectDock, SIGNAL(open(QString)), m_parent, SLOT(openFile(QString)) );
	connect( m_parent->m_tabEditors, SIGNAL(modelChanged(QAbstractItemModel*)), m_contentDock, SLOT(updateModel(QAbstractItemModel*)) );

	// Recent open action
	for(int i = 0; i < MAXRECENTFILES; i++) {
		connect( m_recentProjectActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()) );
		connect( m_recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()) );
	}
	
	// Refresh
	m_parent->m_tabEditors->setRefreshAction( m_parent->m_refreshFileAct );
	connect( m_parent->m_refreshFileAct, SIGNAL(triggered()), this, SLOT(refreshFile()) );
		
	// Save
	m_parent->m_tabEditors->setSaveAction( m_parent->m_saveAct );
	m_parent->m_tabEditors->setSaveAsAction( m_parent->m_saveAsAct );
	connect( m_parent->m_saveAct, SIGNAL(triggered()), this, SLOT(saveFile()) );
	connect( m_parent->m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAsFile()) );
	connect( m_parent->m_saveAllAct, SIGNAL(triggered()), m_parent, SLOT(saveAllFile()) );
	
	// Print
	connect( m_parent->m_printAct, SIGNAL(triggered()), this, SLOT(printFile()) );
	
	// Close
	m_parent->m_tabEditors->setCloseAction( m_parent->m_closeAct );
	connect( m_parent->m_closeAct, SIGNAL(triggered()), m_parent, SLOT(closeFile()) );
	connect( m_parent->m_closeAllAct, SIGNAL(triggered()), m_parent, SLOT(closeAllFile()) );
	
	// Model
	connect( m_parent->m_tabEditors, SIGNAL(setEditorPosition(int,int)), m_parent, SLOT(setEditorPosition(int,int)) );	

	/* EDIT */
	// Undo
	connect( m_parent->m_undoAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(undo()) );
	m_parent->m_undoAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(undoAvailable(bool)), m_parent->m_undoAct, SLOT(setEnabled(bool)) );	

	// Redo
	connect( m_parent->m_redoAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(redo()) );
	m_parent->m_redoAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(redoAvailable(bool)), m_parent->m_redoAct, SLOT(setEnabled(bool)) );	

	// Cut
	connect( m_parent->m_cutAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(cut()) );
	m_parent->m_cutAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(copyAvailable(bool)), m_parent->m_cutAct, SLOT(setEnabled(bool)) );
	
	// Copy
	connect( m_parent->m_copyAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(copy()) );
	m_parent->m_copyAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(copyAvailable(bool)), m_parent->m_copyAct, SLOT(setEnabled(bool)) );	

	// Paste
	connect( m_parent->m_pasteAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(paste()) );
	m_parent->m_pasteAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(pasteAvailable(bool)), m_parent->m_pasteAct, SLOT(setEnabled(bool)) );	
  
	// Select All
	connect( m_parent->m_selectAllAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(selectAll()) );
	m_parent->m_selectAllAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_selectAllAct, SLOT(setEnabled(bool)) );	
  
	// Duplicate Line
	connect( m_parent->m_duplicateLineAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(duplicateCurrentLine()) );
	m_parent->m_duplicateLineAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_duplicateLineAct, SLOT(setEnabled(bool)) );
  
	// Move Line Up
	connect( m_parent->m_moveUpLineAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(moveLineUp()) );
	m_parent->m_moveUpLineAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_moveUpLineAct, SLOT(setEnabled(bool)) );
  
	// Move Line Down
	connect( m_parent->m_moveDownLineAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(moveLineDown()) );
	m_parent->m_moveDownLineAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_moveDownLineAct, SLOT(setEnabled(bool)) );	

	// Upper Case
	connect( m_parent->m_upperTextAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(upperSelectedText()) );
	m_parent->m_upperTextAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(hasTextSelection(bool)), m_parent->m_upperTextAct, SLOT(setEnabled(bool)) );

	// Lower Case
	connect( m_parent->m_lowerTextAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(lowerSelectedText()) );
	m_parent->m_lowerTextAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(hasTextSelection(bool)), m_parent->m_lowerTextAct, SLOT(setEnabled(bool)) );

	// Comment
	connect( m_parent->m_commentLineAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(commentSelectedText()) );
	m_parent->m_commentLineAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(hasTextSelection(bool)), m_parent->m_commentLineAct, SLOT(setEnabled(bool)) );

	// Uncomment
	connect( m_parent->m_uncommentLineAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(uncommentSelectedText()) );
	m_parent->m_uncommentLineAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(hasTextSelection(bool)), m_parent->m_uncommentLineAct, SLOT(setEnabled(bool)) );

	// Complete, Don't need, process by the editor directly
	connect( m_parent->m_completeAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(complete()) );
	m_parent->m_completeAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_completeAct, SLOT(setEnabled(bool)) );
	
	// Search 
	connect( m_parent->m_searchAct, SIGNAL(triggered()), this, SLOT(find()) );
	m_parent->m_searchAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_searchAct, SLOT(setEnabled(bool)) );
	
	// Search next/previous
	connect( m_parent->m_searchNextAct, SIGNAL(triggered()), this, SLOT(findNext()) );
	m_parent->m_searchNextAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_searchNextAct, SLOT(setEnabled(bool)));	
	connect( m_parent->m_searchPreviousAct, SIGNAL(triggered()), this, SLOT(findPrevious()) );
	m_parent->m_searchPreviousAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_searchPreviousAct, SLOT(setEnabled(bool)));	

	// Replace
	connect( m_parent->m_replaceAct, SIGNAL(triggered()), this, SLOT(replace()) );
	m_parent->m_replaceAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_replaceAct, SLOT(setEnabled(bool)) );
	
	// Indent	
	connect( m_parent->m_indentAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(indent()) );
	m_parent->m_indentAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_indentAct, SLOT(setEnabled(bool)) );	

	// Unindent	
	connect( m_parent->m_unindentAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(unindent()) );
	m_parent->m_unindentAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_unindentAct, SLOT(setEnabled(bool)) );	
	
	// Autoindent
	connect( m_parent->m_prettyPrintAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(autoindent()) );
	m_parent->m_prettyPrintAct->setEnabled(false);
	connect( m_parent->m_tabEditors, SIGNAL(textAvailable(bool)), m_parent->m_prettyPrintAct, SLOT(setEnabled(bool)) );	
	
	/* PROJECT */
	connect( m_parent->m_globalUpdateFromRCSAct, SIGNAL(triggered()), this, SLOT(globalUpdateFromVersionManager()) );
	connect( m_parent->m_globalCommitToRCSAct, SIGNAL(triggered()), this, SLOT(globalCommitToVersionManager()) );
	connect( m_parent->m_selectedUpdateFromRCSAct, SIGNAL(triggered()), this, SLOT(selectedUpdateFromVersionManager()) );
	connect( m_parent->m_selectedCommitToRCSAct, SIGNAL(triggered()), this, SLOT(selectedCommitToVersionManager()) );
	connect( m_parent->m_selectedAddToRCSAct, SIGNAL(triggered()), this, SLOT(selectedAddToVersionManager()) );
	connect( m_parent->m_selectedRemoveFromRCSAct, SIGNAL(triggered()), this, SLOT(selectedRemoveFromVersionManager()) );

	connect( m_parent->m_newProjectAct, SIGNAL(triggered()), m_parent, SLOT(newProject()) );
	connect( m_parent->m_openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_parent->m_recentProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_parent->m_projectPropertyAct, SIGNAL(triggered()), this, SLOT(projectProperty()) );
	connect( m_parent->m_closeProjectAct, SIGNAL(triggered()), this, SLOT(closeProject()) );
	connect( m_parent->m_closeProjectNoSessionAct, SIGNAL(triggered()), m_parent, SLOT(closeProjectNoSessionData()) );
	connect( m_parent->m_closeProjectWithSessionAct, SIGNAL(triggered()), m_parent, SLOT(closeProjectWithSessionData()) );
	
	/* SERVICES */
	connect( m_parent->m_refreshWebServicesListAct, SIGNAL(triggered()), m_parent, SLOT(updateWebServicesList()) );
	connect( m_parent->m_callWebServicesAct, SIGNAL(triggered()), m_parent, SLOT(callWebservices()) );
	
	/* WINDOWS */
	connect( m_parent->m_toggledFlatView, SIGNAL(toggled(bool)), m_projectDock, SLOT(toggledView(bool)) );
	connect( m_parent->m_nextTabAct, SIGNAL(triggered()), this, SLOT(nextTab()) );
	connect( m_parent->m_previousTabAct, SIGNAL(triggered()), this, SLOT(previousTab()) );
	

	/* Tools */
	// Create Template
	connect( m_parent->m_createTemplate, SIGNAL(triggered()), m_parent, SLOT(newTemplate()) );

	// Customize
	connect( m_parent->m_customApplicationAct, SIGNAL(triggered()), this, SLOT(customize()) );
	  
  	// About Qt
	connect( m_parent->m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	// About
	connect( m_parent->m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	// Drag & Drop
	connect( m_parent->m_tabEditors, SIGNAL(fileDragged()), this, SLOT(updateActions()) );
}

void PrivateMainformImpl::createDialogs() {
	m_rcslogDialog = new RCSLogDialogImpl( m_parent );
}

void PrivateMainformImpl::connectDbus() {
	qobject_cast<UniqueApplication*>(qApp)->attachMainWindow( m_parent );
}

void PrivateMainformImpl::createSnipet() {
	connect( global.m_snipetList, SIGNAL(listChanged()), this, SLOT(updateSnipetMenu()) );
	updateSnipetMenu();
}

void PrivateMainformImpl::callSnipetMenu() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() != NULL );

	QAction * action = qobject_cast<QAction*>( sender() );
	if( action && TabEditor::isFileEditor( m_parent->m_tabEditors->currentEditor() ) ) {
		Snipet * snipet = action->data().value<Snipet*>();
		
		RunSnipetDialogImpl dlg( snipet );
		if( dlg.exec() ) {
			TextEditor * editor = static_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
			editor->insertText( dlg.getResult() );
		}		
	}
}

void PrivateMainformImpl::updateSnipetMenu() {
	qDeleteAllLater( m_snipetActs ); 
	m_snipetActs.clear();
	
	qDeleteAllLater( m_snipetCategoryActs.values() ); 
	m_snipetCategoryActs.clear();
	
	if( global.m_snipetList->count() > 0 ) {
		foreach( QString category, global.m_snipetList->categories() ) {
			QAction * act = new QAction( category, m_parent );
			m_snipetCategoryActs[ category ] = act;
			act->setMenu( new QMenu( m_parent ) );
		}
		for( int i = 0 ; i < global.m_snipetList->count() ; i++ ) {
			Snipet * snipet = global.m_snipetList->at( i );
			QAction * act = new QAction( QIcon( snipet->icon() ), snipet->name(), m_parent );
			m_snipetActs.append( act );
			m_snipetCategoryActs[ snipet->category() ]->menu()->addAction( act );
			act->setData( QVariant::fromValue( snipet ) );
			connect( act, SIGNAL(triggered()), this, SLOT(callSnipetMenu()) );
		}
		m_parent->m_toolsMenu->insertActions( m_parent->m_createTemplate, m_snipetCategoryActs.values() );
	}
}

void PrivateMainformImpl::openRecentProject() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		m_parent->openProject( action->data().toString() );	
}

void PrivateMainformImpl::openRecentFile() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		m_parent->openFile( action->data().toString() );	
}

void PrivateMainformImpl::openFile( const QString & name, int line ) {
	if( !name.isEmpty() )
		m_parent->openFile( name );
	
	// Deplace to rigth line.
	QTextEdit * ed = qobject_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
	QTextCursor cursor = ed->textCursor();
	cursor.movePosition( QTextCursor::Start );
	cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, line - 1 );
	ed->setTextCursor( cursor );
	ed->setFocus( Qt::OtherFocusReason );
}

void PrivateMainformImpl::openFile() {
	Q_ASSERT( global.m_xinxConfig );
	
	QStringList selectedFiles = QFileDialog::getOpenFileNames( m_parent, tr("Open text file"), m_lastPlace, global.m_xinxConfig->dialogFilters().join(";;") );
	
	m_parent->m_tabEditors->setUpdatesEnabled( false );
	foreach( QString filename, selectedFiles ) {
		m_lastPlace = QFileInfo( filename ).absolutePath();
		m_parent->openFile( filename );
	}
	m_parent->m_tabEditors->setUpdatesEnabled( true );
}

void PrivateMainformImpl::fileEditorRefreshFile( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_parent->m_tabEditors->count() );
	Q_ASSERT( TabEditor::isFileEditor( m_parent->m_tabEditors->editor( index ) ) );
	
	if( TabEditor::isFileEditor( m_parent->m_tabEditors->editor( index ) ) ) {
		FileEditor * ed = static_cast<FileEditor*>( m_parent->m_tabEditors->editor( index ) );
		bool act = ! ed->isModified();
		
		if( ! act ) {
			QMessageBox::StandardButton ret;
			ret = QMessageBox::question( m_parent, tr("Application"),
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

void PrivateMainformImpl::refreshFile() {
	int item = m_parent->m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_parent->m_tabEditors->currentIndex();
	fileEditorRefreshFile( item );
}

void PrivateMainformImpl::saveFile() {
	int item = m_parent->m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_parent->m_tabEditors->currentIndex();
	fileEditorSave( item );
}

void PrivateMainformImpl::saveAsFile() {
	int item = m_parent->m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_parent->m_tabEditors->currentIndex();
	fileEditorSaveAs( item );
}

void PrivateMainformImpl::about() {
	AboutDialogImpl about( m_parent );
	about.exec();
}

void PrivateMainformImpl::customize() {
	CustomDialogImpl custom( m_parent );
	custom.loadFromConfig( global.m_xinxConfig );
	
	if( custom.exec() ) {
		custom.saveToConfig( global.m_xinxConfig );
		global.m_xinxConfig->save();	

		try {
			global.m_completionContents->setPath( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "completion.xnx" ) );
		} catch( ENotCompletionFile ) {
			QMessageBox::warning( m_parent, tr("Load completion"), tr("Can't load completion file.") );
		}
		try {
			global.m_snipetList->loadFromFile( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "template.xnx" ) );
		} catch( SnipetListException ) {
			QMessageBox::warning( m_parent, tr("Load snipet"), tr("Can't load snipet file.") );
		}
	}
}

void PrivateMainformImpl::setupRecentMenu( QMenu * menu, QAction * & seperator, QAction * recentActions[ MAXRECENTFILES ] ) {
	seperator = menu->addSeparator();
	seperator->setVisible( false );
	for(int i = 0; i < MAXRECENTFILES; i++) {
		recentActions[i] = new QAction( m_parent );
		recentActions[i]->setVisible( false );
		menu->addAction( recentActions[i] );
	}
}

void PrivateMainformImpl::updateRecentFiles() {
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

void PrivateMainformImpl::updateRecentProjects() {
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

void PrivateMainformImpl::createStatusBar() {
	m_editorPosition = new QLabel( "000x000" );
	m_parent->statusBar()->addPermanentWidget( m_editorPosition );
	m_parent->statusBar()->showMessage( tr("Ready"), 2000 );
}

void PrivateMainformImpl::createShortcut() {
	// File menu
	m_parent->m_newAct->setShortcut( QKeySequence::New );
	m_parent->m_recentFileAct->setShortcut( QKeySequence::Open );
	m_parent->m_saveAct->setShortcut( QKeySequence::Save );
	m_parent->m_closeAct->setShortcut( QKeySequence::Close );
	m_parent->m_printAct->setShortcut( QKeySequence::Print );
	
	m_parent->m_exitAct->setShortcut( QKeySequence::mnemonic( m_parent->m_exitAct->text() ) );
	
	// Edit menu
	m_parent->m_undoAct->setShortcut( QKeySequence::Undo );
	m_parent->m_redoAct->setShortcut( QKeySequence::Redo );
	
	m_parent->m_cutAct->setShortcut( QKeySequence::Cut );
	m_parent->m_copyAct->setShortcut( QKeySequence::Copy );
	m_parent->m_pasteAct->setShortcut( QKeySequence::Paste );
	
	m_parent->m_selectAllAct->setShortcut( QKeySequence::SelectAll );
	
	m_parent->m_upperTextAct->setShortcut( QKeySequence::mnemonic( m_parent->m_upperTextAct->text() ) );
	m_parent->m_lowerTextAct->setShortcut( QKeySequence::mnemonic( m_parent->m_lowerTextAct->text() ) );

	m_parent->m_duplicateLineAct->setShortcut( QKeySequence( "Ctrl+D" ) );
	m_parent->m_indentAct->setShortcut( QKeySequence( "Tab" ) );
	m_parent->m_unindentAct->setShortcut( QKeySequence( "Shift+Tab" ) );
	m_parent->m_commentLineAct->setShortcut( QKeySequence( "Ctrl+Shift+C" ) );
	m_parent->m_uncommentLineAct->setShortcut( QKeySequence( "Ctrl+Shift+D" ) );
	m_parent->m_moveUpLineAct->setShortcut( QKeySequence( "Ctrl+Shift+Up" ) );
	m_parent->m_moveDownLineAct->setShortcut( QKeySequence( "Ctrl+Shift+Down" ) );	
	
	m_parent->m_prettyPrintAct->setShortcut( QKeySequence( "Ctrl+Shift+F" ) );
	m_parent->m_completeAct->setShortcut( QKeySequence( "Ctrl+E" ) );	

	// Search menu
	m_parent->m_searchAct->setShortcut( QKeySequence::Find );
	m_parent->m_searchNextAct->setShortcut( QKeySequence::FindNext );
	m_parent->m_searchPreviousAct->setShortcut( QKeySequence::FindPrevious );
	m_parent->m_replaceAct->setShortcut( QKeySequence::Replace );
	
	// Project menu
	m_parent->m_globalUpdateFromRCSAct->setShortcut( QKeySequence::Refresh );
	m_parent->m_globalCommitToRCSAct->setShortcut( QKeySequence( "F6" ) );
	
	// Webservices menu
	m_parent->m_callWebServicesAct->setShortcut( QKeySequence( "F9" ) );
		
	// Windows menu
	m_parent->m_nextTabAct->setShortcut( QKeySequence::NextChild );
	m_parent->m_previousTabAct->setShortcut( QKeySequence::PreviousChild );
}

void PrivateMainformImpl::readWindowSettings() {
	m_parent->resize( global.m_xinxConfig->size() );
	m_parent->move( global.m_xinxConfig->position() );
  
	if( ! global.m_xinxConfig->storedMainWindowState().isEmpty() ) {
		m_parent->restoreState( global.m_xinxConfig->storedMainWindowState() );
	}
}

void PrivateMainformImpl::storeWindowSettings() {
	global.m_xinxConfig->storeMainWindowState( m_parent->saveState() );
	
	global.m_xinxConfig->setPosition( m_parent->pos() );
	global.m_xinxConfig->setSize( m_parent->size() );
	
	global.m_xinxConfig->save();
}

void PrivateMainformImpl::createTabEditorButton() {
	QToolButton * closeTab = new QToolButton( m_parent->m_tabEditors );
	closeTab->setDefaultAction( m_parent->m_closeAct );
	m_parent->m_tabEditors->setCornerWidget( closeTab );
}

void PrivateMainformImpl::updateActions() {
	/* Project action */
	m_parent->m_saveProjectAct->setEnabled( global.m_project != NULL );
	m_parent->m_closeProjectAct->setEnabled( global.m_project != NULL );
	m_parent->m_closeProjectWithSessionAct->setEnabled( global.m_project != NULL );
	m_parent->m_closeProjectNoSessionAct->setEnabled( global.m_project != NULL );
	m_parent->m_projectPropertyAct->setEnabled( global.m_project != NULL );
	
	m_parent->m_globalUpdateFromRCSAct->setEnabled( (global.m_project != NULL) && (global.m_project->projectRCS() != XSLProject::NORCS) );
	m_parent->m_globalCommitToRCSAct->setEnabled( (global.m_project != NULL) && (global.m_project->projectRCS() != XSLProject::NORCS) );
	
	m_parent->m_toggledFlatView->setEnabled( global.m_project != NULL );
	m_projectDock->setEnabled( global.m_project != NULL );

	/* Files */
	m_parent->m_saveAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_saveAsAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_saveAllAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_closeAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_closeAllAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_printAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_previousTabAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_nextTabAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_createTemplate->setEnabled( m_parent->m_tabEditors->count() );
	foreach( QAction * act, m_snipetActs ) {
		act->setEnabled( m_parent->m_tabEditors->count() );
	}
	
	if( m_parent->m_tabEditors->count() == 0 ) {
		m_contentDock->updateModel( NULL );
	}
}

bool PrivateMainformImpl::fileEditorMayBeSave( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_parent->m_tabEditors->count() );
	Q_ASSERT( TabEditor::isFileEditor( m_parent->m_tabEditors->editor( index ) ) );
	
	if ( m_parent->m_tabEditors->editor( index )->isModified() ) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning( m_parent, tr("Application"),
									tr("The document %1 has been modified.\n"
									"Do you want to save your changes?").arg( m_parent->m_tabEditors->editor(index)->getTitle() ),
									QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if ( ret == QMessageBox::Save ) {
			fileEditorSave( index );
		} else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

QString PrivateMainformImpl::fileEditorCheckPathName( const QString & pathname ) {
	Q_ASSERT( global.m_xinxConfig );
	
	QString prefix = global.m_project ? global.m_project->specifPrefix() + "_" : "" ;
	QString filename = QFileInfo( pathname ).fileName();
	bool hasSpecifiqueName = global.m_project && 
							 ( filename.startsWith( prefix.toLower() ) || filename.startsWith( prefix.toUpper() ) );
	bool canBeCustomize = global.m_xinxConfig->managedFile4Name( filename ).canBeCustomize;
	
	if( global.m_project && global.m_xinxConfig->isAlertWhenStdFile() && canBeCustomize && !hasSpecifiqueName ) {
		QMessageBox::StandardButton res = QMessageBox::warning( m_parent, tr( "Save standard XSL" ), tr( "You're being to save standard file, do you whant make it specifique ?" ), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
		if( res == QMessageBox::Cancel )
			return QString();
		if( res == QMessageBox::Yes )
			return getUserPathName( pathname );
		return pathname;
	} else
		return pathname;
}

QString PrivateMainformImpl::getUserPathName( const QString & pathname, const QString & suffix ) {
	Q_ASSERT( global.m_xinxConfig );
	Q_ASSERT( ! ( pathname.isEmpty() && suffix.isEmpty() ) );
	
	QString fileName    = pathname,
			fileSuffix  = suffix.isEmpty() ? QFileInfo( pathname ).completeSuffix() : suffix,
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
			newFileName = QDir( specifPath ).absoluteFilePath( global.m_project->specifPrefix().toLower() + "_" );
		else
			newFileName = specifPath;
	} else {
		bool isCustomized = 
			global.m_project && (
			QFileInfo( fileName ).fileName().startsWith( global.m_project->specifPrefix().toLower() ) ||
			QFileInfo( fileName ).fileName().startsWith( global.m_project->specifPrefix().toUpper() ) );
			
		if( customFile.canBeCustomize && (!isCustomized) ) {
			newFileName = QDir( specifPath ).
				absoluteFilePath( global.m_project->specifPrefix().toLower() + "_" + QFileInfo( fileName ).fileName() );
		} else {
			newFileName = fileName;
		}
	}
	
	fileName = QFileDialog::getSaveFileName( m_parent, tr("Save text file"), newFileName, global.m_xinxConfig->dialogFilters().join(";;"), &filter );
	
	if( !fileName.isEmpty() ) {
		m_lastPlace = QFileInfo( fileName ).absolutePath();
		if( global.m_project && ( global.m_project->projectRCS() != XSLProject::NORCS ) ) {
			QMessageBox::StandardButton res = QMessageBox::question( m_parent, tr("Add file"), tr("Do you want add file %1 to the repository ?").arg( QFileInfo( fileName ).fileName() ), QMessageBox::Yes | QMessageBox::No );
			if( res == QMessageBox::Yes ) {
				QString backup = fileEditorStandardBackup( pathname, fileName );
				m_fileToAdd.clear();
				if( backup.isEmpty() )
					m_fileToAdd << fileName;
				else
					m_fileToAdd << fileName << backup;
			}
		}
	}
		
	return fileName;
}

QString PrivateMainformImpl::fileEditorStandardBackup( const QString & oldname, const QString & newname ) {
	if( ! global.m_project ) return QString();
		
	QString prefix = global.m_project->specifPrefix() + "_";
	QString oldfilename = QFileInfo( oldname ).fileName();
	QString newfilename = QFileInfo( newname ).fileName();
	bool isOldSpecifiqueFile = oldfilename.startsWith( prefix.toLower() ) || oldfilename.startsWith( prefix.toUpper() );
	bool isNewSpecifiqueFile = newfilename.startsWith( prefix.toLower() ) || newfilename.startsWith( prefix.toUpper() );
	QString specifPath = QDir( global.m_project->specifPath() ).absoluteFilePath( global.m_xinxConfig->managedFile4Name( oldfilename ).customPath );
	QString destname = QDir( specifPath ).absoluteFilePath( oldfilename );

	if( ( ! isOldSpecifiqueFile ) && isNewSpecifiqueFile ) {
		QFile::copy( oldname, destname );
		m_projectDock->refreshPath( specifPath );
		return destname;
	}
	return QString();
}

void PrivateMainformImpl::fileEditorSave( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_parent->m_tabEditors->count() );
	Q_ASSERT( TabEditor::isFileEditor( m_parent->m_tabEditors->editor( index ) ) );
	
	if ( ! m_parent->m_tabEditors->editor( index )->hasName() ) {
		fileEditorSaveAs( index );
	} else {
		QString fileName = fileEditorCheckPathName( qobject_cast<FileEditor*>( m_parent->m_tabEditors->editor(index) )->getFileName() );
		if( ! fileName.isEmpty() ) {
			qobject_cast<FileEditor*>( m_parent->m_tabEditors->editor( index ) )->saveFile( fileName );
			if( m_fileToAdd.count() > 0 ) {
				m_parent->addFilesToVersionManager( m_fileToAdd );
				m_fileToAdd.clear();
			}
			m_parent->statusBar()->showMessage( tr("File %1 saved").arg( m_parent->m_tabEditors->editor(index)->getTitle() ), 2000 );
		}
	}
}

void PrivateMainformImpl::fileEditorSaveAs( int index ) {
	QString fileName = getUserPathName( 
		qobject_cast<FileEditor*>( m_parent->m_tabEditors->editor( index ) )->getFileName(), 
		qobject_cast<FileEditor*>( m_parent->m_tabEditors->editor( index ) )->getSuffix() );

	if( ! fileName.isEmpty() ) {
		qobject_cast<FileEditor*>( m_parent->m_tabEditors->editor( index ) )->saveFile( fileName );
		if( m_fileToAdd.count() > 0 ) {
			m_parent->addFilesToVersionManager( m_fileToAdd );
			m_fileToAdd.clear();
		}
		m_parent->statusBar()->showMessage( tr("File %1 saved").arg( m_parent->m_tabEditors->editor(index)->getTitle() ), 2000 );
	}
}

void PrivateMainformImpl::fileEditorClose( int index ) {
	if( fileEditorMayBeSave( index ) ) {
		m_contentDock->updateModel( NULL );
		Editor * editor = m_parent->m_tabEditors->editor( index );
		m_parent->m_tabEditors->removeTab( index ); 
		delete editor;
	}
	
	updateActions();
}

void PrivateMainformImpl::printFile() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() != NULL );
	
	if( TabEditor::isFileEditor( m_parent->m_tabEditors->currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() );
		QTextDocument *document = editor->textEdit()->document();
		QPrinter printer;

		QPrintDialog *dlg = new QPrintDialog( &printer, m_parent );
		if ( dlg->exec() != QDialog::Accepted ) return;

		document->print( &printer );
  
		delete dlg;

		m_parent->statusBar()->showMessage( tr("Printed"), 2000 );
	}
}

void PrivateMainformImpl::globalUpdateFromVersionManager() {
	m_parent->updateFromVersionManager();
}

void PrivateMainformImpl::globalCommitToVersionManager() {
	m_parent->commitToVersionManager();	
}

void PrivateMainformImpl::selectedUpdateFromVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		m_parent->updateFromVersionManager( list );
}

void PrivateMainformImpl::selectedCommitToVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		m_parent->commitToVersionManager( list );
}

void PrivateMainformImpl::selectedAddToVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		m_parent->addFilesToVersionManager( list );
}

void PrivateMainformImpl::selectedRemoveFromVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		m_parent->removeFilesFromVersionManager( list );
}

void PrivateMainformImpl::nextTab() {
	m_parent->m_tabEditors->setCurrentIndex( ( m_parent->m_tabEditors->currentIndex() + 1 ) % m_parent->m_tabEditors->count() );
}

void PrivateMainformImpl::previousTab() {
	m_parent->m_tabEditors->setCurrentIndex( ( m_parent->m_tabEditors->currentIndex() - 1 + m_parent->m_tabEditors->count() ) % m_parent->m_tabEditors->count() );
}

void PrivateMainformImpl::createFindReplace() {
	m_cursorStart 	   = QTextCursor();
	m_cursorEnd		   = QTextCursor();
	m_findDialog       = new ReplaceDialogImpl( m_parent );
	connect( m_findDialog, SIGNAL(find(QString, QString, ReplaceDialogImpl::FindOptions)), this, SLOT(findFirst(QString, QString, ReplaceDialogImpl::FindOptions)) );
}

void PrivateMainformImpl::findFirst( const QString & chaine, const QString & dest, const struct ReplaceDialogImpl::FindOptions & options ) {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() );
	Q_ASSERT( qobject_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() ) );

	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;
	m_nbFindedText		= 0;
	m_searchInverse		= false;

	bool selectionOnly = ( m_findOptions.searchExtend == ReplaceDialogImpl::FindOptions::SEARCHSELECTION );
	bool backwardSearch = ( m_findOptions.searchDirection == ReplaceDialogImpl::FindOptions::SEARCHUP );

	TextEditor * textEdit = static_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();

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
	
	findNext();
}
	
void PrivateMainformImpl::findNext() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() );
	Q_ASSERT( qobject_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() ) );
	
	TextEditor * textEdit = static_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
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
				QMessageBox::information( m_parent, 
							tr("Search/Replace"), 
							tr("%1 occurences of '%2' replaced.").arg( m_nbFindedText ).arg( m_findExpression ), 
							QMessageBox::Ok);
			} else {
				QMessageBox::StandardButton ret = QMessageBox::warning( m_parent, 
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
 					QMessageBox messageBoxQuestion( QMessageBox::Question, tr("Application"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, m_parent );
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
	m_yesToAllReplace = false;
}

void PrivateMainformImpl::findPrevious() {
	m_searchInverse = true;
	findNext();
	m_searchInverse = false;
}

void PrivateMainformImpl::find() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() );
	Q_ASSERT( qobject_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() ) );
	
	TextEditor * textEdit = qobject_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(false);
	m_findDialog->exec();
}

void PrivateMainformImpl::replace() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() );
	Q_ASSERT( qobject_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() ) );

	TextEditor * textEdit = static_cast<FileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(true);
	m_findDialog->exec();
}

void PrivateMainformImpl::closeProject( bool session ) {
	if( ! global.m_project ) return;		
		
	global.m_project->clearSessionNode();
	for( int i = 0; i < m_parent->m_tabEditors->count(); i++ ) {
		QDomElement node = global.m_project->sessionDocument().createElement( "editor" );
		m_parent->m_tabEditors->editor( i )->serializeEditor( node, session );
		global.m_project->sessionNode().appendChild( node );
	}
	global.m_project->saveToFile();

	m_projectDock->setProjectPath( NULL );

	if( ! session )
		m_parent->closeAllFile(); 
	else {
		m_parent->m_tabEditors->setUpdatesEnabled( false );
		for( int i = m_parent->m_tabEditors->count() - 1; i >= 0; i-- ) {
			Editor * ed = m_parent->m_tabEditors->editor( i );
			m_parent->m_tabEditors->removeTab( i );	
			delete ed;
		}
		m_parent->m_tabEditors->setUpdatesEnabled( true );
	}
	m_contentDock->updateModel( NULL );

	qDeleteAll( *(global.m_webServices) );
	global.m_webServices->clear();
	
	delete global.m_project;
	global.m_project = NULL;

	m_parent->updateWebServicesList();
	updateActions();
	updateRecentFiles();
	updateTitle();
	
	global.emitProjectChanged();
}

void PrivateMainformImpl::updateTitle() {
	if( ! global.m_project )
		m_parent->setWindowTitle( tr("XINX") );
	else {
		m_parent->setWindowTitle( tr("%1 - %2").arg( global.m_project->projectName() ).arg( tr("XINX") ) );
	}
}

void PrivateMainformImpl::openProject() {
	QString fileName = QFileDialog::getOpenFileName( m_parent, tr("Open a project"), global.m_xinxConfig->xinxProjectPath(), "Projet (*.prj)" );
	if( ! fileName.isEmpty() )
		m_parent->openProject( fileName );
}

void PrivateMainformImpl::projectProperty() {
	Q_ASSERT( global.m_project != NULL );
	
	ProjectPropertyImpl property ( m_parent );
	property.loadFromProject( global.m_project );
	
	if( property.exec() ) {
		property.saveToProject( global.m_project );

		m_parent->updateWebServicesList();
		global.m_project->saveToFile();
		
		global.emitProjectChanged();
	}
}

void PrivateMainformImpl::closeProject() {
	closeProject( global.m_xinxConfig->saveSessionByDefault() );
}

void PrivateMainformImpl::webServicesReponse( QHash<QString,QString> query, QHash<QString,QString> response, QString errorCode, QString errorString ) {
	if( ! ( errorString.isEmpty() && errorCode.isEmpty() ) ) {
		QMessageBox::warning( m_parent, tr("WebServices Error"), tr("Web services has error code %1 : %2").arg( errorCode ).arg( errorString ) );
	} else {
		ServiceResultDialogImpl * dlg = new ServiceResultDialogImpl( m_parent );
		dlg->setInputStreamText( query );
		dlg->setOutputStreamText( response );
		dlg->show();
	}
}

void PrivateMainformImpl::rcsLogTerminated() {
	RCS * rcs = m_projectDock->rcs();
	if( rcs )
		rcs->disconnect();
}

/* MainformImpl */

MainformImpl::MainformImpl( QWidget * parent, Qt::WFlags f ) : QMainWindow( parent, f ) {
	setupUi(this);
	d = new PrivateMainformImpl( this );

	// Update the status bar position
	setEditorPosition( 1, 1 );
	
	// Restore windows property
	d->readWindowSettings();
}

MainformImpl::~MainformImpl() {
	delete d;
}

void MainformImpl::setEditorPosition( int line, int column ) {
	d->m_editorPosition->setText( QString("   %1 x %2   ").arg( line, 3, 10, QLatin1Char('0') ).arg( column, 3, 10, QLatin1Char('0') ) );
}

void MainformImpl::closeEvent( QCloseEvent *event ) {
	if( global.m_project ) {
		d->closeProject( global.m_xinxConfig->saveSessionByDefault() );
	} else if( ! closeAllFile() ) {
		event->ignore();
		return;
	}
	d->storeWindowSettings();
	event->accept();
}

void MainformImpl::newStylesheetFile() {
	m_tabEditors->newFileEditorXSL();
	d->updateActions();
}

void MainformImpl::newXmlDataFile() {
	m_tabEditors->newFileEditorXML();
	d->updateActions();
}

void MainformImpl::newJavascriptFile() {
	m_tabEditors->newFileEditorJS();
	d->updateActions();
}

void MainformImpl::newWebservicesFile() {
	if( global.m_webServices->size() == 0 ) {
		QMessageBox::warning( this, tr("WebServices"), tr("No WebServices can be found. Please update WebServices list to continue.") );
		return;
	}
	m_tabEditors->newFileEditorWS();
	d->updateActions();
}

void MainformImpl::newDefaultFile() {
	if( global.m_project && ( global.m_project->projectType() == XSLProject::SERVICES ) ) 
		newWebservicesFile();
	else
		newStylesheetFile();
}

void MainformImpl::newTemplate() {
	Q_ASSERT( m_tabEditors->currentEditor() != NULL );
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( m_tabEditors->currentEditor() );
		QString selectedText = editor->textEdit()->textCursor().selectedText();
		
		Snipet * newSnipet;
		SnipetDialogImpl dlg( selectedText );
		if( dlg.exec() == QDialog::Accepted ) {
			newSnipet = dlg.getSnipet();
			global.m_snipetList->add( newSnipet );
			
			global.m_snipetList->saveToFile();
		}
		
	}
}

void MainformImpl::openFile( const QString & filename ) {
	Q_ASSERT( ! filename.isEmpty() );

	// Add recent action
	if( global.m_project ) {
		global.m_project->lastOpenedFile().removeAll( filename );
		global.m_project->lastOpenedFile().prepend( filename );
     
		while( global.m_project->lastOpenedFile().size() > MAXRECENTFILES )
			global.m_project->lastOpenedFile().removeLast();
	}

	// Load the file in the editor
	m_tabEditors->loadFileEditor( filename );
	d->updateRecentFiles();
	d->updateActions();
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainformImpl::saveFileAs( const QString & name ) {
	qobject_cast<FileEditor*>( m_tabEditors->currentEditor() )->saveFile( name );
}

void MainformImpl::saveAllFile() {
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		d->fileEditorSave( i ); 	
	}
}

void MainformImpl::closeFile() {
	int item = m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_tabEditors->currentIndex();
	d->fileEditorClose( item );
}

bool MainformImpl::closeAllFile() {
	m_tabEditors->setUpdatesEnabled( false );
	for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) {
		if ( d->fileEditorMayBeSave( i ) ) {
			d->m_contentDock->updateModel( NULL );
			Editor * editor = m_tabEditors->editor( i );
			m_tabEditors->removeTab( i ); 
			delete editor;
		} else {
			m_tabEditors->setUpdatesEnabled( true );
			return false;
		}
	}
	m_tabEditors->setUpdatesEnabled( true );
	
	d->updateActions();
	return true;
}

void MainformImpl::newProject() {
	ProjectPropertyImpl property ( this );
	if( ! property.exec() ) return;
		
	XSLProject * project = new XSLProject();
	property.saveToProject( project );

	QString filename = QFileDialog::getSaveFileName( this, tr("Save a project"), project->projectPath(), "Projet (*.prj)" );
	if( filename.isEmpty() ) {
		delete project;
		return;
	}
	d->m_lastProjectOpenedPlace = project->projectPath();

	project->saveToFile( filename );
	delete project;
	
	openProject( filename );
}

void MainformImpl::openProject( const QString & filename ) {
	Q_ASSERT( ! filename.isEmpty() );

	if( global.m_project ) 
		d->closeProject();
	else 
		closeAllFile();
		
	global.m_xinxConfig->recentProjectFiles().removeAll( filename );

	global.m_project = NULL;
	try {
		global.m_project      		= new XSLProject( filename );
		d->m_lastProjectOpenedPlace = QFileInfo( filename ).absolutePath();
		d->m_lastPlace              = global.m_project->projectPath();

		updateWebServicesList();

		global.m_xinxConfig->recentProjectFiles().prepend( filename );
		while( global.m_xinxConfig->recentProjectFiles().size() > MAXRECENTFILES )
			global.m_xinxConfig->recentProjectFiles().removeLast();

		m_tabEditors->setUpdatesEnabled( false );
		QDomElement element = global.m_project->sessionNode().firstChildElement( "editor" );
		while( ! element.isNull() ) {
			Editor * editor = m_tabEditors->newFileEditor( element.attribute( "filename" ) );
			editor->deserializeEditor( element );
			element = element.nextSiblingElement( "editor" );
		}
		m_tabEditors->setUpdatesEnabled( true );

		d->m_projectDock->setProjectPath( global.m_project );

		d->updateTitle();
	} catch( XSLProjectException e ) {
		delete global.m_project;
		global.m_project = NULL;
		QMessageBox::warning( this, tr("Can't open project"), e.getMessage() );
	}
	d->updateActions();
	d->updateRecentProjects();
	d->updateRecentFiles();
	
	global.emitProjectChanged();
}

void MainformImpl::closeProjectNoSessionData() {
	d->closeProject( false );
}

void MainformImpl::closeProjectWithSessionData() {
	d->closeProject( true );
}

void MainformImpl::callWebservices() {
	Q_ASSERT( m_tabEditors->currentEditor() != NULL );
	Q_ASSERT( global.m_project );
	
	WebServicesEditor * editor = qobject_cast<WebServicesEditor*>( m_tabEditors->currentEditor() );
	if( editor ) 
		editor->service()->call( editor->operation(), editor->values() );
}

void MainformImpl::updateWebServicesList() {
	bool enabled = global.m_project && ( global.m_project->projectType() == XSLProject::SERVICES );
	qDeleteAll( *(global.m_webServices) );
	global.m_webServices->clear();

	if( enabled ) {
		foreach( QString link, global.m_project->serveurWeb() ) {
			WebServices * ws = new WebServices( link, this );
			global.m_webServices->append( ws );
			ws->askWSDL( this );
			connect( ws, SIGNAL(soapResponse(QHash<QString,QString>,QHash<QString,QString>,QString,QString)), d, SLOT(webServicesReponse(QHash<QString,QString>,QHash<QString,QString>,QString,QString)) );
		}
	}
		
	m_webServicesMenu->setEnabled( enabled );
	m_refreshWebServicesListAct->setEnabled( enabled );
	m_callWebServicesAct->setEnabled( enabled );

	global.emitWebServicesChanged();
}

void MainformImpl::updateFromVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d->m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( d->m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		d->m_rcslogDialog->init();

		if( list.count() == 0 )
			rcs->update( QStringList() << global.m_project->projectPath() );
		else
			rcs->update( list );

		d->m_rcslogDialog->exec();
	}
}

void MainformImpl::commitToVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		CommitMessageDialogImpl dlg;

		if( list.count() == 0 ) 
			dlg.setFilesOperation( rcs->operations( QStringList() << global.m_project->projectPath() ) );
		else
			dlg.setFilesOperation( rcs->operations( list ) );
			
		if( ! dlg.exec() ) return ;
		QString message = dlg.messages();

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d->m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( d->m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		d->m_rcslogDialog->init();

		rcs->commit( dlg.filesOperation(), message );

		d->m_rcslogDialog->exec();
	}
}

void MainformImpl::addFilesToVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d->m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( d->m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		d->m_rcslogDialog->init();

		rcs->add( list );

		d->m_rcslogDialog->exec();
	}
}

void MainformImpl::removeFilesFromVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d->m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( d->m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		d->m_rcslogDialog->init();

		rcs->remove( list );

		d->m_rcslogDialog->exec();
	}
}
