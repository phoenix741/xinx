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
#include "replacedialogimpl.h"
#include "logdialogimpl.h"
#include "snipetdockwidget.h"
#include "aboutdialogimpl.h"
#include "customdialog/customdialogimpl.h"
#include "projectpropertyimpl.h"
#include "uniqueapplication.h"
#include "newprojectwizard.h"
#include "searchfilethread.h"
#include "xinxprojectwizard/projectwizard.h"
#include <project/xinxproject.h>
#include <core/exceptions.h>
#include <core/xinxthread.h>
#include <plugins/xinxpluginsloader.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <editors/editorfactory.h>
#include <rcs/rcsmanager.h>
#include <scripts/scriptmanager.h>
#include <snipets/snipetmanager.h>
#include <snipets/snipetmenu.h>
#include "welcomdlgimpl.h"
#include <project/externalfileresolver.h>
#include <contentview2/contentview2cache.h>
#include <contentview2/contentview2manager.h>
#include <dtoolview.h>
#include <dviewbutton.h>

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
#include <QIcon>
#include <QThreadPool>
#include <QDebug>

/* MainformImpl */

MainformImpl::MainformImpl(QWidget * parent) : DMainWindow(parent),  m_lastFileName(QDir::currentPath()), m_lastProjectOpenedPlace(QDir::currentPath()), m_closeTabBtn(0)
{
	createMainForm();
	createMenus();

	registerTypes();
	createTabEditorButton();
	createStatusBar();
	createDockWidget();
	createFindReplace();
	createTools();
	createRCS();
	updateActions();
	updateRecentFiles();
	updateRecentProjects();

	// Update the status bar position
	setEditorPosition(1, 1);

	// Restore windows property
	readWindowSettings();

	startTimer(1000);
}

MainformImpl::~MainformImpl()
{

}

void MainformImpl::createMainForm()
{
	setObjectName("MainForm");
	setWindowIcon(QIcon(":/images/splash.png"));
	setWindowTitle("XINX");

	setCorner(Qt::TopLeftCorner,     Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner,  Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner,    Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	setDockOptions(AnimatedDocks | AllowTabbedDocks | VerticalTabs);

	m_tabEditors = new TabEditor(this);
	setCentralWidget(m_tabEditors);

	QMetaObject::connectSlotsByName(this);
}

void MainformImpl::createMenus()
{
	QMenu * fileMenu, * editMenu, *searchMenu, *bookmarkMenu, *projectMenu, *windowsMenu, *toolsMenu, *helpMenu;
	QToolBar *projectToolBar, * fileToolBar, * editToolBar, * searchToolBar;
	m_menuBar = new QMenuBar(this);
	setMenuBar(m_menuBar);
	m_menus.insert("file", fileMenu = new QMenu(tr("&File"), m_menuBar));
	m_menus.insert("edit", editMenu = new QMenu(tr("&Edit"), m_menuBar));
	m_menus.insert("search", searchMenu = new QMenu(tr("&Search"), m_menuBar));
	m_menus.insert("bookmark", bookmarkMenu = new QMenu(tr("&Bookmark"), m_menuBar));
	m_menus.insert("project", projectMenu = new QMenu(tr("&Project"), m_menuBar));
	m_menus.insert("windows", windowsMenu = new QMenu(tr("&Windows"), m_menuBar));
	m_menus.insert("tools", toolsMenu = new QMenu(tr("&Tools"), m_menuBar));
	m_menus.insert("help", helpMenu = new QMenu(tr("&Help"), m_menuBar));
	m_scriptMenu = new QMenu(tr("&Script"), m_menuBar);
	m_snipetMenu = SnipetManager::self()->createSnipetMenu(tr("&Snipet"), m_menuBar);
	connect(m_snipetMenu, SIGNAL(snipetTriggered(int)), this, SLOT(callSnipetAction(int)));

	m_toolBars.insert("project", projectToolBar = new QToolBar(this));
	m_toolBars.insert("file", fileToolBar = new QToolBar(this));
	m_toolBars.insert("edit", editToolBar = new QToolBar(this));
	m_toolBars.insert("search", searchToolBar = new QToolBar(this));

	projectToolBar->setWindowTitle(tr("&Project"));
	fileToolBar->setWindowTitle(tr("&File"));
	editToolBar->setWindowTitle(tr("&Edit"));
	searchToolBar->setWindowTitle(tr("&Search"));

	createActions();

	m_menuBar->addAction(fileMenu->menuAction());
	m_menuBar->addAction(editMenu->menuAction());
	m_menuBar->addAction(searchMenu->menuAction());
	m_menuBar->addAction(bookmarkMenu->menuAction());
	m_menuBar->addAction(projectMenu->menuAction());
	m_menuBar->addAction(toolsMenu->menuAction());
	m_menuBar->addAction(windowsMenu->menuAction());
	m_menuBar->addAction(helpMenu->menuAction());

	fileMenu->addAction(m_newAct);
	fileMenu->addAction(m_recentFileAct);
	fileMenu->addAction(m_saveAct);
	fileMenu->addAction(m_saveAsAct);
	fileMenu->addAction(m_saveAllAct);
	fileMenu->addSeparator();
	fileMenu->addAction(m_closeAct);
	fileMenu->addAction(m_closeAllAct);
	fileMenu->addSeparator();
	fileMenu->addAction(m_printAct);
	fileMenu->addSeparator();
	fileMenu->addAction(m_exitAct);

	projectMenu->addAction(m_newProjectAct);
	projectMenu->addAction(m_recentProjectAct);
	projectMenu->addAction(m_closeProjectAct);
	projectMenu->addAction(m_projectPropertyAct);
	projectMenu->addSeparator();
	projectMenu->addAction(RCSManager::self()->updateAllAction());
	projectMenu->addAction(RCSManager::self()->commitAllAction());
	projectMenu->addAction(RCSManager::self()->abortAction());

	windowsMenu->addAction(m_nextTabAct);
	windowsMenu->addAction(m_previousTabAct);
	windowsMenu->addSeparator();
	windowsMenu->addAction(m_toggledFlatView);
	windowsMenu->addAction(m_alwaysShowRunDialog);
	windowsMenu->addSeparator();

	toolsMenu->addAction(m_snipetMenu->menuAction());
	toolsMenu->addAction(m_scriptMenu->menuAction());
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_customApplicationAct);

	m_scriptMenu->addAction(m_refreshScripts);
	m_scriptMenu->addSeparator();

	helpMenu->addAction(m_aboutAct);
	helpMenu->addAction(m_aboutQtAct);
	helpMenu->addAction(QWhatsThis::createAction());

	searchMenu->addAction(m_searchAct);
	searchMenu->addAction(m_searchNextAct);
	searchMenu->addAction(m_searchPreviousAct);
	searchMenu->addSeparator();
	searchMenu->addAction(m_replaceAct);

	editMenu->addAction(m_undoAct);
	editMenu->addAction(m_redoAct);
	editMenu->addSeparator();
	editMenu->addAction(m_cutAct);
	editMenu->addAction(m_copyAct);
	editMenu->addAction(m_pasteAct);
	editMenu->addSeparator();
	editMenu->addAction(m_selectAllAct);
	editMenu->addAction(m_duplicateLineAct);
	editMenu->addAction(m_moveUpLineAct);
	editMenu->addAction(m_moveDownLineAct);
	editMenu->addSeparator();
	editMenu->addAction(m_upperTextAct);
	editMenu->addAction(m_lowerTextAct);
	editMenu->addAction(m_showSpaceAndTabAct);
	editMenu->addSeparator();
	editMenu->addAction(m_indentAct);
	editMenu->addAction(m_unindentAct);
	editMenu->addSeparator();
	editMenu->addAction(m_prettyPrintAct);
	editMenu->addAction(m_highlightWord);
	editMenu->addAction(m_completeAct);

	bookmarkMenu->addAction(m_bookmarkAct);
	bookmarkMenu->addAction(m_previousBookmarkAct);
	bookmarkMenu->addAction(m_nextBookmarkAct);
	bookmarkMenu->addSeparator();
	bookmarkMenu->addAction(m_clearAllBookmarkAct);


	projectToolBar->addAction(m_recentProjectAct);
	projectToolBar->addAction(m_closeProjectAct);
	projectToolBar->setOrientation(Qt::Horizontal);
	projectToolBar->setObjectName("projectToolBar");
	addToolBar(Qt::TopToolBarArea, projectToolBar);
	QToolButton * btn = qobject_cast<QToolButton*>(projectToolBar->widgetForAction(m_recentProjectAct));
	if (btn) btn->setPopupMode(QToolButton::MenuButtonPopup);
	btn = qobject_cast<QToolButton*>(projectToolBar->widgetForAction(m_closeProjectAct));
	if (btn) btn->setPopupMode(QToolButton::MenuButtonPopup);

	fileToolBar->addAction(m_newAct);
	fileToolBar->addAction(m_recentFileAct);
	fileToolBar->addAction(m_saveAct);
	fileToolBar->addAction(m_saveAllAct);
	fileToolBar->addSeparator();
	fileToolBar->addAction(m_printAct);
	fileToolBar->setOrientation(Qt::Horizontal);
	fileToolBar->setObjectName("fileToolBar");
	addToolBar(Qt::TopToolBarArea, fileToolBar);
	btn = qobject_cast<QToolButton*>(fileToolBar->widgetForAction(m_newAct));
	if (btn) btn->setPopupMode(QToolButton::MenuButtonPopup);
	btn = qobject_cast<QToolButton*>(fileToolBar->widgetForAction(m_recentFileAct));
	if (btn) btn->setPopupMode(QToolButton::MenuButtonPopup);

	editToolBar->addAction(m_undoAct);
	editToolBar->addSeparator();
	editToolBar->addAction(m_cutAct);
	editToolBar->addAction(m_copyAct);
	editToolBar->addAction(m_pasteAct);
	editToolBar->addSeparator();
	editToolBar->addAction(m_moveUpLineAct);
	editToolBar->addAction(m_moveDownLineAct);
	editToolBar->addSeparator();
	editToolBar->addAction(m_lowerTextAct);
	editToolBar->addAction(m_upperTextAct);
	editToolBar->addAction(m_showSpaceAndTabAct);
	editToolBar->setOrientation(Qt::Horizontal);
	editToolBar->setObjectName("editToolBar");
	addToolBar(Qt::TopToolBarArea, editToolBar);

	searchToolBar->addAction(m_searchAct);
	searchToolBar->setOrientation(Qt::Horizontal);
	searchToolBar->setObjectName("searchToolBar");
	addToolBar(Qt::TopToolBarArea, searchToolBar);

	createPluginsActions();
}

