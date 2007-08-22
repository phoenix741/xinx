/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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
#include "mainformimpl.h"
#include "private/p_mainformimpl.h"
#include "globals.h"
#include "xinxconfig.h"
#include "xslproject.h"
#include "snipet.h"
#include "snipetlist.h"
#include "snipetdialog.h"
#include "fileeditor.h"
#include "texteditor.h"

// Qt header
#include <QKeySequence>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileInfo>

/* PrivateMainformImpl */

PrivateMainformImpl::PrivateMainformImpl( MainformImpl * parent ) : m_parent( parent ) {
	createSubMenu();
	createDockWidget();
	createStatusBar();
	createShortcut();
	updateActions();
	updateRecentFiles();
	updateRecentProjects();
}

PrivateMainformImpl::~PrivateMainformImpl() {
	
}

void PrivateMainformImpl::createDockWidget() {
	m_contentDock = new FileContentDockWidget( "File Content", m_parent );
	m_contentDock->setObjectName( "m_contentDock" );
	m_parent->addDockWidget( Qt::LeftDockWidgetArea, m_contentDock );
	QAction * action = m_contentDock->toggleViewAction();
	action->setShortcut( tr("Ctrl+1") );
	m_parent->m_windowsMenu->addAction( action ); 
	
	m_projectDock = new ProjectDirectoryDockWidget( "Project Directory", m_parent );
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
	setupRecentMenu( recentProjectMenu, m_recentFileSeparator, m_recentFileActs );
	m_parent->m_recentFileAct->setMenu( recentFileMenu );
	btn = qobject_cast<QToolButton*>( m_parent->m_fileToolBar->widgetForAction( m_parent->m_recentFileAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
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
	m_parent->m_commentLineAct->setShortcut( QKeySequence( "Ctrl+Shift+D" ) );
	m_parent->m_uncommentLineAct->setShortcut( QKeySequence( "Ctrl+Shift+C" ) );
	m_parent->m_moveUpLineAct->setShortcut( QKeySequence( "Ctrl+Shift+Up" ) );
	m_parent->m_moveDownLineAct->setShortcut( QKeySequence( "Ctrl+Shift+Down" ) );	
	
	m_parent->m_completeAct->setShortcut( QKeySequence( "Ctrl+C" ) );	

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
	m_parent->m_closeProjectSessionAct->setEnabled( global.m_project != NULL );
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
		}
		
	}
}

void MainformImpl::open( const QString & filename ) {
	Q_ASSERT( ! filename.isEmpty() );

	if( global.m_project ) {
		global.m_project->lastOpenedFile().removeAll( filename );
		global.m_project->lastOpenedFile().prepend( filename );
     
		while( global.m_project->lastOpenedFile().size() > MAXRECENTFILES )
			global.m_project->lastOpenedFile().removeLast();
	}

	m_tabEditors->loadFileEditor( filename );
	d->updateRecentFiles();
	d->updateActions();
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainformImpl::saveAs( const QString & name ) {
	
}

void MainformImpl::saveAll() {
	
}

void MainformImpl::close() {
	
}

void MainformImpl::closeAll() {
	
}

void MainformImpl::newProject() {
	
}

void MainformImpl::openProject( const QString & name ) {
	
}

void MainformImpl::closeProject() {
	
}

void MainformImpl::closeProjectWithSessionData() {
	
}

void MainformImpl::callWebservices() {
	
}

void MainformImpl::refreshWebservices() {
	
}
