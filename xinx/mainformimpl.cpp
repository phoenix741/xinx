/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "mainformimpl.h"
#include "filecontentdockwidget.h"
#include "projectdirectorydockwidget.h"
#include "xmlpresentationdockwidget.h"
#include "replacedialogimpl.h"
#include "logdialogimpl.h"
#include "snipet.h"
#include "snipetdockwidget.h"
#include "snipet.h"
#include "snipetlist.h"
#include "snipetdialog.h"
#include "aboutdialogimpl.h"
#include "customdialogimpl.h"
#include "projectpropertyimpl.h"
#include "commitmessagedialogimpl.h"
#include "uniqueapplication.h"
#include "runsnipetdialogimpl.h"
#include "specifiquedlgimpl.h"
#include "newprojectwizard.h"
#include "scriptmanager.h"
#include "searchfilethread.h"
#include "dbus/orgshadowarexinxadaptor.h"
#include <core/xinxproject.h>
#include <core/exceptions.h>
#include <core/xinxthread.h>
#include <plugins/xinxpluginsloader.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <rcs/rcs.h>

// Qt header
#include <QObject>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QTextCursor>
#include <QTimer>
#include <QMessageBox>
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

/* Meta type */

Q_DECLARE_METATYPE(Snipet*);

/* MainformImpl */


MainformImpl::MainformImpl( QWidget * parent, Qt::WFlags f ) : QMainWindow( parent, f ),  m_lastProjectOpenedPlace( QDir::currentPath() ), m_rcsExecute( false ), m_headContent( QString() ), m_closeTabBtn(0) {
	setupUi(this);

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

	// Attache to main application
	qobject_cast<UniqueApplication*>(qApp)->attachMainWindow( this );

	// Update the status bar position
	setEditorPosition( 1, 1 );

	// Restore windows property
	readWindowSettings();

	startTimer( 1000 );
}

MainformImpl::~MainformImpl() {

}

void MainformImpl::registerTypes() {
	qRegisterMetaType<TextFileEditor>( "TextFileEditor" );
}

void MainformImpl::createDockWidget() {
	m_projectDock = new ProjectDirectoryDockWidget( tr("Project Directory"), this );
	m_projectDock->setObjectName( QString::fromUtf8("m_projectDock") );
	m_projectDock->setGlobalUpdateAction( m_globalUpdateFromRCSAct );
	m_projectDock->setGlobalCommitAction( m_globalCommitToRCSAct );
	m_projectDock->setSelectedUpdateAction( m_selectedUpdateFromRCSAct );
	m_projectDock->setSelectedCommitAction( m_selectedCommitToRCSAct );
	m_projectDock->setSelectedAddAction( m_selectedAddToRCSAct );
	m_projectDock->setSelectedRemoveAction( m_selectedRemoveFromRCSAct );
	m_projectDock->setSelectedCompareWithHeadAction( m_compareWithHeadAct );
	m_projectDock->setSelectedCompareWithStdAction( m_compareWithStdAct );
	m_projectDock->setSelectedCompareAction( m_compareTwoFileAct );
	m_projectDock->setToggledViewAction( m_toggledFlatView );
	addDockWidget( Qt::LeftDockWidgetArea, m_projectDock );
	QAction * action = m_projectDock->toggleViewAction();
	action->setShortcut( QKeySequence( "Alt+1" ) );
	m_windowsMenu->addAction( action );

	m_contentDock = new FileContentDockWidget( tr("File Content"), this );
	m_contentDock->setObjectName( QString::fromUtf8("m_contentDock") );
	addDockWidget( Qt::LeftDockWidgetArea, m_contentDock );
	action = m_contentDock->toggleViewAction();
	action->setShortcut( QKeySequence( "Alt+2" ) );
	m_windowsMenu->addAction( action );

	m_xmlpresentationdock = new XmlPresentationDockWidget( tr("XML Presentation"), this );
	m_xmlpresentationdock->setObjectName( QString::fromUtf8( "m_xmlpresentationdock" ) );
	addDockWidget( Qt::RightDockWidgetArea, m_xmlpresentationdock );
	action = m_xmlpresentationdock->toggleViewAction();
	action->setShortcut( QKeySequence( "Alt+3" ) );
	m_windowsMenu->addAction( action );

	m_snipetsDock = new SnipetDockWidget( tr("Snipets"), this );
	m_snipetsDock->setObjectName( QString::fromUtf8( "m_snipetsDock" ) );
	addDockWidget( Qt::RightDockWidgetArea, m_snipetsDock );
	action = m_snipetsDock->toggleViewAction();
	action->setShortcut( QKeySequence( "Alt+4" ) );
	m_windowsMenu->addAction( action );

	// Load dock from plugins and assign automatic shortcut
	int dockShortcut = 5;
	foreach( XinxPluginElement * pluginElement, XinxPluginsLoader::self()->plugins() ) {
		IDockPlugin * dockPlugin = qobject_cast<IDockPlugin*>( pluginElement->plugin() );
		if( pluginElement->isActivated() && dockPlugin ) {
			QDockWidget * dock = dockPlugin->createDockWidget( this );
			addDockWidget( Qt::RightDockWidgetArea, dock );
			action = dock->toggleViewAction();
			action->setShortcut( QString( "Alt+%1" ).arg( dockShortcut++ ) );
			m_windowsMenu->addAction( action );
		}
	}

	m_logDock = new LogDockWidget( tr("Log"), this );
	connect( m_logDock, SIGNAL(open(QString,int)), this, SLOT(openFile(QString,int)) );
	m_logDock->setObjectName( QString::fromUtf8("m_logDock") );
	addDockWidget( Qt::BottomDockWidgetArea, m_logDock );
	action = m_logDock->toggleViewAction();
	m_windowsMenu->addAction( action );
}