void MainformImpl::createActions()
{
	/* FILE */

	// New
	m_newMenu = new QMenu(this);
	createNewSubMenu();

	m_newAct = new QAction(QIcon(":/images/filenew.png"), tr("&New"), this);
	m_newAct->setToolTip(tr("New document"));
	m_newAct->setStatusTip(tr("Create a new document"));
	m_newAct->setShortcut(QKeySequence::New);
	m_newAct->setData(QVariant::fromValue<IFileTypePlugin*>(XinxPluginsLoader::self()->fileTypes()[0]));
	m_newAct->setMenu(m_newMenu);
	connect(m_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

	// Open
	m_openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open ..."), this);
	m_openAct->setToolTip(tr("Open a file on the disk"));
	m_openAct->setStatusTip(tr("Open an existing file on the disk"));
	connect(m_openAct, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

	// Recent File
	QMenu * recentFileMenu = new QMenu(this);
	recentFileMenu->addAction(m_openAct);

	m_recentFileAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Recent file"), this);
	m_recentFileAct->setShortcut(QKeySequence::Open);
	m_recentFileAct->setMenu(recentFileMenu);
	setupRecentMenu(recentFileMenu, m_recentFileSeparator, m_recentFileActs);

	connect(m_recentFileAct, SIGNAL(triggered()), this, SLOT(openFile()));
	for (int i = 0; i < MAXRECENTFILES; i++)
	{
		connect(m_recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}

	// Refresh file
	m_refreshFileAct = new QAction(QIcon(":/images/reload.png"), tr("&Refresh"), this);
	m_tabEditors->setRefreshAction(m_refreshFileAct);
	connect(m_refreshFileAct, SIGNAL(triggered()), this, SLOT(refreshFile()));

	// Copy Filename
	m_copyFileNameAct = new QAction(tr("&Copy filename to Clipboard"), this);
	connect(m_copyFileNameAct, SIGNAL(triggered()), this, SLOT(copyFileName()));
	m_tabEditors->setCopyFileNameAction(m_copyFileNameAct);

	// Copy Path
	m_copyPathAct = new QAction(tr("C&opy path to clipboard"), this);
	m_tabEditors->setCopyPathAction(m_copyPathAct);
	connect(m_copyPathAct, SIGNAL(triggered()), this, SLOT(copyPath()));

	// Save
	m_saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save"), this);
	m_saveAct->setToolTip(tr("Save the document"));
	m_saveAct->setStatusTip(tr("Save the document to disk"));
	m_saveAct->setShortcut(QKeySequence::Save);
	m_tabEditors->setSaveAction(m_saveAct);
	connect(m_saveAct, SIGNAL(triggered()), this, SLOT(saveFile()));

	// Save As
	m_saveAsAct = new QAction(QIcon(":/images/filesaveas.png"), tr("Save &As..."), this);
	m_saveAsAct->setToolTip(tr("Save the document As"));
	m_saveAsAct->setStatusTip(tr("Save the document with a new name to the disk"));
	m_tabEditors->setSaveAsAction(m_saveAsAct);
	connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAsFile()));

	// Save All
	m_saveAllAct = new QAction(QIcon(":/images/filesaveall.png"), tr("Save A&ll..."), this);
	m_saveAllAct->setToolTip(tr("Save all documents"));
	m_saveAllAct->setStatusTip(tr("Save all documents to disk"));
	connect(m_saveAllAct, SIGNAL(triggered()), this, SLOT(saveAllFile()));

	// Print
	m_printAct = new QAction(QIcon(":/images/fileprint.png"), tr("&Print ..."), this);
	m_printAct->setToolTip(tr("Print the document"));
	m_printAct->setStatusTip(tr("Print the current document"));
	m_printAct->setShortcut(QKeySequence::Print);
	connect(m_printAct, SIGNAL(triggered()), this, SLOT(printFile()));

	// Close
	m_closeAct = new QAction(QIcon(":/images/fileclose.png"), tr("&Close"), this);
	m_closeAct->setToolTip(tr("Close document"));
	m_closeAct->setStatusTip(tr("Close the current document"));
	m_closeAct->setShortcut(QKeySequence::Close);
	m_tabEditors->setCloseAction(m_closeAct);
	connect(m_closeAct, SIGNAL(triggered()), this, SLOT(closeFile()));

	// Close all
	m_closeAllAct = new QAction(tr("&Close All"), this);
	m_closeAllAct->setToolTip(tr("Close All document"));
	m_closeAllAct->setStatusTip(tr("Close All document"));
	connect(m_closeAllAct, SIGNAL(triggered()), this, SLOT(closeAllFile()));

	// Exit
	m_exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
	m_exitAct->setStatusTip(tr("Quit XINX"));
	m_exitAct->setShortcut(QKeySequence::mnemonic(m_exitAct->text()));
	connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

	/* EDIT */

	// Undo
	m_undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
	m_undoAct->setStatusTip(tr("Undo the last action"));
	m_undoAct->setShortcut(QKeySequence::Undo);
	m_undoAct->setEnabled(false);
	connect(m_undoAct, SIGNAL(triggered()), m_tabEditors, SLOT(undo()));
	connect(m_tabEditors, SIGNAL(undoAvailable(bool)), m_undoAct, SLOT(setEnabled(bool)));

	// Redo
	m_redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
	m_redoAct->setStatusTip(tr("Redo the next action"));
	m_redoAct->setShortcut(QKeySequence::Redo);
	m_redoAct->setEnabled(false);
	connect(m_redoAct, SIGNAL(triggered()), m_tabEditors, SLOT(redo()));
	connect(m_tabEditors, SIGNAL(redoAvailable(bool)), m_redoAct, SLOT(setEnabled(bool)));

	// Cut
	m_cutAct = new QAction(QIcon(":/images/editcut.png"), tr("Cu&t"), this);
	m_cutAct->setStatusTip(tr("Cut the selection to the clipboard"));
	m_cutAct->setShortcut(QKeySequence::Cut);
	m_cutAct->setEnabled(false);
	connect(m_cutAct, SIGNAL(triggered()), m_tabEditors, SLOT(cut()));
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_cutAct, SLOT(setEnabled(bool)));

	// Copy
	m_copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
	m_copyAct->setStatusTip(tr("Copy the selection to the clipboard"));
	m_copyAct->setShortcut(QKeySequence::Copy);
	m_copyAct->setEnabled(false);
	connect(m_copyAct, SIGNAL(triggered()), m_tabEditors, SLOT(copy()));
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_copyAct, SLOT(setEnabled(bool)));

	// Paste
	m_pasteAct = new QAction(QIcon(":/images/editpaste.png"), tr("&Paste"), this);
	m_pasteAct->setStatusTip(tr("Paste the clipboard's contents"));
	m_pasteAct->setShortcut(QKeySequence::Paste);
	m_pasteAct->setEnabled(false);
	connect(m_pasteAct, SIGNAL(triggered()), m_tabEditors, SLOT(paste()));
	connect(m_tabEditors, SIGNAL(pasteAvailable(bool)), m_pasteAct, SLOT(setEnabled(bool)));

	// Select All
	m_selectAllAct = new QAction(QIcon(":/images/edit-select-all.png"), tr("&Select All"), this);
	m_selectAllAct->setStatusTip(tr("Select all the text of the current editor"));
	m_selectAllAct->setShortcut(QKeySequence::SelectAll);
	m_selectAllAct->setEnabled(false);
	connect(m_selectAllAct, SIGNAL(triggered()), m_tabEditors, SLOT(selectAll()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_selectAllAct, SLOT(setEnabled(bool)));

	// Duplicate Line
	m_duplicateLineAct = new QAction(tr("&Duplicate current line"), this);
	m_duplicateLineAct->setStatusTip(tr("Duplicate the current line"));
	m_duplicateLineAct->setShortcut(QKeySequence("Ctrl+D"));
	m_duplicateLineAct->setEnabled(false);
	connect(m_duplicateLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(duplicateCurrentLine()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_duplicateLineAct, SLOT(setEnabled(bool)));

	// Move Line Up
	m_moveUpLineAct = new QAction(QIcon(":/images/1uparrow.png"), tr("&Move line up"), this);
	m_moveUpLineAct->setStatusTip(tr("Move the current line up"));
	m_moveUpLineAct->setShortcut(QKeySequence("Ctrl+Shift+Up"));
	m_moveUpLineAct->setEnabled(false);
	connect(m_moveUpLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineUp()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_moveUpLineAct, SLOT(setEnabled(bool)));

	// Move Line Down
	m_moveDownLineAct = new QAction(QIcon(":/images/1downarrow.png"), tr("&Move line down"), this);
	m_moveDownLineAct->setStatusTip(tr("Move the current line down"));
	m_moveDownLineAct->setShortcut(QKeySequence("Ctrl+Shift+Down"));
	m_moveDownLineAct->setEnabled(false);
	connect(m_moveDownLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineDown()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_moveDownLineAct, SLOT(setEnabled(bool)));

	// Upper
	m_upperTextAct = new QAction(QIcon(":/images/fontsizeup.png"), tr("&Upper Case"), this);
	m_upperTextAct->setStatusTip(tr("Make the selected character upper case"));
	m_upperTextAct->setShortcut(QKeySequence::mnemonic(m_upperTextAct->text()));
	m_upperTextAct->setEnabled(false);
	connect(m_upperTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(upperSelectedText()));
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_upperTextAct, SLOT(setEnabled(bool)));

	// Lower
	m_lowerTextAct = new QAction(QIcon(":/images/fontsizedown.png"), tr("&Lower Case"), this);
	m_lowerTextAct->setStatusTip(tr("Make the selected character lower case"));
	m_lowerTextAct->setShortcut(QKeySequence::mnemonic(m_lowerTextAct->text()));
	m_lowerTextAct->setEnabled(false);
	connect(m_lowerTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(lowerSelectedText()));
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_lowerTextAct, SLOT(setEnabled(bool)));

	// Indent
	m_indentAct = new QAction(QIcon(":/images/format-indent-more.png"), tr("Indent Code"), this);
	m_indentAct->setShortcut(QKeySequence("Tab"));
	m_indentAct->setEnabled(false);
	connect(m_indentAct, SIGNAL(triggered()), m_tabEditors, SLOT(indent()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_indentAct, SLOT(setEnabled(bool)));

	// Unindent
	m_unindentAct = new QAction(QIcon(":/images/format-indent-less.png"), tr("Unindent code"), this);
	m_unindentAct->setShortcut(QKeySequence("Shift+Tab"));
	m_unindentAct->setEnabled(false);
	connect(m_unindentAct, SIGNAL(triggered()), m_tabEditors, SLOT(unindent()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_unindentAct, SLOT(setEnabled(bool)));

	// Complete
	m_completeAct = new QAction(tr("Completer"), this);
	m_completeAct->setShortcut(QKeySequence("Ctrl+E"));
	m_completeAct->setEnabled(false);
	connect(m_completeAct, SIGNAL(triggered()), m_tabEditors, SLOT(complete()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_completeAct, SLOT(setEnabled(bool)));

	// Pretty Print
	m_prettyPrintAct = new QAction(tr("Pretty print"), this);
	m_prettyPrintAct->setShortcut(QKeySequence("Ctrl+Shift+F"));
	m_prettyPrintAct->setEnabled(false);
	connect(m_prettyPrintAct, SIGNAL(triggered()), m_tabEditors, SLOT(autoindent()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_prettyPrintAct, SLOT(setEnabled(bool)));

	// Highlight word
	m_highlightWord = new QAction(tr("Highlight same word"), this);
	m_highlightWord->setShortcut(QKeySequence("Ctrl+U"));
	connect(m_highlightWord, SIGNAL(triggered()), m_tabEditors, SLOT(highlightWord()));

	// Show space and tab
	m_showSpaceAndTabAct = new QAction(QIcon(":/images/draw-text.png"), tr("Show space and &Tabulation"), this);
	m_showSpaceAndTabAct->setCheckable(true);
	m_showSpaceAndTabAct->setChecked(XINXConfig::self()->config().editor.showTabulationAndSpace);
	connect(m_showSpaceAndTabAct, SIGNAL(triggered()), this, SLOT(updateSpaceAndTab()));

	/* SEARCH */

	// Search
	m_searchAct = new QAction(QIcon(":/images/find.png"), tr("&Search ..."), this);
	m_searchAct->setStatusTip(tr("Search a text in the current editor"));
	m_searchAct->setShortcut(QKeySequence::Find);
	connect(m_searchAct, SIGNAL(triggered()), this, SLOT(find()));

	// Search Previous
	m_searchPreviousAct = new QAction(tr("Search previous"), this);
	m_searchPreviousAct->setIconText(tr("Search previous"));
	m_searchPreviousAct->setToolTip(tr("Search previous"));
	m_searchPreviousAct->setShortcut(QKeySequence::FindPrevious);
	m_searchPreviousAct->setEnabled(false);
	connect(m_searchPreviousAct, SIGNAL(triggered()), this, SLOT(findPrevious()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_searchPreviousAct, SLOT(setEnabled(bool)));

	// Search Next
	m_searchNextAct = new QAction(tr("Search next"), this);
	m_searchNextAct->setIconText(tr("Search next"));
	m_searchNextAct->setToolTip(tr("Search next"));
	m_searchNextAct->setStatusTip(tr("Search the next element in the current editor"));
	m_searchNextAct->setShortcut(QKeySequence::FindNext);
	m_searchNextAct->setEnabled(false);
	connect(m_searchNextAct, SIGNAL(triggered()), this, SLOT(findNext()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_searchNextAct, SLOT(setEnabled(bool)));

	// Replace
	m_replaceAct = new QAction(tr("&Replace ..."), this);
	m_replaceAct->setStatusTip(tr("Replace a text in the current editor"));
	m_replaceAct->setShortcut(QKeySequence::Replace);
	m_replaceAct->setEnabled(false);
	connect(m_replaceAct, SIGNAL(triggered()), this, SLOT(replace()));
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_replaceAct, SLOT(setEnabled(bool)));

	/* PROJECT */

	connect(XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateActions()));
	connect(XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateTitle()));
	connect(XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateRecentProjects()));
	connect(XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateRecentFiles()));
	connect(XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(updateRCS()));

	// New Project
	m_newProjectAct = new QAction(QIcon(":/images/window_new.png"), tr("&New project ..."), this);
	m_newProjectAct->setStatusTip(tr("Create a new project"));
	connect(m_newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()));

	// Open Project
	m_openProjectAct = new QAction(QIcon(":/images/project_open.png"), tr("&Choose file ..."), this);
	m_openProjectAct->setToolTip(tr("Choose a project file"));
	m_openProjectAct->setStatusTip(tr("Choose a project file on the disk"));
	connect(m_openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

	// Recent Project
	QMenu * recentProjectMenu = new QMenu(this);
	recentProjectMenu->addAction(m_openProjectAct);
	m_recentProjectAct = new QAction(QIcon(":/images/project_open.png"), tr("&Recent Project"), this);
	m_recentProjectAct->setMenu(recentProjectMenu);
	setupRecentMenu(recentProjectMenu, m_recentSeparator, m_recentProjectActs);

	connect(m_recentProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));
	for (int i = 0; i < MAXRECENTFILES; i++)
	{
		connect(m_recentProjectActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()));
	}

	// Save Project
	m_saveProjectAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save project"), this);
	m_saveProjectAct->setToolTip(tr("Save the project"));

	// Close Project
	QMenu * closeProjectMenu = new QMenu(this);

	m_closeProjectAct = new QAction(QIcon(":/images/project_close.png"), tr("Close project"), this);
	connect(m_closeProjectAct, SIGNAL(triggered()), this, SLOT(closeProject()));
	m_closeProjectAct->setMenu(closeProjectMenu);

	m_closeProjectNoSessionAct = new QAction(tr("&Close Project (no session)"), this);
	closeProjectMenu->addAction(m_closeProjectNoSessionAct);

	connect(m_closeProjectNoSessionAct, SIGNAL(triggered()), this, SLOT(closeProjectNoSessionData()));

	m_closeProjectWithSessionAct = new QAction(tr("Close Project (session)"), this);
	m_closeProjectWithSessionAct->setWhatsThis(tr("Save and Close project with session information (the content of file editor is saved too)."));
	closeProjectMenu->addAction(m_closeProjectWithSessionAct);

	connect(m_closeProjectWithSessionAct, SIGNAL(triggered()), this, SLOT(closeProjectWithSessionData()));

	// Property
	m_projectPropertyAct = new QAction(tr("Project &property"), this);
	m_projectPropertyAct->setStatusTip(tr("Show/Modify the property of the project"));
	connect(m_projectPropertyAct, SIGNAL(triggered()), this, SLOT(projectProperty()));

	/* RCS */

	// toggled View
	m_toggledFlatView = new QAction(QIcon(":/images/flatlist.png"), tr("Toggled Flat View"), this);
	m_toggledFlatView->setWhatsThis(tr("If checked the list is showed as flat instead of tree. Each list of file is preceded of a directory header."));
	m_toggledFlatView->setCheckable(true);

	/* WINDOWS */

	// Next Tab
	m_nextTabAct = new QAction(tr("Next tab"), this);
	m_nextTabAct->setToolTip(tr("Next tab"));
	m_nextTabAct->setShortcut(QKeySequence::NextChild);
	connect(m_nextTabAct, SIGNAL(triggered()), this, SLOT(nextTab()));

	// Previous Tab
	m_previousTabAct = new QAction(tr("Previous Tab"), this);
	m_previousTabAct->setShortcut(QKeySequence::PreviousChild);
	connect(m_previousTabAct, SIGNAL(triggered()), this, SLOT(previousTab()));

	/* TOOLS */

	// Custom
	m_customApplicationAct = new QAction(QIcon(":/images/configure.png"), tr("Customize ..."), this);
	connect(m_customApplicationAct, SIGNAL(triggered()), this, SLOT(customize()));

	// Refresh scripts
	m_refreshScripts = new QAction(tr("&Refresh Scripts"), this);
	connect(m_refreshScripts, SIGNAL(triggered()), ScriptManager::self(), SLOT(loadScripts()));

	/* BOOKMARK */

	// Next Bookmark
	m_nextBookmarkAct = new QAction(QIcon(":/images/bookmarknext.png"), tr("&Next bookmark"), this);
	m_nextBookmarkAct->setShortcut(QKeySequence("F2"));
	connect(m_nextBookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(nextBookmark()));

	// Set Bookmark
	m_bookmarkAct = new QAction(QIcon(":/images/bookmark.png"), tr("&Bookmark"), this);
	m_bookmarkAct->setShortcut(QKeySequence("Ctrl+B"));
	connect(m_bookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(bookmark()));

	// Previous Bookmark
	m_previousBookmarkAct = new QAction(QIcon(":/images/bookmarkprevious.png"), tr("&Previous bookmark"), this);
	m_previousBookmarkAct->setShortcut(QKeySequence("Shift+F2"));
	connect(m_previousBookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(previousBookmark()));

	// Clear All Bookmark
	m_clearAllBookmarkAct = new QAction(QIcon(":/images/clearallbookmarks.png"), tr("&Clear all bookmark"), this);
	m_clearAllBookmarkAct->setShortcut(QKeySequence("Ctrl+F2"));
	connect(m_clearAllBookmarkAct, SIGNAL(triggered()), m_tabEditors, SLOT(clearAllBookmark()));

	/* ABOUT */

	// About Qt
	m_aboutQtAct = new QAction(QIcon(":/images/help-hint.png"), tr("About &Qt"), this);
	m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// About
	m_aboutAct = new QAction(QIcon(":/images/help-about.png"), tr("&About"), this);
	m_aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	// Tab Editor
	connect(m_tabEditors, SIGNAL(setEditorPosition(int,int)), this, SLOT(setEditorPosition(int,int)));
	connect(m_tabEditors, SIGNAL(contentChanged()), this, SLOT(updateEditorInformations()));

	// Drag & Drop
	connect(m_tabEditors, SIGNAL(fileOpened(QString)), this, SLOT(updateActions()));

	// Timer LOG
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(logTimeout()));

	/* WINDOW */
	m_alwaysShowRunDialog = new QAction(tr("Always show run snipet dialog"), this);
	m_alwaysShowRunDialog->setCheckable(true);
	m_alwaysShowRunDialog->setChecked(XINXConfig::self()->config().snipets.alwaysShowDialog);
	connect(m_alwaysShowRunDialog, SIGNAL(triggered()), this, SLOT(changeShowSnipetDialogAction()));
}

