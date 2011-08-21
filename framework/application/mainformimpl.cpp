/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "mainformimpl.h"
#include <contentview3/dockwidget.h>
#include <project/projectdirectorywidgetimpl.h>
#include <editors/editortabdockwidget.h>
#include <search/replacedialogimpl.h>
#include <core/errordockimpl.h>
#include <versioncontrol/rcslogdockimpl.h>
#include <search/searchdockimpl.h>
#include <snipets/snipetdockwidget.h>
#include <application/aboutdialogimpl.h>
#include <application/customdialog/customdialogimpl.h>
#include <core/uniqueapplication.h>
#include <search/searchfilethread.h>
#include <core/exceptions.h>
#include <scripts/scriptmanager.h>
#include <snipets/snipetmanager.h>
#include <snipets/snipetmenu.h>
#include <dtoolview.h>
#include <dviewbutton.h>
#include <editors/editormanager.h>
#include <project/xinxprojectmanager.h>
#include <versioncontrol/versioncontrolmanager.h>
#include <plugins/xinxpluginsloader.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <plugins/xinxpluginelement.h>
#include <plugins/interfaces/gui.h>
#include <actions/actionmanager.h>
#include <actions/actioninterface.h>
#include <jobs/xinxjobmanager.h>
#include <jobs/xinxjobprogressbar.h>
#include <session/sessionmanager.h>
#include <jobs/xinxjobprogressdock.h>
#include <application/versionavailabledialog.h>

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
#include <QIcon>
#include <QThreadPool>
#include <QDebug>
#include <QProgressDialog>

/* MainformImpl */

MainformImpl::MainformImpl(QWidget * parent) : DMainWindow(parent)
{
	createMainForm();
	createMenus();

	createStatusBar();
	createDockWidget();
	createFindReplace();
	createTools();

	connectSignals();

	// Update the status bar position
	setEditorPosition(tr("No position"));

	// Restore windows property
	readWindowSettings();

	// Create the object that test if a version exists
	new VersionAvailableDialog(this);
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

	setCentralWidget(EditorManager::self()->editorsWidget());

	QMetaObject::connectSlotsByName(this);
}