void MainformImpl::createSubMenu() {
	// New sub menu
	m_newMenu = new QMenu( this );
	createNewSubMenu();
	m_newAct->setData( QVariant::fromValue<IFileTypePlugin*>( XinxPluginsLoader::self()->fileTypes()[0] ) );
	connect( m_newAct, SIGNAL(triggered()), this, SLOT(newFile()) );

	m_newAct->setMenu( m_newMenu );
	QToolButton * btn = qobject_cast<QToolButton*>( m_fileToolBar->widgetForAction( m_newAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );

	// Recent project menu
	QMenu * recentProjectMenu = new QMenu( this );
	recentProjectMenu->addAction( m_openProjectAct );
	setupRecentMenu( recentProjectMenu, m_recentSeparator, m_recentProjectActs );
	m_recentProjectAct->setMenu( recentProjectMenu );
	btn = qobject_cast<QToolButton*>( m_projectToolBar->widgetForAction( m_recentProjectAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );

	// Recent file menu
	QMenu * recentFileMenu = new QMenu( this );
	recentFileMenu->addAction( m_openAct );
	setupRecentMenu( recentFileMenu, m_recentFileSeparator, m_recentFileActs );
	m_recentFileAct->setMenu( recentFileMenu );
	btn = qobject_cast<QToolButton*>( m_fileToolBar->widgetForAction( m_recentFileAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );

	// Close project sub menu
	QMenu * closeProjectMenu = new QMenu( this );
	closeProjectMenu->addAction( m_closeProjectWithSessionAct );
	closeProjectMenu->addAction( m_closeProjectNoSessionAct );

	m_closeProjectAct->setMenu( closeProjectMenu );
	btn = qobject_cast<QToolButton*>( m_projectToolBar->widgetForAction( m_closeProjectAct ) );
	if( btn )
		btn->setPopupMode( QToolButton::MenuButtonPopup );
}

void MainformImpl::createNewSubMenu() {
	QList<QAction*> actions = m_newMenu->actions();
	m_newMenu->clear();
	qDeleteAll( actions );

	foreach( IFileTypePlugin * plugin, XinxPluginsLoader::self()->fileTypes() ) {
		QAction * action = new QAction(
		        QIcon( plugin->icon() ),
				tr( "New %1" ).arg( plugin->description() ),
				this
				);
		action->setData( QVariant::fromValue<IFileTypePlugin*>( plugin ) );
		m_newMenu->addAction( action );

		connect( action, SIGNAL(triggered()), this, SLOT(newFile()) );
	}
}

void MainformImpl::createStatusBar() {
	m_codecLabel = new QLabel( tr("Unknown") );
	m_lineFeedLabel = new QLabel( tr("Unknown") );
	m_editorPosition = new QLabel( "000x000" );
	m_threadCount = new QLabel( "000 (000)" );
	statusBar()->addPermanentWidget( m_codecLabel );
	statusBar()->addPermanentWidget( m_lineFeedLabel );
	statusBar()->addPermanentWidget( m_editorPosition );
	statusBar()->addPermanentWidget( m_threadCount );
	statusBar()->showMessage( tr("Ready"), 2000 );
}

void MainformImpl::createShortcut() {
	// File menu
	m_newAct->setShortcut( QKeySequence::New );
	m_recentFileAct->setShortcut( QKeySequence::Open );
	m_saveAct->setShortcut( QKeySequence::Save );
	m_closeAct->setShortcut( QKeySequence::Close );
	m_printAct->setShortcut( QKeySequence::Print );

	m_exitAct->setShortcut( QKeySequence::mnemonic( m_exitAct->text() ) );

	// Edit menu
	m_undoAct->setShortcut( QKeySequence::Undo );
	m_redoAct->setShortcut( QKeySequence::Redo );

	m_cutAct->setShortcut( QKeySequence::Cut );
	m_copyAct->setShortcut( QKeySequence::Copy );
	m_pasteAct->setShortcut( QKeySequence::Paste );

	m_selectAllAct->setShortcut( QKeySequence::SelectAll );

	m_upperTextAct->setShortcut( QKeySequence::mnemonic( m_upperTextAct->text() ) );
	m_lowerTextAct->setShortcut( QKeySequence::mnemonic( m_lowerTextAct->text() ) );

	m_duplicateLineAct->setShortcut( QKeySequence( "Ctrl+D" ) );
	m_indentAct->setShortcut( QKeySequence( "Tab" ) );
	m_unindentAct->setShortcut( QKeySequence( "Shift+Tab" ) );
	m_commentLineAct->setShortcut( QKeySequence( "Ctrl+Shift+C" ) );
	m_uncommentLineAct->setShortcut( QKeySequence( "Ctrl+Shift+D" ) );
	m_moveUpLineAct->setShortcut( QKeySequence( "Ctrl+Shift+Up" ) );
	m_moveDownLineAct->setShortcut( QKeySequence( "Ctrl+Shift+Down" ) );

	m_prettyPrintAct->setShortcut( QKeySequence( "Ctrl+Shift+F" ) );
	m_completeAct->setShortcut( QKeySequence( "Ctrl+E" ) );
	m_highlightWord->setShortcut( QKeySequence( "Ctrl+U" ) );

	// Bookmark menu
	m_bookmarkAct->setShortcut( QKeySequence( "Ctrl+B" ) );
	m_nextBookmarkAct->setShortcut( QKeySequence( "F2" ) );
	m_previousBookmarkAct->setShortcut( QKeySequence( "Shift+F2" ) );
	m_clearAllBookmarkAct->setShortcut( QKeySequence( "Ctrl+F2" ) );

	// Search menu
	m_searchAct->setShortcut( QKeySequence::Find );
	m_searchNextAct->setShortcut( QKeySequence::FindNext );
	m_searchPreviousAct->setShortcut( QKeySequence::FindPrevious );
	m_replaceAct->setShortcut( QKeySequence::Replace );

	// Project menu
	m_globalUpdateFromRCSAct->setShortcut( QKeySequence::Refresh );
	m_globalCommitToRCSAct->setShortcut( QKeySequence( "F6" ) );
	m_cancelRCSOperationAct->setShortcut( QKeySequence( "Escape" ) );

	// Windows menu
	m_nextTabAct->setShortcut( QKeySequence::NextChild );
	m_previousTabAct->setShortcut( QKeySequence::PreviousChild );
}

void MainformImpl::createActions() {
	/* FILE */

	// Open
	connect( m_openAct, SIGNAL(triggered()), this, SLOT(openFile()) );
	connect( m_recentFileAct, SIGNAL(triggered()), this, SLOT(openFile()) );
	connect( m_contentDock, SIGNAL(open(QString,int)), this, SLOT(openFile(QString,int)) );
	connect( m_projectDock, SIGNAL(open(QString)), this, SLOT(openFile(QString)) );
	connect( m_tabEditors, SIGNAL(modelChanged(QAbstractItemModel*)), m_contentDock, SLOT(updateModel(QAbstractItemModel*)) );
	connect( m_tabEditors, SIGNAL(positionChanged(QModelIndex)), m_contentDock, SLOT(positionChanged(QModelIndex)) );
	connect( m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)) );

	// Recent open action
	for(int i = 0; i < MAXRECENTFILES; i++) {
		connect( m_recentProjectActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()) );
		connect( m_recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()) );
	}

	// Refresh
	m_tabEditors->setRefreshAction( m_refreshFileAct );
	connect( m_refreshFileAct, SIGNAL(triggered()), this, SLOT(refreshFile()) );

	// Save
	m_tabEditors->setSaveAction( m_saveAct );
	m_tabEditors->setSaveAsAction( m_saveAsAct );
	connect( m_saveAct, SIGNAL(triggered()), this, SLOT(saveFile()) );
	connect( m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAsFile()) );
	connect( m_saveAllAct, SIGNAL(triggered()), this, SLOT(saveAllFile()) );

	// Print
	connect( m_printAct, SIGNAL(triggered()), this, SLOT(printFile()) );

	// Close
	m_tabEditors->setCloseAction( m_closeAct );
	connect( m_closeAct, SIGNAL(triggered()), this, SLOT(closeFile()) );
	connect( m_closeAllAct, SIGNAL(triggered()), this, SLOT(closeAllFile()) );

	// Tab Editor
	connect( m_tabEditors, SIGNAL(setEditorPosition(int,int)), this, SLOT(setEditorPosition(int,int)) );
	connect( m_tabEditors, SIGNAL(contentChanged()), this, SLOT(updateEditorInformations()) );

	/* EDIT */
	// Undo
	connect( m_undoAct, SIGNAL(triggered()), m_tabEditors, SLOT(undo()) );
	m_undoAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(undoAvailable(bool)), m_undoAct, SLOT(setEnabled(bool)) );

	// Redo
	connect( m_redoAct, SIGNAL(triggered()), m_tabEditors, SLOT(redo()) );
	m_redoAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(redoAvailable(bool)), m_redoAct, SLOT(setEnabled(bool)) );

	// Cut
	connect( m_cutAct, SIGNAL(triggered()), m_tabEditors, SLOT(cut()) );
	m_cutAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(copyAvailable(bool)), m_cutAct, SLOT(setEnabled(bool)) );

	// Copy
	connect( m_copyAct, SIGNAL(triggered()), m_tabEditors, SLOT(copy()) );
	m_copyAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(copyAvailable(bool)), m_copyAct, SLOT(setEnabled(bool)) );

	connect( m_copyFileNameAct, SIGNAL(triggered()), this, SLOT(copyFileName()) );
	m_tabEditors->setCopyFileNameAction( m_copyFileNameAct );
	connect( m_copyPathAct, SIGNAL(triggered()), this, SLOT(copyPath()) );
	m_tabEditors->setCopyPathAction( m_copyPathAct );

	// Paste
	connect( m_pasteAct, SIGNAL(triggered()), m_tabEditors, SLOT(paste()) );
	m_pasteAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(pasteAvailable(bool)), m_pasteAct, SLOT(setEnabled(bool)) );

	// Select All
	connect( m_selectAllAct, SIGNAL(triggered()), m_tabEditors, SLOT(selectAll()) );
	m_selectAllAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_selectAllAct, SLOT(setEnabled(bool)) );

	// Duplicate Line
	connect( m_duplicateLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(duplicateCurrentLine()) );
	m_duplicateLineAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_duplicateLineAct, SLOT(setEnabled(bool)) );

	// Move Line Up
	connect( m_moveUpLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineUp()) );
	m_moveUpLineAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_moveUpLineAct, SLOT(setEnabled(bool)) );

	// Move Line Down
	connect( m_moveDownLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineDown()) );
	m_moveDownLineAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_moveDownLineAct, SLOT(setEnabled(bool)) );

	// Upper Case
	connect( m_upperTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(upperSelectedText()) );
	m_upperTextAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(hasTextSelection(bool)), m_upperTextAct, SLOT(setEnabled(bool)) );

	// Lower Case
	connect( m_lowerTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(lowerSelectedText()) );
	m_lowerTextAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(hasTextSelection(bool)), m_lowerTextAct, SLOT(setEnabled(bool)) );

	// Show space and Tabulation
	m_showSpaceAndTabAct->setChecked( XINXConfig::self()->config().editor.showTabulationAndSpace );
	connect( m_showSpaceAndTabAct, SIGNAL(triggered()), this, SLOT(updateSpaceAndTab()) );

	// Comment
	connect( m_commentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(commentSelectedText()) );
	m_commentLineAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(hasTextSelection(bool)), m_commentLineAct, SLOT(setEnabled(bool)) );

	// Uncomment
	connect( m_uncommentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(uncommentSelectedText()) );
	m_uncommentLineAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(hasTextSelection(bool)), m_uncommentLineAct, SLOT(setEnabled(bool)) );

	// Complete, Don't need, process by the editor directly
	connect( m_completeAct, SIGNAL(triggered()), m_tabEditors, SLOT(complete()) );
	m_completeAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_completeAct, SLOT(setEnabled(bool)) );

	// Highlight Same Word
	connect( m_highlightWord, SIGNAL(triggered()), m_tabEditors, SLOT(highlightWord()) );

	// Search
	connect( m_searchAct, SIGNAL(triggered()), this, SLOT(find()) );

	// Search next/previous
	connect( m_searchNextAct, SIGNAL(triggered()), this, SLOT(findNext()) );
	m_searchNextAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_searchNextAct, SLOT(setEnabled(bool)));
	connect( m_searchPreviousAct, SIGNAL(triggered()), this, SLOT(findPrevious()) );
	m_searchPreviousAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_searchPreviousAct, SLOT(setEnabled(bool)));

	// Replace
	connect( m_replaceAct, SIGNAL(triggered()), this, SLOT(replace()) );
	m_replaceAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_replaceAct, SLOT(setEnabled(bool)) );

	// Indent
	connect( m_indentAct, SIGNAL(triggered()), m_tabEditors, SLOT(indent()) );
	m_indentAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_indentAct, SLOT(setEnabled(bool)) );

	// Unindent
	connect( m_unindentAct, SIGNAL(triggered()), m_tabEditors, SLOT(unindent()) );
	m_unindentAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_unindentAct, SLOT(setEnabled(bool)) );

	// Autoindent
	connect( m_prettyPrintAct, SIGNAL(triggered()), m_tabEditors, SLOT(autoindent()) );
	m_prettyPrintAct->setEnabled(false);
	connect( m_tabEditors, SIGNAL(textAvailable(bool)), m_prettyPrintAct, SLOT(setEnabled(bool)) );

	/* Bookmark */
	connect( m_bookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(bookmark()) );
	connect( m_nextBookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(nextBookmark()) );
	connect( m_previousBookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(previousBookmark()) );
	connect( m_clearAllBookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(clearAllBookmark()) );

	/* PROJECT */
	connect( m_globalUpdateFromRCSAct, SIGNAL(triggered()), this, SLOT(globalUpdateFromVersionManager()) );
	connect( m_globalCommitToRCSAct, SIGNAL(triggered()), this, SLOT(globalCommitToVersionManager()) );
	connect( m_selectedUpdateFromRCSAct, SIGNAL(triggered()), this, SLOT(selectedUpdateFromVersionManager()) );
	connect( m_selectedCommitToRCSAct, SIGNAL(triggered()), this, SLOT(selectedCommitToVersionManager()) );
	connect( m_selectedAddToRCSAct, SIGNAL(triggered()), this, SLOT(selectedAddToVersionManager()) );
	connect( m_selectedRemoveFromRCSAct, SIGNAL(triggered()), this, SLOT(selectedRemoveFromVersionManager()) );
	connect( m_compareWithHeadAct, SIGNAL(triggered()), this, SLOT(selectedCompareWithVersionManager()) );
	connect( m_compareWithStdAct, SIGNAL(triggered()), this, SLOT(selectedCompareWithStd()) );
	connect( m_compareTwoFileAct, SIGNAL(triggered()), this, SLOT(selectedCompare()) );

	connect( m_newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()) );
	connect( m_openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_recentProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_projectPropertyAct, SIGNAL(triggered()), this, SLOT(projectProperty()) );
	connect( m_closeProjectAct, SIGNAL(triggered()), this, SLOT(closeProject()) );
	connect( m_closeProjectNoSessionAct, SIGNAL(triggered()), this, SLOT(closeProjectNoSessionData()) );
	connect( m_closeProjectWithSessionAct, SIGNAL(triggered()), this, SLOT(closeProjectWithSessionData()) );

	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateActions()) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateTitle()) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateRecentProjects()) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateRecentFiles()) );

	/* WINDOWS */
	connect( m_toggledFlatView, SIGNAL(toggled(bool)), m_projectDock, SLOT(toggledView(bool)) );
	connect( m_nextTabAct, SIGNAL(triggered()), this, SLOT(nextTab()) );
	connect( m_previousTabAct, SIGNAL(triggered()), this, SLOT(previousTab()) );

	/* Tools */
	// Create Template
	connect( m_createTemplate, SIGNAL(triggered()), this, SLOT(newTemplate()) );
	connect( m_refreshSnipet, SIGNAL(triggered()), SnipetListManager::self(), SLOT(loadFromSnipetFile()) );
	connect( m_refreshScripts, SIGNAL(triggered()), ScriptManager::self(), SLOT(loadScripts()) );

	// Customize
	connect( m_customApplicationAct, SIGNAL(triggered()), this, SLOT(customize()) );

  	// About Qt
	connect( m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// About
	connect( m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	// What's This ?
	m_helpMenu->addAction( QWhatsThis::createAction() );

	// Drag & Drop
	connect( m_tabEditors, SIGNAL(fileOpened(QString)), this, SLOT(updateActions()) );

	// Timer LOG
	m_timer = new QTimer( this );
	connect( m_timer, SIGNAL(timeout()), this, SLOT(logTimeout()) );
}

void MainformImpl::connectDbus() {
	new XinxAdaptor( this );
	QDBusConnection::sessionBus().registerObject( "/", this );
}

void MainformImpl::createTools() {
	connect( SnipetListManager::self(), SIGNAL(listChanged()), this, SLOT(updateToolsMenu()) );
	connect( ScriptManager::self(), SIGNAL(changed()), this, SLOT(updateToolsMenu()) );
	updateToolsMenu();
}

void MainformImpl::callSnipetMenu() {
	Q_ASSERT( m_tabEditors->currentEditor() != NULL );

	QAction * action = qobject_cast<QAction*>( sender() );
	if( action && TabEditor::isTextFileEditor( m_tabEditors->currentEditor() ) ) {
		const Snipet & snipet = action->data().value<Snipet>();

		RunSnipetDialogImpl dlg( snipet );
		if( dlg.exec() ) {
			XinxCodeEdit * editor = static_cast<TextFileEditor*>( m_tabEditors->currentEditor() )->textEdit();
			editor->insertText( dlg.getResult() );
		}
	}
}

void MainformImpl::callScriptAction() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action ) {
		ScriptValue qsScript = action->data().value<ScriptValue>();
		qsScript.callScript();
	}
}