void MainformImpl::createPluginsActions()
{
	// Clear all action for better creation
	m_pluginsAction.clear();

	// Create actions

	// Pour chaque plugins
	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{

		// Si le plugin est activé
		if (e->isActivated() && qobject_cast<IXinxPlugin*>(e->plugin()))
		{
			XinxAction::MenuList menuList = qobject_cast<IXinxPlugin*>(e->plugin())->actions();

			// Pour chaque menu
			foreach(const XinxAction::ActionList & menu, menuList)
			{
				const QString & menuName = menu.menu();
				const QString & menuId = menu.menuId();

				// Pour chaque élemént du menu
				foreach(XinxAction::MenuItem * item, menu)
				{

					// Si l'élément est un séparateur
					if (dynamic_cast<XinxAction::Separator*>(item))
					{
						if (! m_menus.value(menuId))
						{
							QMenu * newMenu = 0;
							newMenu = new QMenu(menuName, m_menuBar);
							m_menuBar->insertMenu(m_menus[ "tools" ]->menuAction(), newMenu);
							m_menus[ menuId ] = newMenu;
						}
						m_menus[ menuId ]->addSeparator();

						// Si l'élément est une action
					}
					else if (dynamic_cast<XinxAction::Action*>(item))
					{
						QAction * actionToAdd = dynamic_cast<XinxAction::Action*>(item)->action();
						dynamic_cast<XinxAction::Action*>(item)->updateActionState();

						// Si le menu n'existe pas alors le créer.
						if (! m_menus.value(menuId))
						{
							QMenu * newMenu = 0;
							newMenu = new QMenu(menuName, m_menuBar);
							m_menuBar->insertMenu(m_menus[ "tools" ]->menuAction(), newMenu);
							m_menus[ menuId ] = newMenu;
						}
						m_menus[ menuId ]->addAction(actionToAdd);

						// Gestion de la toolbar
						if (dynamic_cast<XinxAction::Action*>(item)->isInToolBar())
						{
							if (! m_toolBars.value(menuId))
							{
								QToolBar * newToolBar = 0;
								newToolBar = new QToolBar(this);
								newToolBar->setWindowTitle(menuName);
								newToolBar->setObjectName(menuId);
								addToolBar(Qt::TopToolBarArea, newToolBar);
								m_toolBars[ menuId ] = newToolBar;
							}
							m_toolBars[ menuId ]->addAction(actionToAdd);
						}

						m_pluginsAction.append(actionToAdd);
					}
				}
			}
		}
	}
}

