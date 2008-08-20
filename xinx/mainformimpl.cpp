/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "xslproject.h"
#include "snipet.h"
#include "snipetlist.h"
#include "snipetdialog.h"
#include "textfileeditor.h"
#include "texteditor.h"
#include "aboutdialogimpl.h"
#include "customdialogimpl.h"
#include "projectpropertyimpl.h"
#include "rcs.h"
#include "commitmessagedialogimpl.h"
#include "uniqueapplication.h"
#include "runsnipetdialogimpl.h"
#include "exceptions.h"
#include "xinxthread.h"
#include "specifiquedlgimpl.h"
#include "xinxpluginsloader.h"
#include "newprojectwizard.h"
#include "scriptmanager.h"
#include "searchfilethread.h"

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
#include <QTemporaryFile>
#include <QProcess>
#include <QTextStream>
#include <QDataStream>
#include <QClipboard>
#include <QDateTime>
#include <QWhatsThis>
#include <QTextCodec>

/* PrivateMainformImpl */

PrivateMainformImpl::PrivateMainformImpl( MainformImpl * parent ) : m_lastProjectOpenedPlace( QDir::currentPath() ), m_rcsExecute( false ), m_headContent( QString() ), m_closeTabBtn(0), m_parent( parent ) {
	registerTypes();
	createTabEditorButton();
	createSubMenu();
	createStatusBar();
	createDockWidget();
	createShortcut();
	createActions();
	createFindReplace();
	createTools();
	connectDbus();
	updateActions();
	updateRecentFiles();
	updateRecentProjects();
}

PrivateMainformImpl::~PrivateMainformImpl() {

}

void PrivateMainformImpl::registerTypes() {
	qRegisterMetaType<TextFileEditor>( "TextFileEditor" );
}

void PrivateMainformImpl::createDockWidget() {
	m_projectDock = new ProjectDirectoryDockWidget( tr("Project Directory"), m_parent );
	m_projectDock->setObjectName( QString::fromUtf8("m_projectDock") );
	m_projectDock->setGlobalUpdateAction( m_parent->m_globalUpdateFromRCSAct );
	m_projectDock->setGlobalCommitAction( m_parent->m_globalCommitToRCSAct );
	m_projectDock->setSelectedUpdateAction( m_parent->m_selectedUpdateFromRCSAct );
	m_projectDock->setSelectedCommitAction( m_parent->m_selectedCommitToRCSAct );
	m_projectDock->setSelectedAddAction( m_parent->m_selectedAddToRCSAct );
	m_projectDock->setSelectedRemoveAction( m_parent->m_selectedRemoveFromRCSAct );
	m_projectDock->setSelectedCompareWithHeadAction( m_parent->m_compareWithHeadAct );
	m_projectDock->setSelectedCompareWithStdAction( m_parent->m_compareWithStdAct );
	m_projectDock->setSelectedCompareAction( m_parent->m_compareTwoFileAct );
	m_projectDock->setToggledViewAction( m_parent->m_toggledFlatView );
	m_parent->addDockWidget( Qt::LeftDockWidgetArea, m_projectDock );
	QAction * action = m_projectDock->toggleViewAction();
	action->setShortcut( tr("Alt+1") );
	m_parent->m_windowsMenu->addAction( action );

	m_contentDock = new FileContentDockWidget( tr("File Content"), m_parent );
	m_contentDock->setObjectName( QString::fromUtf8("m_contentDock") );
	m_parent->addDockWidget( Qt::LeftDockWidgetArea, m_contentDock );
	action = m_contentDock->toggleViewAction();
	action->setShortcut( tr("Alt+2") );
	m_parent->m_windowsMenu->addAction( action );

	m_xmlpresentationdock = new XmlPresentationDockWidget( tr("XML Presentation"), m_parent );
	m_xmlpresentationdock->setObjectName( QString::fromUtf8( "m_xmlpresentationdock" ) );
	m_parent->addDockWidget( Qt::RightDockWidgetArea, m_xmlpresentationdock );
	action = m_xmlpresentationdock->toggleViewAction();
	action->setShortcut( tr("Alt+3") );
	m_parent->m_windowsMenu->addAction( action );

	m_snipetsDock = new SnipetDockWidget( tr("Snipets"), m_parent );
	m_snipetsDock->setObjectName( QString::fromUtf8( "m_snipetsDock" ) );
	m_parent->addDockWidget( Qt::RightDockWidgetArea, m_snipetsDock );
	action = m_snipetsDock->toggleViewAction();
	action->setShortcut( tr("Alt+4") );
	m_parent->m_windowsMenu->addAction( action );

	m_logDock = new LogDockWidget( tr("Log"), m_parent );
	connect( m_logDock, SIGNAL(open(QString,int)), this, SLOT(openFile(QString,int)) );
	m_logDock->setObjectName( QString::fromUtf8("m_logDock") );
	m_parent->addDockWidget( Qt::BottomDockWidgetArea, m_logDock );
	action = m_logDock->toggleViewAction();
	action->setShortcut( tr("Alt+9") );
	m_parent->m_windowsMenu->addAction( action );
}