void MainformImpl::createMenus()
{
	QMenu * sessionMenu, *projectMenu, * fileMenu, * editMenu, *searchMenu, *bookmarkMenu, *windowsMenu, *toolsMenu, *helpMenu;
	QToolBar * projectToolBar, * fileToolBar, * editToolBar, * searchToolBar;
	m_menuBar = new QMenuBar(this);
	setMenuBar(m_menuBar);
	m_menus.insert("session", sessionMenu = new QMenu(tr("Sess&ion"), m_menuBar));
	m_menus.insert("project", projectMenu = new QMenu(tr("&Project"), m_menuBar));
	m_menus.insert("file", fileMenu = new QMenu(tr("&File"), m_menuBar));
	m_menus.insert("edit", editMenu = new QMenu(tr("&Edit"), m_menuBar));
	m_menus.insert("search", searchMenu = new QMenu(tr("&Search"), m_menuBar));
	m_menus.insert("bookmark", bookmarkMenu = new QMenu(tr("&Bookmark"), m_menuBar));
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

	m_menuBar->addAction(sessionMenu->menuAction());
	m_menuBar->addAction(projectMenu->menuAction());
	m_menuBar->addSeparator();
	m_menuBar->addAction(fileMenu->menuAction());
	m_menuBar->addAction(editMenu->menuAction());
	m_menuBar->addAction(searchMenu->menuAction());
	m_menuBar->addAction(bookmarkMenu->menuAction());
	m_menuBar->addSeparator();
	m_menuBar->addAction(toolsMenu->menuAction());
	m_menuBar->addAction(windowsMenu->menuAction());
	m_menuBar->addAction(helpMenu->menuAction());


	sessionMenu->addAction(XinxSession::SessionManager::self()->newSessionAction());
	sessionMenu->addSeparator();
	XinxSession::SessionManager::self()->sessionActions()->setMenu(sessionMenu);
	XinxSession::SessionManager::self()->sessionActions()->setMenuSeparator(sessionMenu->addSeparator());
	sessionMenu->addAction(XinxSession::SessionManager::self()->removeSessionAction());
	sessionMenu->addAction(XinxSession::SessionManager::self()->renameSessionAction());
	sessionMenu->addSeparator();
	sessionMenu->addAction(m_exitAct);

	fileMenu->addAction(EditorManager::self()->newAction());
	fileMenu->addAction(EditorManager::self()->openAction());
	fileMenu->addAction(EditorManager::self()->saveAction());
	fileMenu->addAction(EditorManager::self()->saveAsAction());
	fileMenu->addAction(EditorManager::self()->saveAllAction());
	fileMenu->addSeparator();
	fileMenu->addAction(EditorManager::self()->closeAction());
	fileMenu->addAction(EditorManager::self()->closeAllAction());
	fileMenu->addSeparator();
	fileMenu->addAction(EditorManager::self()->printAction());

	projectMenu->addAction(XinxProject::Manager::self()->newProjectAction());
	projectMenu->addAction(XinxProject::Manager::self()->openProjectAction());
	projectMenu->addAction(XinxProject::Manager::self()->closeProjectAction());
	projectMenu->addAction(XinxProject::Manager::self()->customizeProjectAction());
	projectMenu->addSeparator();
	projectMenu->addAction(VersionControl::Manager::self()->updateAllAction());
	projectMenu->addAction(VersionControl::Manager::self()->commitAllAction());
	projectMenu->addAction(VersionControl::Manager::self()->abortAction());

	windowsMenu->addAction(EditorManager::self()->nextEditorAction());
	windowsMenu->addAction(EditorManager::self()->previousEditorAction());
	windowsMenu->addSeparator();
	windowsMenu->addAction(m_alwaysShowRunDialog);
	windowsMenu->addSeparator();

	toolsMenu->addAction(m_snipetMenu->menuAction());
	toolsMenu->addAction(m_scriptMenu->menuAction());
	toolsMenu->addSeparator();
	toolsMenu->addAction(m_customApplicationAct);

	m_scriptMenu->addAction(m_refreshScripts);
	m_scriptMenu->addSeparator();

	helpMenu->addAction(m_createTicketAct);
	helpMenu->addSeparator();
	helpMenu->addAction(m_aboutAct);
	helpMenu->addAction(m_aboutQtAct);
	helpMenu->addAction(QWhatsThis::createAction(this));

	searchMenu->addAction(m_searchAct);
	searchMenu->addAction(m_searchNextAct);
	searchMenu->addAction(m_searchPreviousAct);
	searchMenu->addSeparator();
	searchMenu->addAction(m_replaceAct);

	projectToolBar->addAction(XinxProject::Manager::self()->newProjectAction());
	projectToolBar->addAction(XinxProject::Manager::self()->openProjectAction());
	projectToolBar->addAction(XinxProject::Manager::self()->closeProjectAction());
	projectToolBar->setOrientation(Qt::Horizontal);
	projectToolBar->setObjectName("projectToolBar");
	addToolBar(Qt::TopToolBarArea, projectToolBar);

	fileToolBar->addAction(EditorManager::self()->newAction());
	fileToolBar->addAction(EditorManager::self()->openAction());
	fileToolBar->addAction(EditorManager::self()->saveAction());
	fileToolBar->addAction(EditorManager::self()->saveAllAction());
	fileToolBar->addSeparator();
	fileToolBar->addAction(EditorManager::self()->printAction());
	fileToolBar->setOrientation(Qt::Horizontal);
	fileToolBar->setObjectName("fileToolBar");
	addToolBar(Qt::TopToolBarArea, fileToolBar);
	editToolBar->setOrientation(Qt::Horizontal);
	editToolBar->setObjectName("editToolBar");
	addToolBar(Qt::TopToolBarArea, editToolBar);

	QToolButton * btn = qobject_cast<QToolButton*>(fileToolBar->widgetForAction(EditorManager::self()->openAction()));
	if (btn) btn->setPopupMode(QToolButton::MenuButtonPopup);

	searchToolBar->addAction(m_searchAct);
	searchToolBar->setOrientation(Qt::Horizontal);
	searchToolBar->setObjectName("searchToolBar");
	addToolBar(Qt::TopToolBarArea, searchToolBar);

	connect(XinxAction::ActionManager::self(), SIGNAL(changed()), this, SLOT(createPluginsActions()));
	XinxAction::ActionManager::self()->generateMenu();
}