void MainformImpl::updateToolsMenu() {
	/* Snipet */
	qDeleteAllLater( m_snipetActs );
	m_snipetActs.clear();

	qDeleteAllLater( m_snipetCategoryActs.values() );
	m_snipetCategoryActs.clear();

	if( SnipetListManager::self()->snipets().count() > 0 ) {
		foreach( const QString & category, SnipetListManager::self()->snipets().categories() ) {
			QAction * act = new QAction( category, this );
			m_snipetCategoryActs[ category ] = act;
			act->setMenu( new QMenu( this ) );
		}
		for( int i = 0 ; i < SnipetListManager::self()->snipets().count() ; i++ ) {
			const Snipet & snipet = SnipetListManager::self()->snipets().at( i );
			QAction * act = new QAction( QIcon( snipet.icon() ), snipet.name(), this );
			m_snipetActs.append( act );
			m_snipetCategoryActs[ snipet.category() ]->menu()->addAction( act );
			act->setData( QVariant::fromValue( snipet ) );
			connect( act, SIGNAL(triggered()), this, SLOT(callSnipetMenu()) );
		}
		m_templateMenu->insertActions( m_createTemplate, m_snipetCategoryActs.values() );
	}

	/* Scripts */
	qDeleteAllLater( m_scriptActs );
	m_scriptActs.clear();

	foreach( const ScriptValue & s, ScriptManager::self()->objects() ) {
		QAction * action = new QAction( this );
		action->setText( s.text() );
		action->setData( QVariant::fromValue( s ) );
		m_scriptMenu->addAction( action );

		connect( action, SIGNAL(triggered()), this, SLOT(callScriptAction()) );
		m_scriptActs.append( action );
	}

	updateActions();
}