void PrivateMainformImpl::createSubMenu() {
	// New sub menu
	QMenu * newMenu = new QMenu( m_parent );
	foreach( IFileTypePlugin * plugin, XinxPluginsLoader::self()->fileTypes() ) {
		QAction * action = new QAction(
				plugin->icon(),
				tr( "New %1" ).arg( plugin->description() ),
				this
				);
		action->setData( QVariant::fromValue<IFileTypePlugin*>( plugin ) );
		newMenu->addAction( action );

		connect( action, SIGNAL(triggered()), this, SLOT(newFile()) );
	}

	m_parent->m_newAct->setData( QVariant::fromValue<IFileTypePlugin*>( XinxPluginsLoader::self()->fileTypes()[0] ) );
	connect( m_parent->m_newAct, SIGNAL(triggered()), this, SLOT(newFile()) );

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

void PrivateMainformImpl::createStatusBar() {
	m_codecLabel = new QLabel( tr("Unknown") );
	m_lineFeedLabel = new QLabel( tr("Unknown") );
	m_editorPosition = new QLabel( "000x000" );
	m_threadCount = new QLabel( "000 (000)" );
	m_parent->statusBar()->addPermanentWidget( m_codecLabel );
	m_parent->statusBar()->addPermanentWidget( m_lineFeedLabel );
	m_parent->statusBar()->addPermanentWidget( m_editorPosition );
	m_parent->statusBar()->addPermanentWidget( m_threadCount );
	connect( XinxThreadManager::self(), SIGNAL(threadCountChange()), this, SLOT(setThreadCountChange()) );
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
	m_parent->m_highlightWord->setShortcut( QKeySequence( "Ctrl+U" ) );

	// Bookmark menu
	m_parent->m_bookmarkAct->setShortcut( QKeySequence( "Ctrl+B" ) );
	m_parent->m_nextBookmarkAct->setShortcut( QKeySequence( "F2" ) );
	m_parent->m_previousBookmarkAct->setShortcut( QKeySequence( "Shift+F2" ) );
	m_parent->m_clearAllBookmarkAct->setShortcut( QKeySequence( "Ctrl+F2" ) );

	// Search menu
	m_parent->m_searchAct->setShortcut( QKeySequence::Find );
	m_parent->m_searchNextAct->setShortcut( QKeySequence::FindNext );
	m_parent->m_searchPreviousAct->setShortcut( QKeySequence::FindPrevious );
	m_parent->m_replaceAct->setShortcut( QKeySequence::Replace );

	// Project menu
	m_parent->m_globalUpdateFromRCSAct->setShortcut( QKeySequence::Refresh );
	m_parent->m_globalCommitToRCSAct->setShortcut( QKeySequence( "F6" ) );
	m_parent->m_cancelRCSOperationAct->setShortcut( QKeySequence( "Escape" ) );

	// Windows menu
	m_parent->m_nextTabAct->setShortcut( QKeySequence::NextChild );
	m_parent->m_previousTabAct->setShortcut( QKeySequence::PreviousChild );
}

void PrivateMainformImpl::createActions() {
	/* FILE */

	// Open
	connect( m_parent->m_openAct, SIGNAL(triggered()), this, SLOT(openFile()) );
	connect( m_parent->m_recentFileAct, SIGNAL(triggered()), this, SLOT(openFile()) );
	connect( m_contentDock, SIGNAL(open(QString,int)), this, SLOT(openFile(QString,int)) );
	connect( m_projectDock, SIGNAL(open(QString)), m_parent, SLOT(openFile(QString)) );
	connect( m_parent->m_tabEditors, SIGNAL(modelChanged(QAbstractItemModel*)), m_contentDock, SLOT(updateModel(QAbstractItemModel*)) );
	connect( m_parent->m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)) );

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

	// Tab Editor
	connect( m_parent->m_tabEditors, SIGNAL(setEditorPosition(int,int)), this, SLOT(setEditorPosition(int,int)) );
	connect( m_parent->m_tabEditors, SIGNAL(contentChanged()), this, SLOT(updateEditorInformations()) );

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

	connect( m_parent->m_copyFileNameAct, SIGNAL(triggered()), this, SLOT(copyFileName()) );
	m_parent->m_tabEditors->setCopyFileNameAction( m_parent->m_copyFileNameAct );
	connect( m_parent->m_copyPathAct, SIGNAL(triggered()), this, SLOT(copyPath()) );
	m_parent->m_tabEditors->setCopyPathAction( m_parent->m_copyPathAct );

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

	// Show space and Tabulation
	m_parent->m_showSpaceAndTabAct->setChecked( XINXConfig::self()->config().editor.showTabulationAndSpace );
	connect( m_parent->m_showSpaceAndTabAct, SIGNAL(triggered()), this, SLOT(updateSpaceAndTab()) );

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

	// Highlight Same Word
	connect( m_parent->m_highlightWord, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(highlightWord()) );

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

	/* Bookmark */
	connect( m_parent->m_bookmarkAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(bookmark()) );
	connect( m_parent->m_nextBookmarkAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(nextBookmark()) );
	connect( m_parent->m_previousBookmarkAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(previousBookmark()) );
	connect( m_parent->m_clearAllBookmarkAct, SIGNAL(triggered()), m_parent->m_tabEditors, SLOT(clearAllBookmark()) );

	/* PROJECT */
	connect( m_parent->m_globalUpdateFromRCSAct, SIGNAL(triggered()), this, SLOT(globalUpdateFromVersionManager()) );
	connect( m_parent->m_globalCommitToRCSAct, SIGNAL(triggered()), this, SLOT(globalCommitToVersionManager()) );
	connect( m_parent->m_selectedUpdateFromRCSAct, SIGNAL(triggered()), this, SLOT(selectedUpdateFromVersionManager()) );
	connect( m_parent->m_selectedCommitToRCSAct, SIGNAL(triggered()), this, SLOT(selectedCommitToVersionManager()) );
	connect( m_parent->m_selectedAddToRCSAct, SIGNAL(triggered()), this, SLOT(selectedAddToVersionManager()) );
	connect( m_parent->m_selectedRemoveFromRCSAct, SIGNAL(triggered()), this, SLOT(selectedRemoveFromVersionManager()) );
	connect( m_parent->m_compareWithHeadAct, SIGNAL(triggered()), this, SLOT(selectedCompareWithVersionManager()) );
	connect( m_parent->m_compareWithStdAct, SIGNAL(triggered()), this, SLOT(selectedCompareWithStd()) );
	connect( m_parent->m_compareTwoFileAct, SIGNAL(triggered()), this, SLOT(selectedCompare()) );

	connect( m_parent->m_newProjectAct, SIGNAL(triggered()), m_parent, SLOT(newProject()) );
	connect( m_parent->m_openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_parent->m_recentProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_parent->m_projectPropertyAct, SIGNAL(triggered()), this, SLOT(projectProperty()) );
	connect( m_parent->m_closeProjectAct, SIGNAL(triggered()), this, SLOT(closeProject()) );
	connect( m_parent->m_closeProjectNoSessionAct, SIGNAL(triggered()), m_parent, SLOT(closeProjectNoSessionData()) );
	connect( m_parent->m_closeProjectWithSessionAct, SIGNAL(triggered()), m_parent, SLOT(closeProjectWithSessionData()) );

	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateActions()) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateTitle()) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateRecentProjects()) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateRecentFiles()) );

	/* WINDOWS */
	connect( m_parent->m_toggledFlatView, SIGNAL(toggled(bool)), m_projectDock, SLOT(toggledView(bool)) );
	connect( m_parent->m_nextTabAct, SIGNAL(triggered()), this, SLOT(nextTab()) );
	connect( m_parent->m_previousTabAct, SIGNAL(triggered()), this, SLOT(previousTab()) );

	/* Tools */
	// Create Template
	connect( m_parent->m_createTemplate, SIGNAL(triggered()), m_parent, SLOT(newTemplate()) );
	connect( m_parent->m_refreshSnipet, SIGNAL(triggered()), SnipetListManager::self(), SLOT(loadFromSnipetFile()) );
	connect( m_parent->m_refreshScripts, SIGNAL(triggered()), ScriptManager::self(), SLOT(loadScripts()) );

	// Customize
	connect( m_parent->m_customApplicationAct, SIGNAL(triggered()), this, SLOT(customize()) );

  	// About Qt
	connect( m_parent->m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// About
	connect( m_parent->m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	// What's This ?
	m_parent->m_helpMenu->addAction( QWhatsThis::createAction() );

	// Drag & Drop
	connect( m_parent->m_tabEditors, SIGNAL(fileOpened(QString)), this, SLOT(updateActions()) );

	// Timer LOG
	m_timer = new QTimer( this );
	connect( m_timer, SIGNAL(timeout()), this, SLOT(logTimeout()) );
}

void PrivateMainformImpl::connectDbus() {
	qobject_cast<UniqueApplication*>(qApp)->attachMainWindow( m_parent );
}

void PrivateMainformImpl::createTools() {
	connect( SnipetListManager::self(), SIGNAL(listChanged()), this, SLOT(updateToolsMenu()) );
	connect( ScriptManager::self(), SIGNAL(changed()), this, SLOT(updateToolsMenu()) );
	updateToolsMenu();
}

void PrivateMainformImpl::callSnipetMenu() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() != NULL );

	QAction * action = qobject_cast<QAction*>( sender() );
	if( action && TabEditor::isTextFileEditor( m_parent->m_tabEditors->currentEditor() ) ) {
		const Snipet & snipet = action->data().value<Snipet>();

		RunSnipetDialogImpl dlg( snipet );
		if( dlg.exec() ) {
			TextEditor * editor = static_cast<TextFileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
			editor->insertText( dlg.getResult() );
		}
	}
}