void MainformImpl::createActions()
{
	/* FILE */

	// Exit
	m_exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
	m_exitAct->setStatusTip(tr("Quit XINX"));
	m_exitAct->setShortcut(QKeySequence::mnemonic(m_exitAct->text()));
	connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

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

	// Search Next
	m_searchNextAct = new QAction(tr("Search next"), this);
	m_searchNextAct->setIconText(tr("Search next"));
	m_searchNextAct->setToolTip(tr("Search next"));
	m_searchNextAct->setStatusTip(tr("Search the next element in the current editor"));
	m_searchNextAct->setShortcut(QKeySequence::FindNext);
	m_searchNextAct->setEnabled(false);
	connect(m_searchNextAct, SIGNAL(triggered()), this, SLOT(findNext()));

	// Replace
	m_replaceAct = new QAction(tr("&Replace ..."), this);
	m_replaceAct->setStatusTip(tr("Replace a text in the current editor"));
	m_replaceAct->setShortcut(QKeySequence::Replace);
	m_replaceAct->setEnabled(false);
	connect(m_replaceAct, SIGNAL(triggered()), this, SLOT(replace()));

	/* PROJECT */
	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(updateTitle()));
	connect(XinxProject::Manager::self(), SIGNAL(changed()), this, SLOT(updateTitle()));

	/* TOOLS */

	// Custom
	m_customApplicationAct = new QAction(QIcon(":/images/configure.png"), tr("Customize ..."), this);
	connect(m_customApplicationAct, SIGNAL(triggered()), this, SLOT(customize()));

	// Refresh scripts
	m_refreshScripts = new QAction(tr("&Refresh Scripts"), this);
	connect(m_refreshScripts, SIGNAL(triggered()), ScriptManager::self(), SLOT(loadScripts()));

	/* ABOUT */

	// Create Ticket
	m_createTicketAct = new QAction(tr("&Create a ticket ..."), this);
	m_createTicketAct->setStatusTip(tr("Create a new ticket (bug or evolution) on the site."));
	connect(m_createTicketAct, SIGNAL(triggered()), ExceptionManager::self(), SLOT(openTicketDialog()));

	// About Qt
	m_aboutQtAct = new QAction(QIcon(":/images/help-hint.png"), tr("About &Qt"), this);
	m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// About
	m_aboutAct = new QAction(QIcon(":/images/help-about.png"), tr("&About"), this);
	m_aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	// Tab Editor
	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
	connect(EditorManager::self(), SIGNAL(codecChanged()), this, SLOT(updateEditorInformations()));
	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(updateActions()));

	/* WINDOW */
	m_alwaysShowRunDialog = new QAction(tr("Always show run snipet dialog"), this);
	m_alwaysShowRunDialog->setCheckable(true);
	m_alwaysShowRunDialog->setChecked(XINXConfig::self()->config().snipets.alwaysShowDialog);
	connect(m_alwaysShowRunDialog, SIGNAL(triggered()), this, SLOT(changeShowSnipetDialogAction()));
}