void MainformImpl::registerTypes()
{
	qRegisterMetaType<TextFileEditor>("TextFileEditor");
}

void MainformImpl::createDockWidget()
{

	m_projectDock = new ProjectDirectoryDockWidget(this);
	m_projectDock->setToggledViewAction(m_toggledFlatView);
	DToolView * view = addToolView(m_projectDock, Qt::LeftDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_projectDock"));
	m_projectDock->setDock(view);

	QAction * action = view->toggleViewAction();
	action->setShortcut(QKeySequence("Alt+1"));
	m_menus["windows"]->addAction(action);
	connect(m_toggledFlatView, SIGNAL(toggled(bool)), m_projectDock, SLOT(toggledView(bool)));
	connect(m_projectDock, SIGNAL(open(QString)), this, SLOT(openFile(QString)));


	m_contentDock = new FileContentDockWidget(this);
	view = addToolView(m_contentDock, Qt::LeftDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_contentDock"));
	action = view->toggleViewAction();
	action->setShortcut(QKeySequence("Alt+2"));
	m_menus["windows"]->addAction(action);
	connect(m_contentDock, SIGNAL(open(QString,int)), this, SLOT(openFile(QString,int)));
	connect(m_tabEditors, SIGNAL(modelChanged(QAbstractItemModel*)), m_contentDock, SLOT(updateModel(QAbstractItemModel*)));
	connect(m_tabEditors, SIGNAL(positionChanged(QModelIndex)), m_contentDock, SLOT(positionChanged(QModelIndex)));

	m_snipetsDock = new SnipetDockWidget(this);
	view = addToolView(m_snipetsDock, Qt::RightDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_snipetsDock"));
	action = view->toggleViewAction();
	action->setShortcut(QKeySequence("Alt+3"));
	m_menus["windows"]->addAction(action);


	m_logDock = new LogDockWidget(this);
	connect(m_logDock, SIGNAL(open(QString,int)), this, SLOT(openFile(QString,int)));
	connect(ErrorManager::self(), SIGNAL(changed()), m_logDock, SLOT(updateErrors()));
	m_logDockView = addToolView(m_logDock, Qt::BottomDockWidgetArea);
	m_logDockView->setObjectName(QString::fromUtf8("m_logDock"));
	action = view->toggleViewAction();
	m_menus["windows"]->addAction(action);
	m_logDock->setDock(m_logDockView);

	// Load dock from plugins and assign automatic shortcut
	int dockShortcut = 4;
	foreach(XinxPluginElement * pluginElement, XinxPluginsLoader::self()->plugins())
	{
		IDockPlugin * dockPlugin = qobject_cast<IDockPlugin*>(pluginElement->plugin());
		if (pluginElement->isActivated() && dockPlugin)
		{
			QList<QWidget*> docks = dockPlugin->createDocksWidget(this);
			foreach(QWidget * dock, docks)
			{
				view = addToolView(dock, Qt::RightDockWidgetArea);
				action = view->toggleViewAction();
				action->setShortcut(QString("Alt+%1").arg(dockShortcut++));
				m_menus["windows"]->addAction(action);
			}
		}
	}
}

void MainformImpl::createNewSubMenu()
{
	QList<QAction*> actions = m_newMenu->actions();
	m_newMenu->clear();
	qDeleteAll(actions);

	foreach(IFileTypePlugin * plugin, XinxPluginsLoader::self()->fileTypes())
	{
		QAction * action = new QAction(
		    QIcon(plugin->icon()),
		    tr("New %1").arg(plugin->description()),
		    this
		);
		action->setData(QVariant::fromValue<IFileTypePlugin*>(plugin));
		m_newMenu->addAction(action);

		connect(action, SIGNAL(triggered()), this, SLOT(newFile()));
	}
}

void MainformImpl::createStatusBar()
{
	m_statusBar = new QStatusBar(this);
	setStatusBar(m_statusBar);

	m_indexingBar = new QProgressBar(this);
	m_indexingBar->setMaximumWidth(150);

	m_codecLabel = new QLabel(tr("Unknown"));
	m_lineFeedLabel = new QLabel(tr("Unknown"));
	m_editorPosition = new QLabel("000x000");
	m_threadCount = new QLabel("000 (000)");
	statusBar()->addPermanentWidget(m_indexingBar);
	statusBar()->addPermanentWidget(m_codecLabel);
	statusBar()->addPermanentWidget(m_lineFeedLabel);
	statusBar()->addPermanentWidget(m_editorPosition);
	statusBar()->addPermanentWidget(m_threadCount);
	statusBar()->showMessage(tr("Ready"), 2000);
}

void MainformImpl::createTools()
{
	connect(ScriptManager::self(), SIGNAL(changed()), this, SLOT(updateToolsMenu()));
	updateToolsMenu();
}

void MainformImpl::createRCS()
{
	connect(RCSManager::self(), SIGNAL(operationStarted()), this, SLOT(logStart()));
	connect(RCSManager::self(), SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()));
	connect(RCSManager::self(), SIGNAL(log(RCS::rcsLog,QString)), m_logDock, SLOT(log(RCS::rcsLog,QString)));
}

void MainformImpl::callSnipetAction(int snipetId)
{
	Q_ASSERT(m_tabEditors->currentEditor() != NULL);

	// TODO : Merge with xinxcodeedit
	// TODO : If no editor, if script return false ... disable action. maybe manage a setEditor on the menu.
	if (TabEditor::isTextFileEditor(m_tabEditors->currentEditor()))
	{
		QString result;
		if (SnipetManager::self()->callSnipet(snipetId, &result, this))
		{
			XinxCodeEdit * editor = static_cast<TextFileEditor*>(m_tabEditors->currentEditor())->textEdit();
			editor->insertText(result);
		}
	}
	else
	{
		qWarning() << tr("Snipet is not supported in this kind of editor.");
	}
}

void MainformImpl::callScriptAction()
{
	QAction * action = qobject_cast<QAction*>(sender());
	if (action)
	{
		ScriptValue qsScript = action->data().value<ScriptValue>();
		qsScript.callScript();
	}
}

void MainformImpl::updateToolsMenu()
{
	/* Scripts */
	qDeleteAllLater(m_scriptActs);
	m_scriptActs.clear();

	foreach(const ScriptValue & s, ScriptManager::self()->objects())
	{
		QAction * action = new QAction(this);
		action->setText(s.text());
		action->setData(QVariant::fromValue(s));
		m_scriptMenu->addAction(action);

		connect(action, SIGNAL(triggered()), this, SLOT(callScriptAction()));
		m_scriptActs.append(action);
	}

	updateActions();
}

void MainformImpl::currentTabChanged(int index)
{
	AbstractEditor * editor = m_tabEditors->editor(index);
	ScriptManager::self()->setCurrentEditeur(editor);
	m_snipetsDock->setEditor(editor);
	updateEditorInformations();

	// Update the state of action
	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		if (e->isActivated() && qobject_cast<IXinxPlugin*>(e->plugin()))
		{
			XinxAction::MenuList menuList = qobject_cast<IXinxPlugin*>(e->plugin())->actions();
			menuList.updateMenuState();
		}
	}

}

void MainformImpl::updateEditorInformations()
{
	AbstractEditor * editor = m_tabEditors->currentEditor();
	if (qobject_cast<TextFileEditor*>(editor))      // This is a text file editor. So we have codec, ...
	{
		TextFileEditor * textFileEditor = qobject_cast<TextFileEditor*>(editor);
		m_codecLabel->setText(textFileEditor->codec()->name());
		switch (textFileEditor->eol())
		{
		case TextFileEditor::MacEndOfLine:
			m_lineFeedLabel->setText("OLD MAC");
			break;
		case TextFileEditor::WindowsEndOfLine:
			m_lineFeedLabel->setText("WIN");
			break;
		case TextFileEditor::UnixEndOfLine:
			m_lineFeedLabel->setText("UNIX");
			break;
		}
	}
	else
	{
		m_codecLabel->setText(tr("No codec"));
		m_lineFeedLabel->setText(tr("Unknown"));
	}
}

void MainformImpl::openWelcomDialog()
{
	WelcomDialogImpl dlg(this);

	int numRecentFiles = qMin(XINXConfig::self()->config().project.recentProjectFiles.size(), MAXRECENTFILES);
	for (int i = 0; i < numRecentFiles; i++)
	{
		dlg.addProjectFile(XINXConfig::self()->config().project.recentProjectFiles[i]);
	}

	connect(&dlg, SIGNAL(createNewProject()), SLOT(newProject()));
	connect(&dlg, SIGNAL(openExistingProject()), SLOT(openProject()));
	connect(&dlg, SIGNAL(requestProject(QString)), SLOT(openProject(QString)));

	dlg.exec();
}

void MainformImpl::newFile()
{
	if (qobject_cast<QAction*>(sender()))
	{
		IFileTypePlugin* plugin = qobject_cast<QAction*>(sender())->data().value<IFileTypePlugin*>();
		m_tabEditors->newEditor(plugin);
		updateActions();
	}
}

void MainformImpl::newFile(const QString &filename)
{
	IFileTypePlugin * plugin = XinxPluginsLoader::self()->matchedFileType(filename);
	m_tabEditors->newEditor(plugin);
}

void MainformImpl::openRecentProject()
{
	QAction * action = qobject_cast<QAction*>(sender());
	if (action)
		openProject(action->data().toString());
}

void MainformImpl::openRecentFile()
{
	QAction * action = qobject_cast<QAction*>(sender());
	if (action)
		openFile(action->data().toString());
}

void MainformImpl::openFile(const QString & name, int line)
{
	if (!name.isEmpty())
		openFile(name);

	// Deplace to rigth line.
	TextFileEditor * ed = qobject_cast<TextFileEditor*>(m_tabEditors->currentEditor());
	ed->textEdit()->gotoLine(line);
	ed->textEdit()->setFocus(Qt::OtherFocusReason);
}

void MainformImpl::openFile()
{
	Q_ASSERT(XINXConfig::self());

	QStringList selectedFiles = QFileDialog::getOpenFileNames(this, tr("Open text file"), m_lastFileName, XinxPluginsLoader::self()->openDialogBoxFilters().join(";;"));

	m_tabEditors->setUpdatesEnabled(false);
	foreach(const QString & filename, selectedFiles)
	{
		m_lastFileName = QFileInfo(filename).absolutePath();
		openFile(filename);
	}
	m_tabEditors->setUpdatesEnabled(true);
}

void MainformImpl::currentCloseFile()
{
	fileEditorClose(m_tabEditors->currentIndex());
}

void MainformImpl::fileEditorRefreshFile(int index)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < m_tabEditors->count());

	AbstractEditor * ed = qobject_cast<AbstractEditor*>(m_tabEditors->editor(index));
	if (ed)
	{
		bool act = ! ed->isModified();

		if (! act)
		{
			QMessageBox::StandardButton ret;
			ret = QMessageBox::question(this, tr("Application"),
			                            tr("The document %1 has been modified.\n"
			                               "Do you really want refresh this?").arg(ed->getTitle()),
			                            QMessageBox::Yes | QMessageBox::No);
			act = (ret == QMessageBox::Yes);
		}
		if (act)
		{
			ScriptManager::self()->callScriptsBeforeLoad();
			ed->loadFromFile();
			ScriptManager::self()->callScriptsAfterLoad();
		}
	}
}