void MainformImpl::currentTabChanged( int index ) {
	AbstractEditor * editor = m_tabEditors->editor( index );
	ScriptManager::self()->setCurrentEditeur( editor );
	m_snipetsDock->setEditor( editor );
	updateEditorInformations();
}

void MainformImpl::updateEditorInformations() {
	AbstractEditor * editor = m_tabEditors->currentEditor();
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

void MainformImpl::newFile() {
	if( qobject_cast<QAction*>( sender() ) ) {
		IFileTypePlugin* plugin = qobject_cast<QAction*>( sender() )->data().value<IFileTypePlugin*>();
		m_tabEditors->createEditor( plugin );
		updateActions();
	}
}

void MainformImpl::newFile( const QString &filename ) {
	IFileTypePlugin * plugin = XinxPluginsLoader::self()->matchedFileType( filename );
	// TabEditor gère le cas où le plugins est null ....
	m_tabEditors->createEditor( plugin ); // Pas de nom de fichier car on créé un nouveau fichier
}

void MainformImpl::openRecentProject() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		openProject( action->data().toString() );
}

void MainformImpl::openRecentFile() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		openFile( action->data().toString() );
}

void MainformImpl::openFile( const QString & name, int line ) {
	if( !name.isEmpty() )
		openFile( name );

	// Deplace to rigth line.
	TextFileEditor * ed = qobject_cast<TextFileEditor*>( m_tabEditors->currentEditor() );
	ed->textEdit()->gotoLine( line );
	ed->textEdit()->setFocus( Qt::OtherFocusReason );
}

void MainformImpl::openFile() {
	Q_ASSERT( XINXConfig::self() );

	QStringList selectedFiles = QFileDialog::getOpenFileNames( this, tr("Open text file"), SpecifiqueDialogImpl::lastPlace(), XinxPluginsLoader::self()->openDialogBoxFilters().join(";;") );

	m_tabEditors->setUpdatesEnabled( false );
	foreach( const QString & filename, selectedFiles ) {
		SpecifiqueDialogImpl::setLastPlace( QFileInfo( filename ).absolutePath() );
		openFile( filename );
	}
	m_tabEditors->setUpdatesEnabled( true );
}

void MainformImpl::currentCloseFile() {
	fileEditorClose( m_tabEditors->currentIndex() );
}

void MainformImpl::fileEditorRefreshFile( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_tabEditors->count() );

	AbstractEditor * ed = qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) );
	if( ed ) {
		bool act = ! ed->isModified();

		if( ! act ) {
			QMessageBox::StandardButton ret;
			ret = QMessageBox::question( this, tr("Application"),
										tr("The document %1 has been modified.\n"
										   "Do you really want refresh this?").arg( ed->getTitle() ),
										QMessageBox::Yes | QMessageBox::No);
			act = ( ret == QMessageBox::Yes );
		}
		if( act ) {
			ScriptManager::self()->callScriptsBeforeLoad();
			ed->loadFromFile();
			ScriptManager::self()->callScriptsAfterLoad();
		}
	}
}

void MainformImpl::refreshFile() {
	int item = m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_tabEditors->currentIndex();
	fileEditorRefreshFile( item );
}

void MainformImpl::saveFile() {
	int item = m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_tabEditors->currentIndex();
	fileEditorSave( item );
}

void MainformImpl::saveAsFile() {
	int item = m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_tabEditors->currentIndex();
	fileEditorSaveAs( item );
}

void MainformImpl::about() {
	AboutDialogImpl about( this );
	about.exec();
}