void MainformImpl::connectSignals()
{
	connect(EditorManager::self(), SIGNAL(fileOpened(QString)), this, SLOT(sOpenedFile(QString)));
	connect(EditorManager::self(), SIGNAL(filePrinted(QString)), this, SLOT(sPrintedFile(QString)));
	connect(EditorManager::self(), SIGNAL(fileSaved(QString,QString)), this, SLOT(sSavedFile(QString)));
	connect(EditorManager::self(), SIGNAL(fileClosed(QString)), this, SLOT(sClosedFile(QString)));
	connect(EditorManager::self(), SIGNAL(positionChanged(QString)), this, SLOT(setEditorPosition(QString)));
}

void MainformImpl::createPluginsActions()
{
	foreach(const QString & menuId, XinxAction::ActionManager::self()->menus())
	{
		if (! m_menus.value(menuId))
		{
			QMenu * newMenu = new QMenu(XinxAction::ActionManager::self()->nameOfMenu(menuId), m_menuBar);
			m_menuBar->insertMenu(m_menus[ "tools" ]->menuAction(), newMenu);
			m_menus[ menuId ] = newMenu;
		}

		foreach(XinxAction::MenuItem * item, XinxAction::ActionManager::self()->menu(menuId))
		{
			m_menus[ menuId ]->addAction(item->action());
		}
	}

	foreach(const QString & menuId, XinxAction::ActionManager::self()->toolBars())
	{
		if (! m_toolBars.value(menuId))
		{
			QToolBar * newToolBar = new QToolBar(this);
			newToolBar->setWindowTitle(XinxAction::ActionManager::self()->nameOfMenu(menuId));
			newToolBar->setObjectName(menuId);
			addToolBar(Qt::TopToolBarArea, newToolBar);
			m_toolBars[ menuId ] = newToolBar;
		}

		foreach(XinxAction::MenuItem * item, XinxAction::ActionManager::self()->toolBar(menuId))
		{
			m_toolBars[ menuId ]->addAction(item->action());
		}
	}
}