void MainformImpl::refreshFile()
{
	int item = m_tabEditors->getClickedTab();
	if (item == -1) item = m_tabEditors->currentIndex();
	fileEditorRefreshFile(item);
}

void MainformImpl::saveFile()
{
	int item = m_tabEditors->getClickedTab();
	if (item == -1) item = m_tabEditors->currentIndex();
	fileEditorSave(item);
}

void MainformImpl::saveAsFile()
{
	int item = m_tabEditors->getClickedTab();
	if (item == -1) item = m_tabEditors->currentIndex();
	fileEditorSaveAs(item);
}

void MainformImpl::about()
{
	AboutDialogImpl about(this);
	about.exec();
}

void MainformImpl::customize()
{
	CustomDialogImpl custom(this);
	custom.loadConfig();

	if (custom.exec())
	{
		createPluginsActions();
	}
}

void MainformImpl::setupRecentMenu(QMenu * menu, QAction * & separator, QAction * recentActions[ MAXRECENTFILES ])
{
	separator = menu->addSeparator();
	separator->setVisible(false);
	for (int i = 0; i < MAXRECENTFILES; i++)
	{
		recentActions[i] = new QAction(this);
		recentActions[i]->setVisible(false);
		menu->addAction(recentActions[i]);
	}
}

void MainformImpl::updateRecentFiles()
{
	int numRecentFiles;
	if (XINXProjectManager::self()->project())
	{
		numRecentFiles = qMin(XINXProjectManager::self()->project()->session()->lastOpenedFile().size(), MAXRECENTFILES);

		for (int i = 0; i < numRecentFiles; i++)
		{
			QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(XINXProjectManager::self()->project()->session()->lastOpenedFile()[i]).fileName());
			if (XinxPluginsLoader::self()->matchedFileType(QFileInfo(text).fileName()))
				m_recentFileActs[i]->setIcon(QIcon(XinxPluginsLoader::self()->matchedFileType(QFileInfo(text).fileName())->icon()));
			m_recentFileActs[i]->setText(text);
			m_recentFileActs[i]->setData(XINXProjectManager::self()->project()->session()->lastOpenedFile()[i]);
			m_recentFileActs[i]->setVisible(true);
		}
	}
	else
		numRecentFiles = 0;

	for (int j = numRecentFiles; j < MAXRECENTFILES; j++)
		m_recentFileActs[j]->setVisible(false);

	m_recentFileSeparator->setVisible(numRecentFiles > 0);
}