void PrivateMainformImpl::callScriptAction() {
	QAction * action = qobject_cast<QAction*>( sender() );
	QScriptValue qsScript = action->data().value<QScriptValue>();

	QScriptValue result = qsScript.property( "run" ).call( qsScript );
	if( result.isError() ) {
		qWarning( qPrintable( tr("An error occure while run the script : %1").arg( result.toString() ) ) );
	}
}

void PrivateMainformImpl::updateToolsMenu() {
	/* Snipet */
	qDeleteAllLater( m_snipetActs );
	m_snipetActs.clear();

	qDeleteAllLater( m_snipetCategoryActs.values() );
	m_snipetCategoryActs.clear();

	if( SnipetListManager::self()->snipets().count() > 0 ) {
		foreach( QString category, SnipetListManager::self()->snipets().categories() ) {
			QAction * act = new QAction( category, m_parent );
			m_snipetCategoryActs[ category ] = act;
			act->setMenu( new QMenu( m_parent ) );
		}
		for( int i = 0 ; i < SnipetListManager::self()->snipets().count() ; i++ ) {
			const Snipet & snipet = SnipetListManager::self()->snipets().at( i );
			QAction * act = new QAction( QIcon( snipet.icon() ), snipet.name(), m_parent );
			m_snipetActs.append( act );
			m_snipetCategoryActs[ snipet.category() ]->menu()->addAction( act );
			act->setData( QVariant::fromValue( snipet ) );
			connect( act, SIGNAL(triggered()), this, SLOT(callSnipetMenu()) );
		}
		m_parent->m_templateMenu->insertActions( m_parent->m_createTemplate, m_snipetCategoryActs.values() );
	}

	/* Scripts */
	qDeleteAllLater( m_scriptActs );
	m_scriptActs.clear();

	foreach( const QScriptValue & s, ScriptManager::self()->objects() ) {
		QAction * action = new QAction( this );
		action->setText( s.property("text").toString() );
		action->setData( QVariant::fromValue( s ) );
		m_parent->m_scriptMenu->addAction( action );

		connect( action, SIGNAL(triggered()), this, SLOT(callScriptAction()) );
		m_scriptActs.append( action );
	}

	updateActions();
}

void PrivateMainformImpl::currentTabChanged( int index ) {
	AbstractEditor * editor = m_parent->m_tabEditors->editor( index );
	ScriptManager::self()->setCurrentEditeur( editor );
	m_snipetsDock->setEditor( editor );
	updateEditorInformations();
}

void PrivateMainformImpl::updateEditorInformations() {
	AbstractEditor * editor = m_parent->m_tabEditors->currentEditor();
	if( qobject_cast<TextFileEditor*>( editor ) ) { // This is a text file editor. So we have codec, ...
		TextFileEditor * textFileEditor = qobject_cast<TextFileEditor*>( editor );
		m_codecLabel->setText( textFileEditor->codec()->name() );
		switch( textFileEditor->eol() ) {
		case TextFileEditor::MacEndOfLine:
			m_lineFeedLabel->setText( "MAC" );
			break;
		case TextFileEditor::WindowsEndOfLine:
			m_lineFeedLabel->setText( "WIN" );
			break;
		case TextFileEditor::UnixEndOfLine:
			m_lineFeedLabel->setText( "UNIX" );
			break;
		}
	} else {
		m_codecLabel->setText( tr("No codec") );
		m_lineFeedLabel->setText( tr("Unknown") );
	}
}