void MainformImpl::customize() {
	CustomDialogImpl custom( this );
	custom.loadFromConfig( XINXConfig::self() );

	if( custom.exec() ) {
		custom.saveToConfig( XINXConfig::self() );
		XINXConfig::self()->updateFormatsSchemeFromConfig();
		XINXConfig::self()->save();
		SnipetListManager::self()->saveToSnipetFile();

		if( ! XINXConfig::self()->config().style.isEmpty() ) {
			QApplication::setStyle( XINXConfig::self()->config().style );
		}
	}
}

void MainformImpl::setupRecentMenu( QMenu * menu, QAction * & separator, QAction * recentActions[ MAXRECENTFILES ] ) {
	separator = menu->addSeparator();
	separator->setVisible( false );
	for(int i = 0; i < MAXRECENTFILES; i++) {
		recentActions[i] = new QAction( this );
		recentActions[i]->setVisible( false );
		menu->addAction( recentActions[i] );
	}
}

void MainformImpl::updateRecentFiles() {
	int numRecentFiles;
	if( XINXProjectManager::self()->project() ) {
		numRecentFiles = qMin( XINXProjectManager::self()->project()->session()->lastOpenedFile().size(), MAXRECENTFILES );

		for( int i = 0; i < numRecentFiles; i++ ) {
			QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( XINXProjectManager::self()->project()->session()->lastOpenedFile()[i] ).fileName() );
			if( XinxPluginsLoader::self()->matchedFileType( QFileInfo( text ).fileName() ) )
				m_recentFileActs[i]->setIcon( QIcon( XinxPluginsLoader::self()->matchedFileType( QFileInfo( text ).fileName() )->icon() ) );
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

void MainformImpl::updateRecentProjects() {
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

void MainformImpl::readWindowSettings() {
	resize( XINXConfig::self()->config().size );
	move( XINXConfig::self()->config().position );
	if( XINXConfig::self()->config().maximized )
		setWindowState( windowState() ^ Qt::WindowMaximized );

	if( ! XINXConfig::self()->config().state.isEmpty() ) {
		if( ! restoreState( XINXConfig::self()->config().state ) )
			qWarning( "Can't restore windows state.\n" );
	}
}

void MainformImpl::storeWindowSettings() {
	XINXConfig::self()->config().state = saveState();

	XINXConfig::self()->config().position = pos();
	XINXConfig::self()->config().size = size();
	XINXConfig::self()->config().maximized = isMaximized();

	XINXConfig::self()->save();
}

void MainformImpl::createTabEditorButton() {
	m_closeTabBtn = new QToolButton( m_tabEditors );
	m_closeTabBtn->setIcon( QIcon( ":/images/tabclose.png" ) );
	connect( m_closeTabBtn, SIGNAL(clicked()), this, SLOT(currentCloseFile()) );
	if( ! XINXConfig::self()->config().editor.hideCloseTab )
		m_tabEditors->setCornerWidget( m_closeTabBtn );

	connect( XINXConfig::self(), SIGNAL(changed()), this, SLOT(updateConfigElement()) );
}

void MainformImpl::updateConfigElement() {
	if( m_showSpaceAndTabAct->isChecked() != XINXConfig::self()->config().editor.showTabulationAndSpace )
		m_showSpaceAndTabAct->setChecked( XINXConfig::self()->config().editor.showTabulationAndSpace );
	m_closeTabBtn->setVisible( ! XINXConfig::self()->config().editor.hideCloseTab );

	createNewSubMenu();
}

void MainformImpl::updateActions() {
	/* Project action */
	m_saveProjectAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_closeProjectAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_closeProjectWithSessionAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_closeProjectNoSessionAct->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_projectPropertyAct->setEnabled( XINXProjectManager::self()->project() != NULL );

	m_globalUpdateFromRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute ) );
	m_globalCommitToRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );

	m_selectedUpdateFromRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );
	m_selectedCommitToRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );
	m_selectedAddToRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );
	m_selectedRemoveFromRCSAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && ( ! m_rcsExecute )  );

	m_cancelRCSOperationAct->setEnabled( (XINXProjectManager::self()->project() != NULL) && (!XINXProjectManager::self()->project()->projectRCS().isEmpty()) && m_rcsExecute );

	m_toggledFlatView->setEnabled( XINXProjectManager::self()->project() != NULL );
	m_projectDock->setEnabled( XINXProjectManager::self()->project() != NULL );

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
	m_bookmarkAct->setEnabled( m_tabEditors->count() );
	m_nextBookmarkAct->setEnabled( m_tabEditors->count() );
	m_previousBookmarkAct->setEnabled( m_tabEditors->count() );

	if( m_tabEditors->count() == 0 ) {
		m_contentDock->updateModel( NULL );
	}
}

bool MainformImpl::fileEditorMayBeSave( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_tabEditors->count() );

	AbstractEditor * ed = qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) );
	if( ed && ed->isModified() ) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning( this, tr("Application"),
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

void MainformImpl::fileEditorSave( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_tabEditors->count() );
	Q_ASSERT( qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) ) );

	QString editorFileName = qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) )->lastFileName();
	bool hasName = ! editorFileName.isEmpty();

	if ( ! hasName ) {
		fileEditorSaveAs( index );
	} else {
		QString fileName = SpecifiqueDialogImpl::saveFileAsIfStandard( editorFileName, m_fileToAdd );
		if( ! fileName.isEmpty() ) {
			ScriptManager::self()->callScriptsBeforeSave();
			qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) )->saveToFile( fileName );
			if( m_fileToAdd.count() > 0 )
				addFilesToVersionManager( m_fileToAdd );
			m_projectDock->refreshPath( QFileInfo( fileName ).absoluteFilePath() );
			ScriptManager::self()->callScriptsAfterSave();
			statusBar()->showMessage( tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
		}
		m_fileToAdd.clear();
	}
}

void MainformImpl::fileEditorSaveAs( int index ) {
	Q_ASSERT( index >= 0 );
	Q_ASSERT( index < m_tabEditors->count() );
	Q_ASSERT( qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) ) );

	QString fileName = qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) )->lastFileName();

	fileName = SpecifiqueDialogImpl::saveFileAs( fileName,
				qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) )->defaultFileName(),
				m_fileToAdd );

	if( ! fileName.isEmpty() ) {
		ScriptManager::self()->callScriptsBeforeSave();
		qobject_cast<AbstractEditor*>( m_tabEditors->editor( index ) )->saveToFile( fileName );
		ScriptManager::self()->callScriptsAfterSave();
		if( m_fileToAdd.count() > 0 )
			addFilesToVersionManager( m_fileToAdd );
		m_projectDock->refreshPath( QFileInfo( fileName ).absoluteFilePath() );
		statusBar()->showMessage( tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
	}
	m_fileToAdd.clear();
}

void MainformImpl::fileEditorClose( int index ) {
	if( fileEditorMayBeSave( index ) ) {
		m_contentDock->updateModel( NULL );
		AbstractEditor * editor = m_tabEditors->editor( index );
		m_tabEditors->removeTab( index );
		delete editor;
	}

	updateActions();
}