void MainformImpl::updateRecentProjects()
{
	int numRecentFiles = qMin(XINXConfig::self()->config().project.recentProjectFiles.size(), MAXRECENTFILES);

	for (int i = 0; i < numRecentFiles; i++)
	{
		QString filename, projectName;
		filename = XINXConfig::self()->config().project.recentProjectFiles[i];
		try
		{
			projectName = QString("%1 (%2)").arg(XinxProject(filename).projectName()).arg(QFileInfo(filename).fileName());
		}
		catch (XinxProjectException e)
		{
			projectName = QFileInfo(filename).fileName();
		}

		QString text = tr("&%1 %2").arg(i + 1).arg(projectName);
		m_recentProjectActs[i]->setText(text);
		m_recentProjectActs[i]->setData(XINXConfig::self()->config().project.recentProjectFiles[i]);
		m_recentProjectActs[i]->setVisible(true);
	}

	for (int j = numRecentFiles; j < MAXRECENTFILES; j++)
		m_recentProjectActs[j]->setVisible(false);

	m_recentSeparator->setVisible(numRecentFiles > 0);
}

void MainformImpl::readWindowSettings()
{
	resize(XINXConfig::self()->config().size);
	move(XINXConfig::self()->config().position);
	if (XINXConfig::self()->config().maximized)
		setWindowState(windowState() ^ Qt::WindowMaximized);

	if (! XINXConfig::self()->config().state.isEmpty())
	{
		if (! restoreState(XINXConfig::self()->config().state))
			qWarning("Can't restore windows state.\n");
	}
	restoreGUI();
}

void MainformImpl::storeWindowSettings()
{
	XINXConfig::self()->config().state = saveState();

	XINXConfig::self()->config().position = pos();
	XINXConfig::self()->config().size = size();
	XINXConfig::self()->config().maximized = isMaximized();

	XINXConfig::self()->save();
	saveGUI();
}

void MainformImpl::createTabEditorButton()
{
	m_closeTabBtn = new QToolButton(m_tabEditors);
	m_closeTabBtn->setIcon(QIcon(":/images/tabclose.png"));
	connect(m_closeTabBtn, SIGNAL(clicked()), this, SLOT(currentCloseFile()));

	m_tabEditors->setCornerWidget(m_closeTabBtn);

#if QT_VERSION >= 0x040500
	m_closeTabBtn->setVisible(! XINXConfig::self()->config().editor.hideCloseTab);
	m_tabEditors->setTabsClosable(XINXConfig::self()->config().editor.closeButtonOnEachTab);
#else /* QT_VERSION >= 0x040500 */
	m_closeTabBtn->setVisible(true);
#endif /* QT_VERSION >= 0x040500 */

	connect(XINXConfig::self(), SIGNAL(changed()), this, SLOT(updateConfigElement()));
}

void MainformImpl::updateConfigElement()
{
	if (m_showSpaceAndTabAct->isChecked() != XINXConfig::self()->config().editor.showTabulationAndSpace)
		m_showSpaceAndTabAct->setChecked(XINXConfig::self()->config().editor.showTabulationAndSpace);

	if (m_alwaysShowRunDialog->isChecked() != XINXConfig::self()->config().snipets.alwaysShowDialog)
		m_alwaysShowRunDialog->setChecked(XINXConfig::self()->config().snipets.alwaysShowDialog);

#if QT_VERSION >= 0x040500
	m_closeTabBtn->setVisible(! XINXConfig::self()->config().editor.hideCloseTab);
	m_tabEditors->setTabsClosable(XINXConfig::self()->config().editor.closeButtonOnEachTab);
#else /* QT_VERSION >= 0x040500 */
	m_closeTabBtn->setVisible(true);
#endif /* QT_VERSION >= 0x040500 */

	createNewSubMenu();
}

void MainformImpl::updateActions()
{
	/* Project action */
	m_saveProjectAct->setEnabled(XINXProjectManager::self()->project() != NULL);
	m_closeProjectAct->setEnabled(XINXProjectManager::self()->project() != NULL);
	m_closeProjectWithSessionAct->setEnabled(XINXProjectManager::self()->project() != NULL);
	m_closeProjectNoSessionAct->setEnabled(XINXProjectManager::self()->project() != NULL);
	m_projectPropertyAct->setEnabled(XINXProjectManager::self()->project() != NULL);

	m_toggledFlatView->setEnabled(XINXProjectManager::self()->project() != NULL);
	m_projectDock->setEnabled(XINXProjectManager::self()->project() != NULL);

	/* Files */
	m_saveAct->setEnabled(m_tabEditors->count());
	m_saveAsAct->setEnabled(m_tabEditors->count());
	m_saveAllAct->setEnabled(m_tabEditors->count());
	m_closeAct->setEnabled(m_tabEditors->count());
	m_closeAllAct->setEnabled(m_tabEditors->count());
	m_printAct->setEnabled(m_tabEditors->count());
	m_previousTabAct->setEnabled(m_tabEditors->count());
	m_nextTabAct->setEnabled(m_tabEditors->count());
	m_bookmarkAct->setEnabled(m_tabEditors->count());
	m_nextBookmarkAct->setEnabled(m_tabEditors->count());
	m_previousBookmarkAct->setEnabled(m_tabEditors->count());

	if (m_tabEditors->count() == 0)
	{
		m_contentDock->updateModel(NULL);
	}
}

void MainformImpl::updateRCS()
{
	if (XINXProjectManager::self()->project())
		RCSManager::self()->setCurrentRCS(XINXProjectManager::self()->project()->projectRCS());
	else
		RCSManager::self()->setCurrentRCS(QString());
}

bool MainformImpl::fileEditorMayBeSave(int index)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < m_tabEditors->count());

	AbstractEditor * ed = qobject_cast<AbstractEditor*>(m_tabEditors->editor(index));
	if (ed && ed->isModified())
	{
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("Application"),
		                           tr("The document %1 has been modified.\n"
		                              "Do you want to save your changes?").arg(ed->getTitle()),
		                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save)
		{
			fileEditorSave(index);
		}
		else if (ret == QMessageBox::Cancel)
			return false;
	}

	return true;
}

void MainformImpl::fileEditorSave(int index, bool saveAs)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < m_tabEditors->count());
	Q_ASSERT(qobject_cast<AbstractEditor*>(m_tabEditors->editor(index)));

	const QString filename     = qobject_cast<AbstractEditor*>(m_tabEditors->editor(index))->lastFileName();
	const QString deffilename  = qobject_cast<AbstractEditor*>(m_tabEditors->editor(index))->defaultFileName();
	const QString filter       = XinxPluginsLoader::self()->fileTypeFilter(XinxPluginsLoader::self()->matchedFileType(filename.isEmpty() ? deffilename : filename));
	const bool    emptyname    = filename.isEmpty();

	bool    accept      = false;
	QString newFilename;

	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		IXinxInputOutputPlugin * plugin = qobject_cast<IXinxInputOutputPlugin*>(e->plugin());

		if (e->isActivated() && plugin)
		{
			newFilename = plugin->getFilename(filename, filter, saveAs, accept, this);
			if (accept) break;
		}
	}
	if (! accept)
	{
		newFilename = filename;
		if (saveAs || emptyname)
		{
			if (newFilename.isEmpty() && XINXProjectManager::self()->project())
			{
				newFilename = XINXProjectManager::self()->project()->projectPath();
			}
			newFilename = QFileDialog::getSaveFileName(this, tr("Save text file"), newFilename, filter, const_cast<QString*>(&filter));
		}
	}
	if (newFilename.isEmpty())
		return;

	if (! QFile(newFilename).exists())
		RCSManager::self()->addFileOperation(RCSManager::RCS_ADD, QStringList() << newFilename, this);

	ScriptManager::self()->callScriptsBeforeSave();
	qobject_cast<AbstractEditor*>(m_tabEditors->editor(index))->saveToFile(newFilename);
	m_projectDock->refreshPath(QFileInfo(newFilename).absoluteFilePath());
	ScriptManager::self()->callScriptsAfterSave();

	RCSManager::self()->validFileOperations();

	statusBar()->showMessage(tr("File %1 saved").arg(m_tabEditors->editor(index)->getTitle()), 2000);
}

void MainformImpl::fileEditorSave(int index)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < m_tabEditors->count());
	Q_ASSERT(qobject_cast<AbstractEditor*>(m_tabEditors->editor(index)));

	fileEditorSave(index, false);
}

void MainformImpl::fileEditorSaveAs(int index)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < m_tabEditors->count());
	Q_ASSERT(qobject_cast<AbstractEditor*>(m_tabEditors->editor(index)));

	fileEditorSave(index, true);
}

void MainformImpl::fileEditorClose(int index)
{
	if (fileEditorMayBeSave(index))
	{
		m_contentDock->updateModel(NULL);
		AbstractEditor * editor = m_tabEditors->editor(index);
		m_tabEditors->removeTab(index);
		delete editor;
	}

	updateActions();
}