void PrivateMainformImpl::newFile() {
	if( qobject_cast<QAction*>( sender() ) ) {
		IFileTypePlugin* plugin = qobject_cast<QAction*>( sender() )->data().value<IFileTypePlugin*>();
		m_parent->m_tabEditors->createEditor( plugin );
		updateActions();
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
	TextFileEditor * ed = qobject_cast<TextFileEditor*>( m_parent->m_tabEditors->currentEditor() );
	ed->textEdit()->gotoLine( line );
	ed->textEdit()->setFocus( Qt::OtherFocusReason );
}

void PrivateMainformImpl::openFile() {
	Q_ASSERT( XINXConfig::self() );

	QStringList selectedFiles = QFileDialog::getOpenFileNames( m_parent, tr("Open text file"), SpecifiqueDialogImpl::lastPlace(), XinxPluginsLoader::self()->openDialogBoxFilters().join(";;") );

	m_parent->m_tabEditors->setUpdatesEnabled( false );
	foreach( QString filename, selectedFiles ) {
		SpecifiqueDialogImpl::setLastPlace( QFileInfo( filename ).absolutePath() );
		m_parent->openFile( filename );
	}
	m_parent->m_tabEditors->setUpdatesEnabled( true );
}

void PrivateMainformImpl::currentCloseFile() {
	fileEditorClose( m_parent->m_tabEditors->currentIndex() );
}

void PrivateMainformImpl::fileEditorRefreshFile( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_parent->m_tabEditors->count() );

	AbstractFileEditor * ed = qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) );
	if( ed ) {
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
			ed->loadFromFile();
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
	custom.loadFromConfig( XINXConfig::self() );

	if( custom.exec() ) {
		custom.saveToConfig( XINXConfig::self() );
		XINXConfig::self()->save();
		SnipetListManager::self()->saveToSnipetFile();
	}
}

void PrivateMainformImpl::setupRecentMenu( QMenu * menu, QAction * & separator, QAction * recentActions[ MAXRECENTFILES ] ) {
	separator = menu->addSeparator();
	separator->setVisible( false );
	for(int i = 0; i < MAXRECENTFILES; i++) {
		recentActions[i] = new QAction( m_parent );
		recentActions[i]->setVisible( false );
		menu->addAction( recentActions[i] );
	}
}

void PrivateMainformImpl::updateRecentFiles() {
	int numRecentFiles;
	if( XINXProjectManager::self()->project() ) {
		numRecentFiles = qMin( XINXProjectManager::self()->project()->session()->lastOpenedFile().size(), MAXRECENTFILES );

		for( int i = 0; i < numRecentFiles; i++ ) {
			QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( XINXProjectManager::self()->project()->session()->lastOpenedFile()[i] ).fileName() );
			if( XinxPluginsLoader::self()->matchedFileType( QFileInfo( text ).fileName() ) )
				m_recentFileActs[i]->setIcon( XinxPluginsLoader::self()->matchedFileType( QFileInfo( text ).fileName() )->icon() );
			m_recentFileActs[i]->setText( text );
			m_recentFileActs[i]->setData( XINXProjectManager::self()->project()->session()->lastOpenedFile()[i] );
			m_recentFileActs[i]->setVisible( true );
		}
	} else
		numRecentFiles = 0;

	for( int j = numRecentFiles; j < MAXRECENTFILES; j++ )
		m_recentFileActs[j]->setVisible(false);

	m_recentFileSeparator->setVisible( numRecentFiles > 0 );
}

void PrivateMainformImpl::updateRecentProjects() {
	int numRecentFiles = qMin( XINXConfig::self()->config().project.recentProjectFiles.size(), MAXRECENTFILES );

	for( int i = 0; i < numRecentFiles; i++ ) {
		QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( XINXConfig::self()->config().project.recentProjectFiles[i] ).fileName() );
		m_recentProjectActs[i]->setText( text );
		m_recentProjectActs[i]->setData( XINXConfig::self()->config().project.recentProjectFiles[i] );
		m_recentProjectActs[i]->setVisible( true );
	}

	for( int j = numRecentFiles; j < MAXRECENTFILES; j++ )
		m_recentProjectActs[j]->setVisible(false);

	m_recentSeparator->setVisible( numRecentFiles > 0 );
}

void PrivateMainformImpl::readWindowSettings() {
	m_parent->resize( XINXConfig::self()->config().size );
	m_parent->move( XINXConfig::self()->config().position );
	if( XINXConfig::self()->config().maximized )
		m_parent->setWindowState( m_parent->windowState() ^ Qt::WindowMaximized );

	if( ! XINXConfig::self()->config().state.isEmpty() ) {
		if( ! m_parent->restoreState( XINXConfig::self()->config().state ) )
			qWarning( "Can't restore windows state.\n" );
	}
}

void PrivateMainformImpl::storeWindowSettings() {
	XINXConfig::self()->config().state = m_parent->saveState();

	XINXConfig::self()->config().position = m_parent->pos();
	XINXConfig::self()->config().size = m_parent->size();
	XINXConfig::self()->config().maximized = m_parent->isMaximized();

	XINXConfig::self()->save();
}

void PrivateMainformImpl::createTabEditorButton() {
	m_closeTabBtn = new QToolButton( m_parent->m_tabEditors );
	m_closeTabBtn->setIcon( QIcon( ":/images/tabclose.png" ) );
	connect( m_closeTabBtn, SIGNAL(clicked()), this, SLOT(currentCloseFile()) );
	if( ! XINXConfig::self()->config().editor.hideCloseTab )
		m_parent->m_tabEditors->setCornerWidget( m_closeTabBtn );

	connect( XINXConfig::self(), SIGNAL(changed()), this, SLOT(updateConfigElement()) );
}

void PrivateMainformImpl::updateConfigElement() {
	if( m_parent->m_showSpaceAndTabAct->isChecked() != XINXConfig::self()->config().editor.showTabulationAndSpace )
		m_parent->m_showSpaceAndTabAct->setChecked( XINXConfig::self()->config().editor.showTabulationAndSpace );
	m_closeTabBtn->setVisible( ! XINXConfig::self()->config().editor.hideCloseTab );
}

void PrivateMainformImpl::updateActions() {
	/* Project action */
	m_parent->m_saveProjectAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_parent->m_closeProjectAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_parent->m_closeProjectWithSessionAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_parent->m_closeProjectNoSessionAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_parent->m_projectPropertyAct->setEnabled( XINXProjectManager::self()->project() != NULL );

	m_parent->m_globalUpdateFromRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute ) );
	m_parent->m_globalCommitToRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );

	m_parent->m_selectedUpdateFromRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );
	m_parent->m_selectedCommitToRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );
	m_parent->m_selectedAddToRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );
	m_parent->m_selectedRemoveFromRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );

	m_parent->m_cancelRCSOperationAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && m_rcsExecute );

	m_parent->m_toggledFlatView->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_projectDock->setEnabled( XINXProjectManager::self()->project() != NULL );

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
	m_parent->m_bookmarkAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_nextBookmarkAct->setEnabled( m_parent->m_tabEditors->count() );
	m_parent->m_previousBookmarkAct->setEnabled( m_parent->m_tabEditors->count() );

	if( m_parent->m_tabEditors->count() == 0 ) {
		m_contentDock->updateModel( NULL );
	}
}