void MainformImpl::printFile() {
	Q_ASSERT( m_tabEditors->currentEditor() != NULL );

	if( TabEditor::isTextFileEditor( m_tabEditors->currentEditor() ) ) {
		TextFileEditor * editor = static_cast<TextFileEditor*>( m_tabEditors->currentEditor() );
		QPrinter printer;

		QPrintDialog *dlg = new QPrintDialog( &printer, this );
		if ( dlg->exec() != QDialog::Accepted ) return;

		editor->textEdit()->print( &printer );

		delete dlg;

		statusBar()->showMessage( tr("Printed"), 2000 );
	}
}

void MainformImpl::globalUpdateFromVersionManager() {
	updateFromVersionManager();
}

void MainformImpl::globalCommitToVersionManager() {
	commitToVersionManager();
}

void MainformImpl::selectedUpdateFromVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		updateFromVersionManager( list );
}

void MainformImpl::selectedCommitToVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		commitToVersionManager( list );
}

void MainformImpl::selectedAddToVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		addFilesToVersionManager( list );
}

void MainformImpl::selectedRemoveFromVersionManager() {
	QStringList list = m_projectDock->selectedFiles();
	if( list.count() > 0 )
		removeFilesFromVersionManager( list );
}

void MainformImpl::selectedCompareWithVersionManager() {
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
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );

		m_logDock->init();
		m_rcsExecute = true;

		updateActions();
		m_rcsVisible = m_logDock->isVisible();
		m_logDock->show();
		rcs->updateToRevision( m_compareFileName, revision, &m_headContent );
	}
}

void MainformImpl::logTimeout() {
	m_timer->stop();
	m_logDock->setVisible( false );
}