void MainformImpl::createDockWidget()
{
	/* Project */
	m_projectDock = new ProjectDirectoryWidgetImpl(this);
	m_projectDock->setShortcut(QKeySequence("Alt+1"));
	DToolView * view = addToolView(m_projectDock, Qt::LeftDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_projectDock"));
	connect(view, SIGNAL(visibilityChanged(bool)), m_projectDock, SLOT(setFocus()));
	m_projectDock->setDock(view);

	QAction * action = view->toggleViewAction();
	action->setShortcut(m_projectDock->getShortcut());
	m_menus["windows"]->addAction(action);
	connect(m_projectDock, SIGNAL(open(QString,IFileTypePluginPtr,XinxProject::ProjectPtr)), EditorManager::self(), SLOT(openFile(QString,IFileTypePluginPtr,XinxProject::ProjectPtr)));

	/* Content */
	m_contentDock = new ContentView3::DockWidget(this);
	m_contentDock->setShortcut(QKeySequence("Alt+2"));
	view = addToolView(m_contentDock, Qt::LeftDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_contentDock"));
	action = view->toggleViewAction();
	action->setShortcut(m_contentDock->getShortcut());
	m_menus["windows"]->addAction(action);
	connect(m_contentDock, SIGNAL(open(QString,int,IFileTypePluginPtr,XinxProject::ProjectPtr)), EditorManager::self(), SLOT(openFile(QString,int,IFileTypePluginPtr,XinxProject::ProjectPtr)));
	m_contentDock->setDock(view);

	/* List of opened files */
	m_editorTabDock = new EditorTabDockWidget(this);
	view = addToolView(m_editorTabDock, Qt::LeftDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_editorTabDock"));
	action = view->toggleViewAction();
	m_menus["windows"]->addAction(action);
	connect(m_editorTabDock, SIGNAL(open(QString)), EditorManager::self(), SLOT(openFile(QString)));
	m_editorTabDock->setDock(view);

	/* Snipets */
	m_snipetsDock = new SnipetDockWidget(this);
	m_snipetsDock->setShortcut(QKeySequence("Alt+3"));
	view = addToolView(m_snipetsDock, Qt::RightDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_snipetsDock"));
	action = view->toggleViewAction();
	action->setShortcut(m_snipetsDock->getShortcut());
	m_menus["windows"]->addAction(action);
	connect(view, SIGNAL(visibilityChanged(bool)), m_snipetsDock, SLOT(setFocus()));
	m_snipetsDock->setDock(view);

	/* Error */
	m_errorDock = new ErrorDockWidgetImpl(this);
	connect(m_errorDock, SIGNAL(open(QString,int)), EditorManager::self(), SLOT(openFile(QString,int)));
	connect(ErrorManager::self(), SIGNAL(changed()), m_errorDock, SLOT(updateErrors()));
	view = addToolView(m_errorDock, Qt::BottomDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_errorDock"));
	m_errorDock->setDock(view);

	action = view->toggleViewAction();
	m_menus["windows"]->addAction(action);

	/* RCS */
	m_rcsLogDock = new RCSLogDockWidgetImpl(this);
	m_logDockView = addToolView(m_rcsLogDock, Qt::BottomDockWidgetArea);
	m_logDockView->setObjectName(QString::fromUtf8("m_rcsLogDock"));
	action = m_logDockView->toggleViewAction();
	m_menus["windows"]->addAction(action);
	m_rcsLogDock->setDock(m_logDockView);

	/* Search */
	m_searchDock = new SearchDockWidgetImpl(this);
	connect(m_searchDock, SIGNAL(open(QString,int)), EditorManager::self(), SLOT(openFile(QString,int)));
	view = addToolView(m_searchDock, Qt::BottomDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_searchDock"));
	action = view->toggleViewAction();
	m_menus["windows"]->addAction(action);
	m_searchDock->setDock(view);

	/* Progress */
	m_progressDock = new XinxJobProgressDock(this);
	view = addToolView(m_progressDock, Qt::BottomDockWidgetArea);
	view->setObjectName(QString::fromUtf8("m_progressDock"));
	action = view->toggleViewAction();
	m_menus["windows"]->addAction(action);

	// Load dock from plugins and assign automatic shortcut
	int dockShortcut = 4;
	foreach(XinxPluginElement * pluginElement, XinxPluginsLoader::self()->plugins())
	{
		IDockPlugin * dockPlugin = qobject_cast<IDockPlugin*>(pluginElement->plugin());
		if (pluginElement->isActivated() && dockPlugin)
		{
			QList<XinxDockWidget*> docks = dockPlugin->createDocksWidget(this);
			foreach(XinxDockWidget * dock, docks)
			{
				dock->setShortcut(QString("Alt+%1").arg(dockShortcut++));
				view = addToolView(dock, Qt::RightDockWidgetArea);
				dock->setDock(view);
				action = view->toggleViewAction();
				action->setShortcut(dock->getShortcut());
				m_menus["windows"]->addAction(action);
				connect(view, SIGNAL(visibilityChanged(bool)), dock, SLOT(setFocus()));
			}
		}
	}
}

void MainformImpl::createStatusBar()
{
	m_statusBar = new QStatusBar(this);
	setStatusBar(m_statusBar);

	m_jobProgressBar = new XinxJobProgressBar(this);
	m_jobProgressBar->setMaximumWidth(150);
	m_jobProgressBar->hide();

	m_codecLabel = new QLabel(tr("Unknown"));
	m_lineFeedLabel = new QLabel(tr("Unknown"));
	m_editorPosition = new QLabel("000x000");
	statusBar()->addPermanentWidget(m_jobProgressBar);
	statusBar()->addPermanentWidget(m_codecLabel);
	statusBar()->addPermanentWidget(m_lineFeedLabel);
	statusBar()->addPermanentWidget(m_editorPosition);
	statusBar()->showMessage(tr("Ready"), 2000);
}

void MainformImpl::createTools()
{
	connect(ScriptManager::self(), SIGNAL(changed()), this, SLOT(updateToolsMenu()));
	updateToolsMenu();
}

void MainformImpl::callSnipetAction(int snipetId)
{
	Q_ASSERT(EditorManager::self()->currentEditor() != NULL);

	// FIXME : Merge with xinxcodeedit
	// FIXME : If no editor, if script return false ... disable action. maybe manage a setEditor on the menu.
	if (qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor()))
	{
		QString result;
		if (SnipetManager::self()->callSnipet(snipetId, &result, this))
		{
			XinxCodeEdit * editor = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor())->textEdit();
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
}

void MainformImpl::currentTabChanged(int index)
{
	AbstractEditor * editor = EditorManager::self()->editor(index);
	ScriptManager::self()->setCurrentEditeur(editor);
	m_snipetsDock->setEditor(editor);
	updateEditorInformations();
}

void MainformImpl::updateEditorInformations()
{
	AbstractEditor * editor = EditorManager::self()->currentEditor();
	if (qobject_cast<TextFileEditor*>(editor))      // This is a text file editor. So we have codec, ...
	{
		TextFileEditor * textFileEditor = qobject_cast<TextFileEditor*>(editor);
		m_codecLabel->setText(textFileEditor->codecName());

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

void MainformImpl::updateConfigElement()
{
	if (m_alwaysShowRunDialog->isChecked() != XINXConfig::self()->config().snipets.alwaysShowDialog)
		m_alwaysShowRunDialog->setChecked(XINXConfig::self()->config().snipets.alwaysShowDialog);
}

void MainformImpl::sPrintedFile(const QString & filename)
{
	Q_UNUSED(filename);

	statusBar()->showMessage(tr("Printed"), 2000);
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
	connect(m_findDialog, SIGNAL(findInFiles(QStringList, QStringList, QString, QString, AbstractEditor::SearchOptions)), this, SLOT(findInFiles(QStringList, QStringList, QString, QString, AbstractEditor::SearchOptions)));

	m_replaceNextDlg   = new QMessageBox(QMessageBox::Question, tr("Replace text"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel, this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	m_replaceNextDlg->setWindowModality(Qt::NonModal);
}

void MainformImpl::findFirst(const QString & chaine, const QString & dest, const AbstractEditor::SearchOptions & options)
{
	Q_ASSERT(EditorManager::self()->currentEditor());

	AbstractEditor * editor = EditorManager::self()->currentEditor();

	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;
	m_nbFindedText      = 0;
	m_searchInverse     = false;

	editor->initSearch(m_findOptions);
	findNext();
}

void MainformImpl::findInFiles(const QStringList& directories, const QStringList & filter, const QString& from, const QString& to, const AbstractEditor::SearchOptions& options)
{
	m_searchAct->setEnabled(false);
	m_searchNextAct->setEnabled(false);
	m_searchPreviousAct->setEnabled(false);
	m_replaceAct->setEnabled(false);
	m_searchDock->init();
	m_searchDock->dock()->setVisible(true);

	SearchFileThread * threadSearch = new SearchFileThread();
	connect(m_searchDock, SIGNAL(abort()), threadSearch, SLOT(abort()), Qt::DirectConnection);
	connect(threadSearch, SIGNAL(find(QString,QString,int)), m_searchDock, SLOT(find(QString,QString,int)), Qt::BlockingQueuedConnection);
	connect(threadSearch, SIGNAL(end(bool)), this, SLOT(findEnd(bool)));

	threadSearch->setFilter(filter);
	threadSearch->setPath(directories);
	threadSearch->setSearchString(from, to, options);
	threadSearch->search();
}

void MainformImpl::findEnd(bool abort)
{
	m_searchAct->setEnabled(true);
	m_searchNextAct->setEnabled(true);
	m_searchPreviousAct->setEnabled(true);
	m_replaceAct->setEnabled(true);

	m_searchDock->end();

	if (abort)
	{
		QMessageBox::information(this, tr("Search End"), tr("Search have been aborted"));
	}
	else
	{
		QMessageBox::information(this, tr("Search End"), tr("All string are found"));
	}
}

void MainformImpl::findNext()
{
	Q_ASSERT(EditorManager::self()->currentEditor());

	AbstractEditor * editor = EditorManager::self()->currentEditor();

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
										 tr("%Ln occurence(s) of '%1' replaced.", "", m_nbFindedText).arg(m_findExpression),
										 QMessageBox::Ok);
			}
			else
			{
				const QString warningString = m_replaceExpression.isNull() ?
					tr("%Ln occurence(s) of '%1' found. Return to the beginning of the document ?", "", m_nbFindedText) :
					tr("%Ln occurence(s) of '%1' replaced. Return to the beginning of the document ?", "", m_nbFindedText);

				QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Search/Replace"), warningString.arg(m_findExpression), QMessageBox::Yes | QMessageBox::No);

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
	if (qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor()))
	{
		XinxCodeEdit * textEdit = qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor())->textEdit();
		if (! textEdit->textCursor().selectedText().isEmpty())
		{
			m_findDialog->setText(textEdit->textCursor().selectedText());
		}
	}
	m_findDialog->initialize(EditorManager::self()->editorsCount() > 0);
	m_findDialog->setReplace(false);
	m_findDialog->exec();
}

void MainformImpl::replace()
{
	Q_ASSERT(EditorManager::self()->currentEditor());
	Q_ASSERT(qobject_cast<TextFileEditor*>(EditorManager::self()->currentEditor()));

	XinxCodeEdit * textEdit = static_cast<TextFileEditor*>(EditorManager::self()->currentEditor())->textEdit();
	if (! textEdit->textCursor().selectedText().isEmpty())
	{
		m_findDialog->setText(textEdit->textCursor().selectedText());
	}
	m_findDialog->initialize();
	m_findDialog->setReplace(true);
	m_findDialog->exec();
}

void MainformImpl::updateActions()
{
	m_searchNextAct->setEnabled(EditorManager::self()->editorsCount());
	m_replaceAct->setEnabled(EditorManager::self()->editorsCount());
	m_searchPreviousAct->setEnabled(EditorManager::self()->editorsCount());
}

void MainformImpl::updateTitle()
{
	QString title = "XINX";

	if (EditorManager::self()->currentEditor() && !EditorManager::self()->currentEditor()->getLongTitle().isEmpty())
	{
		title = "[" + EditorManager::self()->currentEditor()->getLongTitle() + "] - " + title;
	}

	XinxProject::ProjectPtr selectedProject = XinxProject::Manager::self()->selectedProject().toStrongRef();

	if (selectedProject)
	{
		title = selectedProject->projectName() + " - " + title;
	}

	setWindowTitle(title);
}

void MainformImpl::setEditorPosition(const QString & text)
{
	m_editorPosition->setText(text);
}

void MainformImpl::closeEvent(QCloseEvent *event)
{
	emit aboutToClose();
	storeWindowSettings(); // Store before the project is closed

	XinxSession::SessionManager::self()->saveSession();
	XinxSession::SessionManager::self()->setApplicationStopping(true);

	if (! EditorManager::self()->closeAllFile())
	{
		event->ignore();
		return;
	}

	XinxProject::Manager::self()->closeAllProject();

	event->accept();
}

void MainformImpl::sOpenedFile(const QString & filename)
{
	Q_UNUSED(filename);

	statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainformImpl::sSavedFile(const QString& filename)
{
	Q_UNUSED(filename);

	statusBar()->showMessage(tr("File saved"), 2000);
}

void MainformImpl::sClosedFile(const QString& filename)
{
	Q_UNUSED(filename);

	statusBar()->showMessage(tr("File closed"), 2000);
}