bool PrivateMainformImpl::fileEditorMayBeSave( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_parent->m_tabEditors->count() );

	AbstractFileEditor * ed = qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) );
	if( ed && ed->isModified() ) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning( m_parent, tr("Application"),
									tr("The document %1 has been modified.\n"
									"Do you want to save your changes?").arg( ed->getTitle() ),
									QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if ( ret == QMessageBox::Save ) {
			fileEditorSave( index );
		} else if (ret == QMessageBox::Cancel)
			return false;
	}

	return true;
}

void PrivateMainformImpl::fileEditorSave( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_parent->m_tabEditors->count() );
	Q_ASSERT( qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) ) );

	QString editorFileName = qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) )->lastFileName();
	bool hasName = ! editorFileName.isEmpty();

	if ( ! hasName ) {
		fileEditorSaveAs( index );
	} else {
		QString fileName = SpecifiqueDialogImpl::saveFileAsIfStandard( editorFileName, m_fileToAdd );
		if( ! fileName.isEmpty() ) {
			qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) )->saveToFile( fileName );
			if( m_fileToAdd.count() > 0 )
				m_parent->addFilesToVersionManager( m_fileToAdd );
			m_projectDock->refreshPath( QFileInfo( fileName ).absoluteFilePath() );
			m_parent->statusBar()->showMessage( tr("File %1 saved").arg( m_parent->m_tabEditors->editor(index)->getTitle() ), 2000 );
		}
		m_fileToAdd.clear();
	}
}

void PrivateMainformImpl::fileEditorSaveAs( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_parent->m_tabEditors->count() );
	Q_ASSERT( qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) ) );

	QString fileName = qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) )->lastFileName();

	fileName = SpecifiqueDialogImpl::saveFileAs( fileName,
				qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) )->defaultFileName(),
				m_fileToAdd );

	if( ! fileName.isEmpty() ) {
		qobject_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( index ) )->saveToFile( fileName );
		if( m_fileToAdd.count() > 0 )
			m_parent->addFilesToVersionManager( m_fileToAdd );
		m_projectDock->refreshPath( QFileInfo( fileName ).absoluteFilePath() );
		m_parent->statusBar()->showMessage( tr("File %1 saved").arg( m_parent->m_tabEditors->editor(index)->getTitle() ), 2000 );
	}
	m_fileToAdd.clear();
}

void PrivateMainformImpl::fileEditorClose( int index ) {
	if( fileEditorMayBeSave( index ) ) {
		m_contentDock->updateModel( NULL );
		AbstractEditor * editor = m_parent->m_tabEditors->editor( index );
		m_parent->m_tabEditors->removeTab( index );
		delete editor;
	}

	updateActions();
}

void PrivateMainformImpl::printFile() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() != NULL );

	if( TabEditor::isTextFileEditor( m_parent->m_tabEditors->currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( m_parent->m_tabEditors->currentEditor() );
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

void PrivateMainformImpl::selectedCompareWithVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	Q_ASSERT( list.size() == 1 );

	QString revision;
	m_compareFileName = list.at( 0 );
	m_headContent = QString();

	RCS * rcs = m_projectDock->rcs();
	if( rcs ) {
		revision = rcs->infos( m_compareFileName ).version;

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_parent->m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );

		m_logDock->init();
		m_rcsExecute = true;

		updateActions();
		m_rcsVisible = m_logDock->isVisible();
		m_logDock->show();
		rcs->updateToRevision( m_compareFileName, revision, &m_headContent );
	}
}

void PrivateMainformImpl::logTimeout() {
	m_timer->stop();
	m_logDock->setVisible( false );
}

void PrivateMainformImpl::rcsLogTerminated() {
	Q_ASSERT( m_projectDock->rcs() );

	if( ! m_headContent.isEmpty() ) {
		try {
			QTemporaryFile * headContentFile = new QTemporaryFile( m_parent ); // Delete when the main windows is destroyed
			if( headContentFile->open() ) {
				headContentFile->setTextModeEnabled( true );
				QTextStream out(headContentFile);
				out << m_headContent;
			}
			QProcess::startDetached( XINXConfig::self()->getTools( "diff" ), QStringList() << m_compareFileName << headContentFile->fileName() );
			m_headContent = QString();
		} catch( ToolsNotDefinedException e ) {
			QMessageBox::warning( m_parent, tr( "Tools" ), e.getMessage() );
		}
	}

	m_rcsExecute = false;
	RCS * rcs = m_projectDock->rcs();
	if( rcs )
		rcs->disconnect();
	updateActions();

	m_logDock->end();
	if( (!m_rcsVisible) && m_logDock->isVisible() && XINXConfig::self()->config().project.closeVersionManagementLog )
		m_timer->start( 5000 );
}

void PrivateMainformImpl::selectedCompareWithStd() {
	try {
		QStringList list = m_projectDock->selectedFiles();
		Q_ASSERT( list.size() == 1 && XINXProjectManager::self()->project() );

		QString customFilename = list.at( 0 ), stdFilename, path, filename;

		path = QFileInfo( customFilename ).absolutePath();
		filename = QFileInfo( customFilename ).fileName();

		foreach( QString prefix, XINXProjectManager::self()->project()->specifiquePrefixes() ) {
			if( filename.startsWith( prefix + "_", Qt::CaseInsensitive ) ) {
				filename.remove( 0, prefix.size() + 1 );
				stdFilename = QDir( path ).absoluteFilePath( filename );

				if( QFileInfo( stdFilename ).exists() )
					QProcess::startDetached( XINXConfig::self()->getTools( "diff" ), QStringList() << customFilename << stdFilename );
				else
					QMessageBox::information( m_parent, tr("Compare"), tr("Standard file %1 not found or not in specifique directory.").arg( filename ), QMessageBox::Ok );

				return;
			}
		}

		QMessageBox::information( m_parent, tr("Compare"), tr("Not a specifique file"), QMessageBox::Ok );
	} catch( ToolsNotDefinedException e ) {
		QMessageBox::warning( m_parent, tr( "Tools" ), e.getMessage() );
	}
}

void PrivateMainformImpl::selectedCompare() {
	try {
		QStringList list = m_projectDock->selectedFiles();
		Q_ASSERT( list.size() == 2 );
		QProcess::startDetached( XINXConfig::self()->getTools( "diff" ), QStringList() << list.at( 0 ) << list.at( 1 ) );
	} catch( ToolsNotDefinedException e ) {
		QMessageBox::warning( m_parent, tr( "Tools" ), e.getMessage() );
	}
}