void MainformImpl::printFile()
{
	Q_ASSERT(m_tabEditors->currentEditor() != NULL);

	if (TabEditor::isTextFileEditor(m_tabEditors->currentEditor()))
	{
		TextFileEditor * editor = static_cast<TextFileEditor*>(m_tabEditors->currentEditor());
		QPrinter printer;

		QPrintDialog *dlg = new QPrintDialog(&printer, this);
		if (dlg->exec() != QDialog::Accepted) return;

		editor->textEdit()->print(&printer);

		delete dlg;

		statusBar()->showMessage(tr("Printed"), 2000);
	}
}


void MainformImpl::logStart()
{
	m_logDock->init();
	m_rcsVisible = m_logDock->isVisible();
	m_logDockView->show();
}

void MainformImpl::logTimeout()
{
	m_timer->stop();
	m_logDockView->setVisible(false);
}

void MainformImpl::rcsLogTerminated()
{
	m_logDock->end();
	if ((!m_rcsVisible) && m_logDockView->isVisible() && XINXConfig::self()->config().project.closeVersionManagementLog)
		m_timer->start(5000);
}


void MainformImpl::nextTab()
{
	m_tabEditors->setCurrentIndex((m_tabEditors->currentIndex() + 1) % m_tabEditors->count());
}

void MainformImpl::previousTab()
{
	m_tabEditors->setCurrentIndex((m_tabEditors->currentIndex() - 1 + m_tabEditors->count()) % m_tabEditors->count());
}

void MainformImpl::copyFileName()
{
	int item = m_tabEditors->getClickedTab();
	if (item == -1) item = m_tabEditors->currentIndex();
	QString name = m_tabEditors->editor(item)->getTitle();
	qApp->clipboard()->setText(name);
}

void MainformImpl::copyPath()
{
	int item = m_tabEditors->getClickedTab();
	if (item == -1) item = m_tabEditors->currentIndex();
	AbstractEditor * ed = dynamic_cast<AbstractEditor*>(m_tabEditors->editor(item));
	if (ed)
	{
		QString name = QDir::toNativeSeparators(ed->lastFileName());
		qApp->clipboard()->setText(name);
	}
}

void MainformImpl::updateSpaceAndTab()
{
	if (XINXConfig::self()->config().editor.showTabulationAndSpace != m_showSpaceAndTabAct->isChecked())
	{
		XINXConfig::self()->config().editor.showTabulationAndSpace = m_showSpaceAndTabAct->isChecked();
		XINXConfig::self()->save();
	}
}

void MainformImpl::changeShowSnipetDialogAction()
{
	if (XINXConfig::self()->config().snipets.alwaysShowDialog != m_alwaysShowRunDialog->isChecked())
	{
		XINXConfig::self()->config().snipets.alwaysShowDialog = m_alwaysShowRunDialog->isChecked();
		XINXConfig::self()->save();
	}
}