void MainformImpl::rcsLogTerminated() {
	Q_ASSERT( m_projectDock->rcs() );

	if( ! m_headContent.isEmpty() ) {
		try {
			QTemporaryFile * headContentFile = new QTemporaryFile( this ); // Delete when the main windows is destroyed
			if( headContentFile->open() ) {
				headContentFile->setTextModeEnabled( true );
				QTextStream out(headContentFile);
				out << m_headContent;
			}
			QProcess::startDetached( XINXConfig::self()->getTools( "diff" ), QStringList() << m_compareFileName << headContentFile->fileName() );
			m_headContent = QString();
		} catch( ToolsNotDefinedException e ) {
			QMessageBox::warning( this, tr( "Tools" ), e.getMessage() );
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

void MainformImpl::selectedCompareWithStd() {
	try {
		QStringList list = m_projectDock->selectedFiles();
		Q_ASSERT( list.size() == 1 && XINXProjectManager::self()->project() );

		QString customFilename = list.at( 0 ), stdFilename, path, filename;
		bool isSpecifique = false;

		path = QFileInfo( customFilename ).absolutePath();
		filename = QFileInfo( customFilename ).fileName();

		foreach( const QString & prefix, XINXProjectManager::self()->project()->specifiquePrefixes() ) {
			if( filename.startsWith( prefix + "_", Qt::CaseInsensitive ) ) {
				isSpecifique = true;
				stdFilename = filename;
				stdFilename.remove( 0, prefix.size() + 1 );
				stdFilename = QDir( path ).absoluteFilePath( stdFilename );

				if( QFileInfo( stdFilename ).exists() ) {
					QProcess::startDetached( XINXConfig::self()->getTools( "diff" ), QStringList() << customFilename << stdFilename );
					return;
				}
			}
		}

		if( isSpecifique )
			QMessageBox::information( this, tr("Compare"), tr("Standard file for %1 not found or not in specifique directory.").arg( filename ), QMessageBox::Ok );
		else
			QMessageBox::information( this, tr("Compare"), tr("Not a specifique file"), QMessageBox::Ok );
	} catch( ToolsNotDefinedException e ) {
		QMessageBox::warning( this, tr( "Tools" ), e.getMessage() );
	}
}

void MainformImpl::selectedCompare() {
	try {
		QStringList list = m_projectDock->selectedFiles();
		Q_ASSERT( list.size() == 2 );
		QProcess::startDetached( XINXConfig::self()->getTools( "diff" ), QStringList() << list.at( 0 ) << list.at( 1 ) );
	} catch( ToolsNotDefinedException e ) {
		QMessageBox::warning( this, tr( "Tools" ), e.getMessage() );
	}
}


void MainformImpl::nextTab() {
	m_tabEditors->setCurrentIndex( ( m_tabEditors->currentIndex() + 1 ) % m_tabEditors->count() );
}

void MainformImpl::previousTab() {
	m_tabEditors->setCurrentIndex( ( m_tabEditors->currentIndex() - 1 + m_tabEditors->count() ) % m_tabEditors->count() );
}

void MainformImpl::copyFileName() {
	int item = m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_tabEditors->currentIndex();
	QString name = m_tabEditors->editor( item )->getTitle();
	qApp->clipboard()->setText( name );
}

void MainformImpl::copyPath() {
	int item = m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_tabEditors->currentIndex();
	AbstractEditor * ed = dynamic_cast<AbstractEditor*>( m_tabEditors->editor( item ) );
	if( ed ) {
		QString name = QDir::toNativeSeparators( ed->lastFileName() );
		qApp->clipboard()->setText( name );
	}
}

void MainformImpl::updateSpaceAndTab() {
	if( XINXConfig::self()->config().editor.showTabulationAndSpace != m_showSpaceAndTabAct->isChecked() ) {
		XINXConfig::self()->config().editor.showTabulationAndSpace = m_showSpaceAndTabAct->isChecked();
		XINXConfig::self()->save();
	}
}

void MainformImpl::createFindReplace() {
	m_findDialog       = new ReplaceDialogImpl( this );
	connect( m_findDialog, SIGNAL(find(QString, QString, AbstractEditor::SearchOptions)), this, SLOT(findFirst(QString, QString, AbstractEditor::SearchOptions)) );
	connect( m_findDialog, SIGNAL(findInFiles(QString, QString, QString, AbstractEditor::SearchOptions)), this, SLOT(findInFiles(QString, QString, QString, AbstractEditor::SearchOptions)) );

	m_replaceNextDlg   = new QMessageBox( QMessageBox::Question, tr("Replace text"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
	m_replaceNextDlg->setWindowModality( Qt::NonModal );
}

void MainformImpl::findFirst( const QString & chaine, const QString & dest, const AbstractEditor::SearchOptions & options ) {
	Q_ASSERT( m_tabEditors->currentEditor() );

	AbstractEditor * editor = m_tabEditors->currentEditor();

	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;
	m_nbFindedText		= 0;
	m_searchInverse		= false;

	editor->initSearch( m_findOptions );
	findNext();
}

void MainformImpl::findInFiles( const QString & directory, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options ) {
	m_searchAct->setEnabled( false );
	m_searchNextAct->setEnabled( false );
	m_searchPreviousAct->setEnabled( false );
	m_replaceAct->setEnabled( false );
	m_logDock->init();
	m_logDock->setVisible( true );

	SearchFileThread * threadSearch = new SearchFileThread();
	connect( threadSearch, SIGNAL(find(QString,QString,int)), m_logDock, SLOT(find(QString,QString,int)), Qt::BlockingQueuedConnection );
	connect( threadSearch, SIGNAL(end()), this, SLOT(findEnd()) );

	threadSearch->setPath( directory );
	threadSearch->setSearchString( from, to, options );
	threadSearch->search();
}

void MainformImpl::findEnd() {
	m_searchAct->setEnabled( true );
	m_searchNextAct->setEnabled( true );
	m_searchPreviousAct->setEnabled( true );
	m_replaceAct->setEnabled( true );

	m_logDock->end();

	QMessageBox::information( this, tr("Search End"), tr("All string are finded") );
}

void MainformImpl::findNext() {
	Q_ASSERT( m_tabEditors->currentEditor() );

	AbstractEditor * editor = m_tabEditors->currentEditor();

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
				QMessageBox::information( this,
				                          tr("Search/Replace"),
				                          tr("%1 occurences of '%2' replaced.").arg( m_nbFindedText ).arg( m_findExpression ),
				                          QMessageBox::Ok );
			} else {
				QMessageBox::StandardButton ret = QMessageBox::warning( this,
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

void MainformImpl::findPrevious() {
	m_searchInverse = true;
	findNext();
	m_searchInverse = false;
}

void MainformImpl::find() {
	if( qobject_cast<TextFileEditor*>( m_tabEditors->currentEditor() ) ) {
		XinxCodeEdit * textEdit = qobject_cast<TextFileEditor*>( m_tabEditors->currentEditor() )->textEdit();
		if( ! textEdit->textCursor().selectedText().isEmpty() ){
			m_findDialog->setText( textEdit->textCursor().selectedText() );
		}
	}
	m_findDialog->initialize( m_tabEditors->count() > 0 );
	m_findDialog->setReplace(false);
	m_findDialog->exec();
}

void MainformImpl::replace() {
	Q_ASSERT( m_tabEditors->currentEditor() );
	Q_ASSERT( qobject_cast<TextFileEditor*>( m_tabEditors->currentEditor() ) );

	XinxCodeEdit * textEdit = static_cast<TextFileEditor*>( m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(true);
	m_findDialog->exec();
}

bool MainformImpl::closeProject( bool session ) {
	if( ! XINXProjectManager::self()->project() ) return false;

	foreach( XinxPluginElement * e, XinxPluginsLoader::self()->plugins() ) {
		if( e->isActivated() && (! qobject_cast<IXinxPlugin*>( e->plugin() )->destroyProject( XINXProjectManager::self()->project() ) ))
			qWarning( qPrintable(tr("Can't stop a project for plugin \"%1\"").arg( qobject_cast<IXinxPlugin*>( e->plugin() )->getPluginAttribute( IXinxPlugin::PLG_NAME ).toString() )) );
	}

	saveProject( session );

	if( ! session ) {
		if( ! closeAllFile() ) {
			return false;
		}
	} else {
		m_tabEditors->setUpdatesEnabled( false );
		for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) {
			AbstractEditor * ed = m_tabEditors->editor( i );
			m_tabEditors->removeTab( i );
			delete ed;
		}
		m_tabEditors->setUpdatesEnabled( true );
	}
	m_projectDock->setProjectPath( NULL );
	m_contentDock->updateModel( NULL );

	XINXProjectManager::self()->deleteProject();

	return true;
}

void MainformImpl::updateTitle() {
	if( ! XINXProjectManager::self()->project() )
		setWindowTitle( tr("XINX") );
	else {
		setWindowTitle( tr("%1 - %2").arg( XINXProjectManager::self()->project()->projectName() ).arg( tr("XINX") ) );
	}
}

void MainformImpl::openProject() {
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open a project"), XINXConfig::self()->config().project.defaultPath, "Projet (*.prj)" );
	if( ! fileName.isEmpty() )
		openProject( fileName );
}

void MainformImpl::projectProperty() {
	Q_ASSERT( XINXProjectManager::self()->project() != NULL );

	ProjectPropertyImpl property ( this );
	property.loadFromProject( XINXProjectManager::self()->project() );

	if( property.exec() ) {
		property.saveToProject( XINXProjectManager::self()->project() );
		XINXProjectManager::self()->project()->saveToFile();
	}
}

void MainformImpl::closeProject() {
	closeProject( XINXConfig::self()->config().project.saveWithSessionByDefault );
}

AppSettings::struct_extentions MainformImpl::extentionOfFileName( const QString & name ) {
	AppSettings::struct_extentions result;
	int dotPosition = name.lastIndexOf( "." );
	QString suffix = name.toLower();
	if( dotPosition >= 0 )
		suffix = suffix.mid( dotPosition + 1 );
	if( XINXConfig::self()->config().files.count( suffix ) > 0 )
		result = XINXConfig::self()->config().files[ suffix ];
	return result;
}

void MainformImpl::setEditorPosition( int line, int column ) {
	m_editorPosition->setText( QString("   %1 x %2   ").arg( line, 3, 10, QLatin1Char('0') ).arg( column, 3, 10, QLatin1Char('0') ) );
}

void MainformImpl::closeEvent( QCloseEvent *event ) {
	emit aboutToClose();
	storeWindowSettings(); // Store before the project is closed

	if( XINXProjectManager::self()->project() ) {
		if( ! closeProject( XINXConfig::self()->config().project.saveWithSessionByDefault ) ) {
			event->ignore();
			return;
		}
	} else if( ! closeAllFile() ) {
		XINXConfig::self()->config().project.lastOpenedProject = QString(); // Pas de projet ouvert
		event->ignore();
		return;
	}
	event->accept();
}

void MainformImpl::timerEvent( QTimerEvent * ) {
	int threadCount      = XinxThreadManager::self()->getThreadCount(),
	    threadClassCount = XinxThreadManager::self()->getThreadClassCount();

	QString threadCountText = QString( "Xinx Thread : %1 (%2) / Other thread %3/%4" )
					.arg( threadCount, 3, 10, QLatin1Char('0') )
					.arg( threadClassCount, 3, 10, QLatin1Char('0') )
					.arg( QThreadPool::globalInstance()->activeThreadCount(), 3, 10, QLatin1Char('0') )
					.arg( QThreadPool::globalInstance()->maxThreadCount(), 3, 10, QLatin1Char('0') );

	if( ( threadCount + QThreadPool::globalInstance()->activeThreadCount() ) > QThread::idealThreadCount() )
		threadCountText = "<font color=\"red\">" + threadCountText + "</font>";

	m_threadCount->setText( threadCountText );
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

void MainformImpl::newTemplate( const QString &name, const QString &category, const QString &description, const QString &text, const QStringList &arguments, const QString &key, const QString &type ) {
	// On créé un template �  partir des informations indiqué.
	Snipet s;
	s.setName( name );
	s.setCategory( category );
	s.setDescription( description );
	s.setText( text );
	s.setType( type );
	s.setKey( key );
	s.params() = arguments;

	SnipetList list = SnipetListManager::self()->snipets();
	list.append( s );
	SnipetListManager::self()->setSnipets( list );

	SnipetListManager::self()->saveToSnipetFile();
}


void MainformImpl::openFile( const QString & filename ) {
	if( filename.isEmpty() ) return; // The file name must not to be empty

	// Add recent action
	if( XINXProjectManager::self()->project() ) {
		XINXProjectManager::self()->project()->session()->lastOpenedFile().removeAll( filename );
		XINXProjectManager::self()->project()->session()->lastOpenedFile().prepend( filename );

		while( XINXProjectManager::self()->project()->session()->lastOpenedFile().size() > MAXRECENTFILES )
			XINXProjectManager::self()->project()->session()->lastOpenedFile().removeLast();
	}

	// Load the file in the editor
	ScriptManager::self()->callScriptsBeforeLoad();
	m_tabEditors->createEditor( XinxPluginsLoader::self()->matchedFileType( filename ), QDir::fromNativeSeparators( filename ) );
	updateRecentFiles();
	updateActions();
	ScriptManager::self()->callScriptsAfterLoad();
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainformImpl::saveFileAs( const QString & name ) {
	if( ! m_tabEditors->currentEditor() ) return; // Can't save the editor if no editor to save.
	ScriptManager::self()->callScriptsBeforeSave();
	qobject_cast<TextFileEditor*>( m_tabEditors->currentEditor() )->saveToFile( name );
	ScriptManager::self()->callScriptsAfterSave();
}

void MainformImpl::saveAllFile() {
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		fileEditorSave( i );
	}
}

void MainformImpl::closeFile() {
	int item = m_tabEditors->getClickedTab();
	if( item == -1 ) item = m_tabEditors->currentIndex();
	if( item == -1 ) return; // Can't close editor if no editor to close
	fileEditorClose( item );
}

bool MainformImpl::closeAllFile() {
	m_tabEditors->setUpdatesEnabled( false );
	for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) {
		if ( fileEditorMayBeSave( i ) ) {
			m_contentDock->updateModel( NULL );
			AbstractEditor * editor = m_tabEditors->editor( i );
			m_tabEditors->removeTab( i );
			delete editor;
		} else {
			m_tabEditors->setUpdatesEnabled( true );
			return false;
		}
	}
	m_tabEditors->setUpdatesEnabled( true );

	updateActions();
	return true;
}

void MainformImpl::newProject() {
	NewProjectWizard wizard;
	if( wizard.exec() ) {
		XinxProject * project = wizard.createProject();

		QString filename = QFileDialog::getSaveFileName( this, tr("Save a project"), project->projectPath(), "Projet (*.prj)" );
		if( filename.isEmpty() ) {
			delete project;
			return;
		}
		m_lastProjectOpenedPlace = project->projectPath();

		project->saveToFile( filename );
		delete project;

		openProject( filename );
	}
}

void MainformImpl::newProject( const QString &name, const QString &path, bool isDerivated, const QString &prefix, const QString &filename ) {
	try {
		XinxProject * project = new XinxProject();
		project->setProjectName( name );
		project->setProjectPath( path );
		if( isDerivated ) {
			project->setSpecifiquePrefix( prefix );
			project->setOptions( XinxProject::hasSpecifique );
		}
		project->saveToFile( filename );

		delete project;

		openProject( filename );
	} catch (XinxProjectException e) {
	}
}

void MainformImpl::openProject( const QString & filename ) {
	if( filename.isEmpty() ) return; // The project filename is not empty

	if( XINXProjectManager::self()->project() )
		closeProject();
	else
		closeAllFile();

	XINXConfig::self()->config().project.recentProjectFiles.removeAll( filename );

	XinxProject * project = NULL;
	try {
		project = new XinxProject( filename );

		m_lastProjectOpenedPlace = QFileInfo( filename ).absolutePath();
		SpecifiqueDialogImpl::setLastPlace( project->projectPath() );

		XINXConfig::self()->config().project.recentProjectFiles.prepend( filename );
		while( XINXConfig::self()->config().project.recentProjectFiles.size() > MAXRECENTFILES )
			XINXConfig::self()->config().project.recentProjectFiles.removeLast();

		XINXProjectManager::self()->deleteProject();
		XINXProjectManager::self()->setCurrentProject( project );
		m_projectDock->setProjectPath( XINXProjectManager::self()->project() );

		foreach( XinxPluginElement * e, XinxPluginsLoader::self()->plugins() ) {
			if( e->isActivated() && (! qobject_cast<IXinxPlugin*>( e->plugin() )->initializeProject( XINXProjectManager::self()->project() ) ))
				qWarning( qPrintable(tr("Can't start a project for plugin \"%1\"").arg( qobject_cast<IXinxPlugin*>( e->plugin() )->getPluginAttribute( IXinxPlugin::PLG_NAME ).toString() )) );
		}

		m_tabEditors->setUpdatesEnabled( false );
		foreach( XinxProjectSessionEditor * data, project->session()->serializedEditors() ) {
			AbstractEditor * editor = AbstractEditor::deserializeEditor( data );
			if( editor )
				m_tabEditors->newTextFileEditor( editor );
			else
				qWarning( qPrintable( tr("Error when restoring editor for %1").arg( data->readProperty( "FileName" ).toString() ) ) );
		}
		if( m_tabEditors->count() > 0 ) m_tabEditors->setCurrentIndex( 0 );
		m_tabEditors->setUpdatesEnabled( true );
	} catch( XinxProjectException e ) {
		delete project;
		if( ( ! e.startWizard() ) || (! QProcess::startDetached( QDir( QApplication::applicationDirPath() ).absoluteFilePath( "xinxprojectwizard" ), QStringList() << "-lang" << XINXConfig::self()->config().language << filename ) ) )
			QMessageBox::warning( this, tr("Can't open project"), e.getMessage() );
	}
}

void MainformImpl::closeProjectNoSessionData() {
	closeProject( false );
}

void MainformImpl::closeProjectWithSessionData() {
	closeProject( true );
}

void MainformImpl::saveProject( bool withSessionData ) {
	Q_ASSERT( XINXProjectManager::self()->project() );

	qDeleteAll( XINXProjectManager::self()->project()->session()->serializedEditors() );
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		m_tabEditors->editor( i )->serialize( new XinxProjectSessionEditor( XINXProjectManager::self()->project()->session() ), withSessionData );
	}
	XINXProjectManager::self()->project()->saveOnlySession();
}

void MainformImpl::updateFromVersionManager( const QStringList & list ) {
	RCS * rcs = m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		m_logDock->init();
		m_rcsExecute = true;
		if( list.count() == 0 )
			rcs->update( QStringList() << XINXProjectManager::self()->project()->projectPath() );
		else
			rcs->update( list );
		updateActions();
		m_rcsVisible = m_logDock->isVisible();
		m_logDock->show();
	}
}

void MainformImpl::commitToVersionManager( const QStringList & list ) {
	RCS * rcs = m_projectDock->rcs();
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

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		m_logDock->init();
		m_rcsExecute = true;

		rcs->commit( dlg.filesOperation(), message );
		updateActions();
		m_rcsVisible = m_logDock->isVisible();
		m_logDock->show();
	}
}

void MainformImpl::addFilesToVersionManager( const QStringList & list ) {
	RCS * rcs = m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		m_logDock->init();
		m_rcsExecute = true;
		rcs->add( list );
		updateActions();
		m_rcsVisible = m_logDock->isVisible();
		m_logDock->show();
	}
}

void MainformImpl::removeFilesFromVersionManager( const QStringList & list ) {
	RCS * rcs = m_projectDock->rcs();
	if( rcs ) {
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_logDock, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_cancelRCSOperationAct, SIGNAL(triggered()), rcs, SLOT(abort()) );
		m_logDock->init();
		m_rcsExecute = true;
		foreach( const QString & file, list ) {
			m_projectDock->removeFile( file );
		}
		rcs->remove( list );
		updateActions();
		m_rcsVisible = m_logDock->isVisible();
		m_logDock->show();
	}
}