void PrivateMainformImpl::nextTab() {
	m_parent->m_tabEditors->setCurrentIndex( ( m_parent->m_tabEditors->currentIndex() + 1 ) % m_parent->m_tabEditors->count() );
}

void PrivateMainformImpl::previousTab() {
	m_parent->m_tabEditors->setCurrentIndex( ( m_parent->m_tabEditors->currentIndex() - 1 + m_parent->m_tabEditors->count() ) % m_parent->m_tabEditors->count() );
}

void PrivateMainformImpl::copyFileName() {
	int item = m_parent->m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_parent->m_tabEditors->currentIndex();
	QString name = m_parent->m_tabEditors->editor( item )->getTitle();
	qApp->clipboard()->setText( name );
}

void PrivateMainformImpl::copyPath() {
	int item = m_parent->m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_parent->m_tabEditors->currentIndex();
	AbstractFileEditor * ed = dynamic_cast<AbstractFileEditor*>( m_parent->m_tabEditors->editor( item ) );
	if( ed ) {
		QString name = ed->lastFileName();
		qApp->clipboard()->setText( name );
	}
}

void PrivateMainformImpl::updateSpaceAndTab() {
	if( XINXConfig::self()->config().editor.showTabulationAndSpace != m_parent->m_showSpaceAndTabAct->isChecked() ) {
		XINXConfig::self()->config().editor.showTabulationAndSpace = m_parent->m_showSpaceAndTabAct->isChecked();
		XINXConfig::self()->save();
	}
}