void MainformImpl::createFindReplace()
{
	m_findDialog       = new ReplaceDialogImpl(this);
	connect(m_findDialog, SIGNAL(find(QString, QString, AbstractEditor::SearchOptions)), this, SLOT(findFirst(QString, QString, AbstractEditor::SearchOptions)));
	connect(m_findDialog, SIGNAL(findInFiles(QString, QString, QString, AbstractEditor::SearchOptions)), this, SLOT(findInFiles(QString, QString, QString, AbstractEditor::SearchOptions)));

	m_replaceNextDlg   = new QMessageBox(QMessageBox::Question, tr("Replace text"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	m_replaceNextDlg->setWindowModality(Qt::NonModal);
}

void MainformImpl::findFirst(const QString & chaine, const QString & dest, const AbstractEditor::SearchOptions & options)
{
	Q_ASSERT(m_tabEditors->currentEditor());

	AbstractEditor * editor = m_tabEditors->currentEditor();

	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;
	m_nbFindedText      = 0;
	m_searchInverse     = false;

	editor->initSearch(m_findOptions);
	findNext();
}

void MainformImpl::findInFiles(const QString & directory, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options)
{
	m_searchAct->setEnabled(false);
	m_searchNextAct->setEnabled(false);
	m_searchPreviousAct->setEnabled(false);
	m_replaceAct->setEnabled(false);
	m_logDock->init();
	m_logDockView->setVisible(true);

	SearchFileThread * threadSearch = new SearchFileThread();
	connect(threadSearch, SIGNAL(find(QString,QString,int)), m_logDock, SLOT(find(QString,QString,int)), Qt::BlockingQueuedConnection);
	connect(threadSearch, SIGNAL(end()), this, SLOT(findEnd()));

	threadSearch->setPath(directory);
	threadSearch->setSearchString(from, to, options);
	threadSearch->search();
}

void MainformImpl::findEnd()
{
	m_searchAct->setEnabled(true);
	m_searchNextAct->setEnabled(true);
	m_searchPreviousAct->setEnabled(true);
	m_replaceAct->setEnabled(true);

	m_logDock->end();

	QMessageBox::information(this, tr("Search End"), tr("All string are finded"));
}

void MainformImpl::findNext()
{
	Q_ASSERT(m_tabEditors->currentEditor());

	AbstractEditor * editor = m_tabEditors->currentEditor();

	bool continuer = true;

	while (continuer)
	{
		continuer = false;

		AbstractEditor::SearchOptions options = m_findOptions;
		if (m_searchInverse) options |= AbstractEditor::BACKWARD;
		bool trouve = editor->find(m_findExpression, options);

		if (trouve)
		{
			m_nbFindedText++;

			if (! m_replaceExpression.isNull())
			{
				QMessageBox::StandardButton ret = QMessageBox::Yes;

				if (! m_yesToAllReplace)
				{
					if (m_nbFindedText == 1)
						m_replaceNextDlg->setAttribute(Qt::WA_Moved, false);
					m_replaceNextDlg->show();
					m_replaceNextDlg->setAttribute(Qt::WA_Moved);
					while (m_replaceNextDlg->isVisible()) qApp->processEvents();
					ret = m_replaceNextDlg->standardButton(m_replaceNextDlg->clickedButton());
				}

				switch (ret)
				{
				case QMessageBox::Yes:
					// Replace chaine
					editor->replace(m_findExpression, m_replaceExpression, options);
					continuer = true;
					break;
				case QMessageBox::YesToAll:
					editor->replace(m_findExpression, m_replaceExpression, options);
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
		else
		{
			if (m_yesToAllReplace && ! m_replaceExpression.isNull())
			{
				QMessageBox::information(this,
				                         tr("Search/Replace"),
				                         tr("%1 occurences of '%2' replaced.").arg(m_nbFindedText).arg(m_findExpression),
				                         QMessageBox::Ok);
			}
			else
			{
				QMessageBox::StandardButton ret = QMessageBox::warning(this,
				                                  tr("Search/Replace"),
				                                  tr("%1 occurences of '%2' %3. Return to the beginning of the document ?").arg(m_nbFindedText).arg(m_findExpression).arg(! m_replaceExpression.isNull() ? tr("replaced") : tr("finded")),
				                                  QMessageBox::Yes | QMessageBox::No);

				if (ret == QMessageBox::Yes)
				{
					m_findOptions   &= ~ AbstractEditor::SearchOptions(AbstractEditor::ONLY_SELECTION);
					options         &= ~ AbstractEditor::SearchOptions(AbstractEditor::ONLY_SELECTION);
					options         |= AbstractEditor::SEARCH_FROM_START;
					editor->initSearch(options);

					continuer = true;
				}
			}
			m_nbFindedText = 0;
		}
	}
	m_yesToAllReplace = false;
}

void MainformImpl::findPrevious()
{
	m_searchInverse = true;
	findNext();
	m_searchInverse = false;
}

void MainformImpl::find()
{
	if (qobject_cast<TextFileEditor*>(m_tabEditors->currentEditor()))
	{
		XinxCodeEdit * textEdit = qobject_cast<TextFileEditor*>(m_tabEditors->currentEditor())->textEdit();
		if (! textEdit->textCursor().selectedText().isEmpty())
		{
			m_findDialog->setText(textEdit->textCursor().selectedText());
		}
	}
	m_findDialog->initialize(m_tabEditors->count() > 0);
	m_findDialog->setReplace(false);
	m_findDialog->exec();
}

void MainformImpl::replace()
{
	Q_ASSERT(m_tabEditors->currentEditor());
	Q_ASSERT(qobject_cast<TextFileEditor*>(m_tabEditors->currentEditor()));

	XinxCodeEdit * textEdit = static_cast<TextFileEditor*>(m_tabEditors->currentEditor())->textEdit();
	if (! textEdit->textCursor().selectedText().isEmpty())
	{
		m_findDialog->setText(textEdit->textCursor().selectedText());
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(true);
	m_findDialog->exec();
}

bool MainformImpl::closeProject(bool session, bool showWelcome)
{
	if (! XINXProjectManager::self()->project()) return false;

	saveProject(session);

	if (! session)
	{
		if (! closeAllFile())
		{
			return false;
		}
	}
	else
	{
		m_tabEditors->setUpdatesEnabled(false);
		for (int i = m_tabEditors->count() - 1; i >= 0; i--)
		{
			AbstractEditor * ed = m_tabEditors->editor(i);
			m_tabEditors->removeTab(i);
			delete ed;
		}
		m_tabEditors->setUpdatesEnabled(true);
	}
	m_contentDock->updateModel(NULL);

	XINXProjectManager::self()->deleteProject();

	// Update the state of action
	foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
	{
		if (e->isActivated() && qobject_cast<IXinxPlugin*>(e->plugin()))
		{
			XinxAction::MenuList menuList = qobject_cast<IXinxPlugin*>(e->plugin())->actions();
			menuList.updateMenuState();
		}
	}

	if (showWelcome)
		openWelcomDialog();

	return true;
}

void MainformImpl::updateTitle()
{
	if (! XINXProjectManager::self()->project())
		setWindowTitle(tr("XINX"));
	else
	{
		setWindowTitle(tr("%1 - %2").arg(XINXProjectManager::self()->project()->projectName()).arg(tr("XINX")));
	}
}

void MainformImpl::openProject()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open a project"), XINXConfig::self()->config().project.defaultPath, tr("Projet (*%1)").arg(XINX_PROJECT_EXTENTION));
	if (! fileName.isEmpty())
		openProject(fileName);
}

void MainformImpl::projectProperty()
{
	Q_ASSERT(XINXProjectManager::self()->project() != NULL);

	ProjectPropertyImpl property(this);
	property.loadFromProject(XINXProjectManager::self()->project());

	if (property.exec())
	{
		property.saveToProject(XINXProjectManager::self()->project());
		XINXProjectManager::self()->project()->saveToFile();
		ExternalFileResolver::self()->clearCache();
	}
}

void MainformImpl::closeProject()
{
	closeProject(XINXConfig::self()->config().project.saveWithSessionByDefault);
}

void MainformImpl::setEditorPosition(int line, int column)
{
	m_editorPosition->setText(QString("   %1 x %2   ").arg(line, 3, 10, QLatin1Char('0')).arg(column, 3, 10, QLatin1Char('0')));
}

void MainformImpl::closeEvent(QCloseEvent *event)
{
	emit aboutToClose();
	storeWindowSettings(); // Store before the project is closed

	if (XINXProjectManager::self()->project())
	{
		if (! closeProject(XINXConfig::self()->config().project.saveWithSessionByDefault, false))
		{
			event->ignore();
			return;
		}
	}
	else if (! closeAllFile())
	{
		XINXConfig::self()->config().project.lastOpenedProject = QString(); // Pas de projet ouvert
		event->ignore();
		return;
	}

	event->accept();
}

void MainformImpl::timerEvent(QTimerEvent *)
{
	int threadCount      = XinxThreadManager::self()->getThreadCount(),
	                       threadClassCount = XinxThreadManager::self()->getThreadClassCount();

	QString threadCountText = QString("Xinx Thread : %1 (%2) / Other thread %3/%4")
	                          .arg(threadCount, 3, 10, QLatin1Char('0'))
	                          .arg(threadClassCount, 3, 10, QLatin1Char('0'))
	                          .arg(QThreadPool::globalInstance()->activeThreadCount(), 3, 10, QLatin1Char('0'))
	                          .arg(QThreadPool::globalInstance()->maxThreadCount(), 3, 10, QLatin1Char('0'));

	if ((threadCount + QThreadPool::globalInstance()->activeThreadCount()) > QThread::idealThreadCount())
		threadCountText = "<font color=\"red\">" + threadCountText + "</font>";

	m_threadCount->setText(threadCountText);
}

void MainformImpl::openFile(const QString & filename)
{
	if (filename.isEmpty()) return;  // The file name must not to be empty

	// Add recent action
	if (XINXProjectManager::self()->project())
	{
		XINXProjectManager::self()->project()->session()->lastOpenedFile().removeAll(filename);
		XINXProjectManager::self()->project()->session()->lastOpenedFile().prepend(filename);

		while (XINXProjectManager::self()->project()->session()->lastOpenedFile().size() > MAXRECENTFILES)
			XINXProjectManager::self()->project()->session()->lastOpenedFile().removeLast();
	}

	// Load the file in the editor
	ScriptManager::self()->callScriptsBeforeLoad();

	m_tabEditors->openFilename(QDir::fromNativeSeparators(filename));
	updateRecentFiles();
	updateActions();
	ScriptManager::self()->callScriptsAfterLoad();
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainformImpl::saveFileAs(const QString & name)
{
	if (! m_tabEditors->currentEditor()) return;  // Can't save the editor if no editor to save.
	ScriptManager::self()->callScriptsBeforeSave();
	qobject_cast<TextFileEditor*>(m_tabEditors->currentEditor())->saveToFile(name);
	ScriptManager::self()->callScriptsAfterSave();
}

void MainformImpl::saveAllFile()
{
	for (int i = 0; i < m_tabEditors->count(); i++)
	{
		fileEditorSave(i);
	}
}

void MainformImpl::closeFile()
{
	int item = m_tabEditors->getClickedTab();
	if (item == -1) item = m_tabEditors->currentIndex();
	if (item == -1) return;  // Can't close editor if no editor to close
	fileEditorClose(item);
}

bool MainformImpl::closeAllFile()
{
	m_tabEditors->setUpdatesEnabled(false);
	for (int i = m_tabEditors->count() - 1; i >= 0; i--)
	{
		if (fileEditorMayBeSave(i))
		{
			m_contentDock->updateModel(NULL);
			AbstractEditor * editor = m_tabEditors->editor(i);
			m_tabEditors->removeTab(i);
			delete editor;
		}
		else
		{
			m_tabEditors->setUpdatesEnabled(true);
			return false;
		}
	}
	m_tabEditors->setUpdatesEnabled(true);

	updateActions();
	return true;
}

void MainformImpl::newProject()
{
	NewProjectWizard wizard;
	if (wizard.exec())
	{
		XinxProject * project = wizard.createProject();

		QString filename = QFileDialog::getSaveFileName(this, tr("Save a project"), project->projectPath(), tr("Projet (*%1)").arg(XINX_PROJECT_EXTENTION));
		if (filename.isEmpty())
		{
			delete project;
			return;
		}
		m_lastProjectOpenedPlace = project->projectPath();

		project->saveToFile(filename);
		delete project;

		openProject(filename);
	}
}

void MainformImpl::openProject(const QString & filename)
{
	if (filename.isEmpty()) return;  // The project filename is not empty

	if (XINXProjectManager::self()->project())
		closeProject(XINXConfig::self()->config().project.saveWithSessionByDefault, false);
	else
		closeAllFile();

	XINXConfig::self()->config().project.recentProjectFiles.removeAll(filename);

	XinxProject * project = NULL;
	try
	{
		project = new XinxProject(filename);

		m_lastProjectOpenedPlace = QFileInfo(filename).absolutePath();
		m_lastFileName           = m_lastProjectOpenedPlace;

		XINXConfig::self()->config().project.recentProjectFiles.prepend(filename);
		while (XINXConfig::self()->config().project.recentProjectFiles.size() > MAXRECENTFILES)
			XINXConfig::self()->config().project.recentProjectFiles.removeLast();

		XINXProjectManager::self()->deleteProject();

		connect(ContentView2::Manager::self()->cache(), SIGNAL(cacheLoaded()), m_indexingBar, SLOT(hide()));
		connect(ContentView2::Manager::self()->cache(), SIGNAL(progressRangeChanged(int,int)), m_indexingBar, SLOT(setRange(int,int)));
		connect(ContentView2::Manager::self()->cache(), SIGNAL(progressValueChanged(int)), m_indexingBar, SLOT(setValue(int)));
		connect(ContentView2::Manager::self()->cache(), SIGNAL(progressValueChanged(int)), m_indexingBar, SLOT(show()));

		XINXProjectManager::self()->setCurrentProject(project);

		foreach(XinxPluginElement * e, XinxPluginsLoader::self()->plugins())
		{
			if (e->isActivated() && qobject_cast<IXinxInputOutputPlugin*>(e->plugin()) && (! qobject_cast<IXinxInputOutputPlugin*>(e->plugin())->loadProject(XINXProjectManager::self()->project())))
				qWarning(qPrintable(tr("Can't start a project for plugin \"%1\"").arg(qobject_cast<IXinxPlugin*>(e->plugin())->getPluginAttribute(IXinxPlugin::PLG_NAME).toString())));
		}

		m_tabEditors->setUpdatesEnabled(false);
		foreach(XinxProjectSessionEditor * data, project->session()->serializedEditors())
		{
			m_tabEditors->addTab(EditorFactory::self()->createEditor(data));
		}
		if (m_tabEditors->count() > 0) m_tabEditors->setCurrentIndex(0);
		m_tabEditors->setUpdatesEnabled(true);

		updateActions();
	}
	catch (XinxProjectException e)
	{
		delete project;

		ProjectWizard wizard(filename);
		if (wizard.exec() == QDialog::Accepted)
		{
			openProject(filename);
		}
	}
}

void MainformImpl::closeProjectNoSessionData()
{
	closeProject(false);
}

void MainformImpl::closeProjectWithSessionData()
{
	closeProject(true);
}

void MainformImpl::saveProject(bool withSessionData)
{
	Q_ASSERT(XINXProjectManager::self()->project());

	qDeleteAll(XINXProjectManager::self()->project()->session()->serializedEditors());
	XINXProjectManager::self()->project()->session()->serializedEditors().clear();
	for (int i = 0; i < m_tabEditors->count(); i++)
	{
		m_tabEditors->editor(i)->serialize(new XinxProjectSessionEditor(XINXProjectManager::self()->project()->session()), withSessionData);
	}
	XINXProjectManager::self()->project()->saveOnlySession();
}