void PrivateMainformImpl::createFindReplace() {
	m_findDialog       = new ReplaceDialogImpl( m_parent );
	connect( m_findDialog, SIGNAL(find(QString, QString, AbstractEditor::SearchOptions)), this, SLOT(findFirst(QString, QString, AbstractEditor::SearchOptions)) );
	connect( m_findDialog, SIGNAL(findInFiles(QString, QString, QString, AbstractEditor::SearchOptions)), this, SLOT(findInFiles(QString, QString, QString, AbstractEditor::SearchOptions)) );

	m_replaceNextDlg   = new QMessageBox( QMessageBox::Question, tr("Replace text"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, m_parent, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
	m_replaceNextDlg->setWindowModality( Qt::NonModal );
}

void PrivateMainformImpl::findFirst( const QString & chaine, const QString & dest, const AbstractEditor::SearchOptions & options ) {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() );

	AbstractEditor * editor = m_parent->m_tabEditors->currentEditor();

	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;
	m_nbFindedText		= 0;
	m_searchInverse		= false;

	editor->initSearch( m_findOptions );
	findNext();
}

void PrivateMainformImpl::findInFiles( const QString & directory, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options ) {
	m_parent->m_searchAct->setEnabled( false );
	m_parent->m_searchNextAct->setEnabled( false );
	m_parent->m_searchPreviousAct->setEnabled( false );
	m_parent->m_replaceAct->setEnabled( false );
	m_logDock->init();
	m_logDock->setVisible( true );

	SearchFileThread * threadSearch = new SearchFileThread();
	connect( threadSearch, SIGNAL(find(QString,QString,int)), m_logDock, SLOT(find(QString,QString,int)), Qt::BlockingQueuedConnection );
	connect( threadSearch, SIGNAL(end()), this, SLOT(findEnd()) );

	threadSearch->setPath( directory );
	threadSearch->setSearchString( from, to, options );
	threadSearch->search();
}

void PrivateMainformImpl::findEnd() {
	m_parent->m_searchAct->setEnabled( true );
	m_parent->m_searchNextAct->setEnabled( true );
	m_parent->m_searchPreviousAct->setEnabled( true );
	m_parent->m_replaceAct->setEnabled( true );

	m_logDock->end();

	QMessageBox::information( m_parent, tr("Search End"), tr("All string are finded") );
}

void PrivateMainformImpl::findNext() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() );

	AbstractEditor * editor = m_parent->m_tabEditors->currentEditor();

	bool continuer = true;

	while( continuer ) {
		continuer = false;

		AbstractEditor::SearchOptions options = m_findOptions;
		if( m_searchInverse ) options |= AbstractEditor::BACKWARD;
		bool trouve = editor->find( m_findExpression, options );

		if( trouve ) {
			m_nbFindedText++;

			if( ! m_replaceExpression.isNull() ) {
				QMessageBox::StandardButton ret = QMessageBox::Yes;

				if(! m_yesToAllReplace) {
					if( m_nbFindedText == 1 )
						m_replaceNextDlg->setAttribute( Qt::WA_Moved, false );
					m_replaceNextDlg->show();
					m_replaceNextDlg->setAttribute( Qt::WA_Moved );
					while( m_replaceNextDlg->isVisible() ) qApp->processEvents();
					ret = m_replaceNextDlg->standardButton( m_replaceNextDlg->clickedButton() );
				}

				switch(ret) {
				case QMessageBox::Yes:
					// Replace chaine
					editor->replace( m_findExpression, m_replaceExpression, options );
					continuer = true;
					break;
				case QMessageBox::YesToAll:
					editor->replace( m_findExpression, m_replaceExpression, options );
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
		} else {
			if( m_yesToAllReplace && ! m_replaceExpression.isNull() ) {
				QMessageBox::information( m_parent,
				                          tr("Search/Replace"),
				                          tr("%1 occurences of '%2' replaced.").arg( m_nbFindedText ).arg( m_findExpression ),
				                          QMessageBox::Ok );
			} else {
				QMessageBox::StandardButton ret = QMessageBox::warning( m_parent,
					tr("Search/Replace"),
					tr("%1 occurences of '%2' %3. Return to the beginning of the document ?").arg( m_nbFindedText ).arg( m_findExpression ).arg( ! m_replaceExpression.isNull() ? tr("replaced") : tr("finded") ),
					QMessageBox::Yes | QMessageBox::No );

				if( ret == QMessageBox::Yes ) {
					m_findOptions   &= ~ AbstractEditor::SearchOptions( AbstractEditor::ONLY_SELECTION );
					options			&= ~ AbstractEditor::SearchOptions( AbstractEditor::ONLY_SELECTION );
					options 		|= AbstractEditor::SEARCH_FROM_START;
					editor->initSearch( options );

					continuer = true;
				}
			}
			m_nbFindedText = 0;
		}
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
	Q_ASSERT( qobject_cast<TextFileEditor*>( m_parent->m_tabEditors->currentEditor() ) );

	TextEditor * textEdit = qobject_cast<TextFileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(false);
	m_findDialog->exec();
}

void PrivateMainformImpl::replace() {
	Q_ASSERT( m_parent->m_tabEditors->currentEditor() );
	Q_ASSERT( qobject_cast<TextFileEditor*>( m_parent->m_tabEditors->currentEditor() ) );

	TextEditor * textEdit = static_cast<TextFileEditor*>( m_parent->m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(true);
	m_findDialog->exec();
}

bool PrivateMainformImpl::closeProject( bool session ) {
	if( ! XINXProjectManager::self()->project() ) return false;

	foreach( XinxPluginElement e, XinxPluginsLoader::self()->plugins() ) {
		if( e.isActivated() && (! qobject_cast<IXinxPlugin*>( e.plugin() )->destroyProject( XINXProjectManager::self()->project() ) ))
			qWarning( qPrintable(tr("Can't stop a project for plugin \"%1\"").arg( qobject_cast<IXinxPlugin*>( e.plugin() )->getPluginAttribute( IXinxPlugin::PLG_NAME ).toString() )) );
	}

	m_parent->saveProject( session );

	if( ! session ) {
		if( ! m_parent->closeAllFile() ) {
			return false;
		}
	} else {
		m_parent->m_tabEditors->setUpdatesEnabled( false );
		for( int i = m_parent->m_tabEditors->count() - 1; i >= 0; i-- ) {
			AbstractEditor * ed = m_parent->m_tabEditors->editor( i );
			m_parent->m_tabEditors->removeTab( i );
			delete ed;
		}
		m_parent->m_tabEditors->setUpdatesEnabled( true );
	}
	m_projectDock->setProjectPath( NULL );
	m_contentDock->updateModel( NULL );

	XINXProjectManager::self()->deleteProject();

	return true;
}

void PrivateMainformImpl::updateTitle() {
	if( ! XINXProjectManager::self()->project() )
		m_parent->setWindowTitle( tr("XINX") );
	else {
		m_parent->setWindowTitle( tr("%1 - %2").arg( XINXProjectManager::self()->project()->projectName() ).arg( tr("XINX") ) );
	}
}

void PrivateMainformImpl::openProject() {
	QString fileName = QFileDialog::getOpenFileName( m_parent, tr("Open a project"), XINXConfig::self()->config().project.defaultPath, "Projet (*.prj)" );
	if( ! fileName.isEmpty() )
		m_parent->openProject( fileName );
}

void PrivateMainformImpl::projectProperty() {
	Q_ASSERT( XINXProjectManager::self()->project() != NULL );

	ProjectPropertyImpl property ( m_parent );
	property.loadFromProject( XINXProjectManager::self()->project() );

	if( property.exec() ) {
		property.saveToProject( XINXProjectManager::self()->project() );
		XINXProjectManager::self()->project()->saveToFile();
	}
}

void PrivateMainformImpl::closeProject() {
	closeProject( XINXConfig::self()->config().project.saveWithSessionByDefault );
}

AppSettings::struct_extentions PrivateMainformImpl::extentionOfFileName( const QString & name ) {
	AppSettings::struct_extentions result;
	int dotPosition = name.lastIndexOf( "." );
	QString suffix = name.toLower();
	if( dotPosition >= 0 )
		suffix = suffix.mid( dotPosition + 1 );
	if( XINXConfig::self()->config().files.count( suffix ) > 0 )
		result = XINXConfig::self()->config().files[ suffix ];
	return result;
}

void PrivateMainformImpl::setEditorPosition( int line, int column ) {
	m_editorPosition->setText( QString("   %1 x %2   ").arg( line, 3, 10, QLatin1Char('0') ).arg( column, 3, 10, QLatin1Char('0') ) );
}

void PrivateMainformImpl::setThreadCountChange() {
	m_threadCount->setText( QString( "%1 (%2)" )
			.arg( XinxThreadManager::self()->getThreadCount(), 3, 10, QLatin1Char('0') )
			.arg( XinxThreadManager::self()->getThreadClassCount(), 3, 10, QLatin1Char('0') ) );
}


/* MainformImpl */

MainformImpl::MainformImpl( QWidget * parent, Qt::WFlags f ) : QMainWindow( parent, f ) {
	setupUi(this);
	d = new PrivateMainformImpl( this );

	// Update the status bar position
	d->setEditorPosition( 1, 1 );

	// Restore windows property
	d->readWindowSettings();
}

MainformImpl::~MainformImpl() {
	delete d;
}

void MainformImpl::closeEvent( QCloseEvent *event ) {
	if( XINXProjectManager::self()->project() ) {
		if( ! d->closeProject( XINXConfig::self()->config().project.saveWithSessionByDefault ) ) {
			event->ignore();
			return;
		}
	} else if( ! closeAllFile() ) {
		XINXConfig::self()->config().project.lastOpenedProject = QString(); // Pas de projet ouvert
		event->ignore();
		return;
	}
	d->storeWindowSettings();
	event->accept();
}

void MainformImpl::newTemplate() {
	Q_ASSERT( m_tabEditors->currentEditor() != NULL );
	if( TabEditor::isTextFileEditor( m_tabEditors->currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( m_tabEditors->currentEditor() );
		QString selectedText = editor->textEdit()->textCursor().selectedText();

		SnipetDialogImpl dlg( selectedText );
		if( dlg.exec() == QDialog::Accepted ) {
			Snipet s = dlg.getSnipet();
			SnipetList list = SnipetListManager::self()->snipets();
			list.append( s );
			SnipetListManager::self()->setSnipets( list );

			SnipetListManager::self()->saveToSnipetFile();
		}

	}
}

void MainformImpl::openFile( const QString & filename ) {
	Q_ASSERT( ! filename.isEmpty() );

	// Add recent action
	if( XINXProjectManager::self()->project() ) {
		XINXProjectManager::self()->project()->session()->lastOpenedFile().removeAll( filename );
		XINXProjectManager::self()->project()->session()->lastOpenedFile().prepend( filename );

		while( XINXProjectManager::self()->project()->session()->lastOpenedFile().size() > MAXRECENTFILES )
			XINXProjectManager::self()->project()->session()->lastOpenedFile().removeLast();
	}

	// Load the file in the editor
	m_tabEditors->createEditor( XinxPluginsLoader::self()->matchedFileType( filename ), QDir::fromNativeSeparators( filename ) );
	d->updateRecentFiles();
	d->updateActions();
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainformImpl::saveFileAs( const QString & name ) {
	qobject_cast<TextFileEditor*>( m_tabEditors->currentEditor() )->saveToFile( name );
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
			AbstractEditor * editor = m_tabEditors->editor( i );
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
	NewProjectWizard wizard;
	if( wizard.exec() ) {
		XSLProject * project = wizard.createProject();

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
}

void MainformImpl::openProject( const QString & filename ) {
	Q_ASSERT( ! filename.isEmpty() );

	if( XINXProjectManager::self()->project() )
		d->closeProject();
	else
		closeAllFile();

	XINXConfig::self()->config().project.recentProjectFiles.removeAll( filename );

	XSLProject * project = NULL;
	try {
		project = new XSLProject( filename );
		d->m_lastProjectOpenedPlace = QFileInfo( filename ).absolutePath();
		SpecifiqueDialogImpl::setLastPlace( project->projectPath() );

		XINXConfig::self()->config().project.recentProjectFiles.prepend( filename );
		while( XINXConfig::self()->config().project.recentProjectFiles.size() > MAXRECENTFILES )
			XINXConfig::self()->config().project.recentProjectFiles.removeLast();

		m_tabEditors->setUpdatesEnabled( false );
		foreach( XSLProjectSessionEditor * data, project->session()->serializedEditors() ) {
			AbstractEditor * editor = AbstractEditor::deserializeEditor( data );
			if( editor )
				m_tabEditors->newTextFileEditor( editor );
			else
				qWarning( qPrintable( tr("Error when restoring editor for %1").arg( data->readProperty( "FileName" ).toString() ) ) );
		}
		if( m_tabEditors->count() > 0 ) m_tabEditors->setCurrentIndex( 0 );
		m_tabEditors->setUpdatesEnabled( true );

		XINXProjectManager::self()->deleteProject();
		XINXProjectManager::self()->setCurrentProject( project );
		d->m_projectDock->setProjectPath( XINXProjectManager::self()->project() );

		foreach( XinxPluginElement e, XinxPluginsLoader::self()->plugins() ) {
			if( e.isActivated() && (! qobject_cast<IXinxPlugin*>( e.plugin() )->initializeProject( XINXProjectManager::self()->project() ) ))
				qWarning( qPrintable(tr("Can't start a project for plugin \"%1\"").arg( qobject_cast<IXinxPlugin*>( e.plugin() )->getPluginAttribute( IXinxPlugin::PLG_NAME ).toString() )) );
		}
	} catch( XSLProjectException e ) {
		delete project;
		if( ( ! e.startWizard() ) || (! QProcess::startDetached( QDir( QApplication::applicationDirPath() ).absoluteFilePath( "xinxprojectwizard" ), QStringList() << filename ) ) )
			QMessageBox::warning( this, tr("Can't open project"), e.getMessage() );
	}
}

void MainformImpl::closeProjectNoSessionData() {
	d->closeProject( false );
}

void MainformImpl::closeProjectWithSessionData() {
	d->closeProject( true );
}

void MainformImpl::saveProject( bool withSessionData ) {
	Q_ASSERT( XINXProjectManager::self()->project() );

	qDeleteAll( XINXProjectManager::self()->project()->session()->serializedEditors() );
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		m_tabEditors->editor( i )->serialize( new XSLProjectSessionEditor( XINXProjectManager::self()->project()->session() ), withSessionData );
	}
	XINXProjectManager::self()->project()->saveOnlySession();
}

void MainformImpl::updateFromVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		d->m_logDock->init();
		d->m_rcsExecute = true;
		if( list.count() == 0 )
			rcs->update( QStringList() << XINXProjectManager::self()->project()->projectPath() );
		else
			rcs->update( list );
		d->updateActions();
		d->m_rcsVisible = d->m_logDock->isVisible();
		d->m_logDock->show();
	}
}

void MainformImpl::commitToVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		QString changeLog;
		CommitMessageDialogImpl dlg;
		QStringList listOfFile = list;
		RCS::FilesOperation operations;

		if( listOfFile.count() == 0 )
			listOfFile << XINXProjectManager::self()->project()->projectPath();

		operations = rcs->operations( listOfFile );

		if( XINXConfig::self()->config().rcs.createChangelog ) {
			changeLog = QDir( XINXProjectManager::self()->project()->projectPath() ).absoluteFilePath( "changelog" );
			if( QFile::exists( changeLog ) )
				operations << qMakePair(changeLog, RCS::Commit);
			else
				operations << qMakePair(changeLog, RCS::AddAndCommit);
		}

		dlg.setFilesOperation( operations );

		if( ! dlg.exec() ) return ;
		QString message = dlg.messages();

		if( XINXConfig::self()->config().rcs.createChangelog ) {
			QFile changeLogFile( changeLog );
			if( changeLogFile.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) ) {
				QTextStream stream( &changeLogFile );
				stream << QDate::currentDate().toString( Qt::ISODate ) << " " << QTime::currentTime().toString( Qt::ISODate ) << " : ";
				if( message.isEmpty() )
					stream << tr( "<Commit with no text>" );
				else
					stream << message;
				stream << endl;
			}
		}

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		d->m_logDock->init();
		d->m_rcsExecute = true;

		rcs->commit( dlg.filesOperation(), message );
		d->updateActions();
		d->m_rcsVisible = d->m_logDock->isVisible();
		d->m_logDock->show();
	}
}

void MainformImpl::addFilesToVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		d->m_logDock->init();
		d->m_rcsExecute = true;
		rcs->add( list );
		d->updateActions();
		d->m_rcsVisible = d->m_logDock->isVisible();
		d->m_logDock->show();
	}
}

void MainformImpl::removeFilesFromVersionManager( const QStringList & list ) {
	RCS * rcs = d->m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), d->m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), d, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		d->m_logDock->init();
		d->m_rcsExecute = true;
		foreach( QString file, list ) {
			d->m_projectDock->removeFile( file );
		}
		rcs->remove( list );
		d->updateActions();
		d->m_rcsVisible = d->m_logDock->isVisible();
		d->m_logDock->show();
	}
}
