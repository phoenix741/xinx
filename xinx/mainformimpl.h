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

#ifndef MAINFORMIMPL_H
#define MAINFORMIMPL_H
#pragma once

// Xinx header
#include <core/xinxcore.h>
#include <core/xinxconfig.h>
#include <editors/abstracteditor.h>
#include "tabeditor.h"
#include <dmainwindow.h>

// Qt header
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QProgressBar>

class FileContentDockWidget;
class ProjectDirectoryDockWidget;
class LogDockWidget;
class XmlPresentationDockWidget;
class SnipetDockWidget;
class ReplaceDialogImpl;

class QLabel;
class QMessageBox;
class QAction;
class QToolButton;
class SnipetMenu;

class MainformImpl : public DMainWindow
{
	Q_OBJECT
public:
	MainformImpl(QWidget * parent = 0);
	virtual ~MainformImpl();
public slots:
	void openFile(const QString & filename);
	void saveAllFile();
	void closeFile();
	bool closeAllFile();

	void openProject(const QString & filename);
	void saveProject(bool withSessionData);
	void closeProjectNoSessionData();
	void closeProjectWithSessionData();

	void openWelcomDialog();
signals:
	void aboutToClose();
protected:
	virtual void closeEvent(QCloseEvent *event);
	virtual void timerEvent(QTimerEvent * event);
private:
	// Creation
	void createMainForm();

	void createTabEditorButton();
	void createMenus();
	void createNewSubMenu();
	void createPluginsActions();
	void createStatusBar();
	void createDockWidget();
	void createActions();
	void createFindReplace();
	void createTools();
	void createRCS();
	void registerTypes();
	void connectSignals();

	// Editor
	QString m_lastFileName;
	QString m_lastProjectOpenedPlace;
	QStringList m_fileToAdd;

	bool fileEditorMayBeSave(QList<AbstractEditor*> editors);
	bool fileEditorMayBeSave(int index);

	void fileEditorSave(AbstractEditor *, bool saveAs = false);
	void fileEditorSaveAs(AbstractEditor *);
	void fileEditorRefreshFile(AbstractEditor *);

	void fileEditorSave(int index, bool saveAs = false);
	void fileEditorSaveAs(int index);
	void fileEditorClose(int index);
	void fileEditorRefreshFile(int index);

	// Project
	bool closeProject(bool session, bool showWelcome = true);

	// Settings
	void readWindowSettings();
	void storeWindowSettings();

	// Dock
	FileContentDockWidget * m_contentDock;
	ProjectDirectoryDockWidget * m_projectDock;
	LogDockWidget * m_logDock;
	SnipetDockWidget * m_snipetsDock;
	QDockWidget * m_logDockView;

	// RCS
	bool m_rcsVisible;
	QTimer * m_timer;

	// Status bar
	QLabel * m_editorPosition;
	QLabel * m_threadCount;
	QLabel * m_codecLabel;
	QLabel * m_lineFeedLabel;

	QProgressBar * m_indexingBar;

	// Status bar
	QStatusBar * m_statusBar;

	// Menus
	QMenuBar * m_menuBar;
	QHash<QString, QMenu*> m_menus;
	QHash<QString, QToolBar*> m_toolBars;
	QMenu * m_newMenu, * m_scriptMenu;
	SnipetMenu *m_snipetMenu;

	// Plugins Actions
	QList<QAction*> m_pluginsAction;

	// Recent action
	QAction * m_recentProjectActs[ MAXRECENTFILES ];
	QAction * m_recentSeparator;
	QAction * m_recentFileActs[ MAXRECENTFILES ];
	QAction * m_recentFileSeparator;

	void setupRecentMenu(QMenu * menu, QAction * & seperator, QAction * recentActions[ MAXRECENTFILES ]);

	// Scripts
	QList<QAction*> m_scriptActs;

	// Find/Replace
	ReplaceDialogImpl * m_findDialog;
	QMessageBox * m_replaceNextDlg;
	bool m_yesToAllReplace, m_searchInverse;
	int m_nbFindedText;
	QString m_findExpression, m_replaceExpression;
	AbstractEditor::SearchOptions m_findOptions;

	// Customize
	QToolButton * m_closeTabBtn;

	// Actions
	QAction *m_newAct, *m_openAct, *m_saveAct, *m_saveAsAct, *m_saveAllAct, *m_printAct, *m_closeAct, *m_closeAllAct,
	*m_exitAct, *m_recentFileAct, *m_refreshFileAct, *m_copyFileNameAct, *m_copyPathAct;
	QAction *m_undoAct, *m_redoAct, *m_cutAct, *m_copyAct, *m_pasteAct, *m_selectAllAct, *m_duplicateLineAct, *m_moveUpLineAct,
	*m_moveDownLineAct, *m_upperTextAct, *m_lowerTextAct, *m_commentLineAct, *m_uncommentLineAct, *m_completeAct, *m_indentAct,
	*m_unindentAct, *m_prettyPrintAct, *m_highlightWord, *m_showSpaceAndTabAct;
	QAction *m_searchAct, *m_searchNextAct, *m_replaceAct, *m_searchPreviousAct;
	QAction *m_newProjectAct, *m_openProjectAct, *m_saveProjectAct, *m_closeProjectNoSessionAct, *m_projectPropertyAct,
	*m_closeProjectWithSessionAct,  *m_recentProjectAct, *m_closeProjectAct;
	QAction *m_nextBookmarkAct, *m_bookmarkAct, *m_previousBookmarkAct, *m_clearAllBookmarkAct;
	QAction *m_toggledFlatView;
	QAction *m_nextTabAct, *m_previousTabAct;
	QAction *m_aboutAct, *m_aboutQtAct;
	QAction *m_customApplicationAct, *m_refreshScripts, *m_alwaysShowRunDialog;

	// Tab Editors
	TabEditor * m_tabEditors;
private slots:
	// Actions
	void updateActions();
	void updateConfigElement();
	void updateTitle();
	void updateEditorInformations();
	void updateRecentFiles();
	void updateRecentProjects();
	void updateRCS();

	// File
	void newFile();
	void openFile();
	void openFile(const QString & name, int line);
	void refreshFile();
	void saveFile();
	void saveAsFile();
	void printFile();
	void currentCloseFile();

	// Recent action
	void openRecentProject();
	void openRecentFile();

	// Edit
	void copyFileName();
	void copyPath();
	void updateSpaceAndTab();

	// Search
	void findEnd();
	void findInFiles(const QString & directory, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options);
	void findFirst(const QString & chaine, const QString & dest, const AbstractEditor::SearchOptions & options);
	void findNext();
	void findPrevious();
	void find();
	void replace();

	// Project
	void newProject();
	void openProject();
	void projectProperty();
	void closeProject();
	void rcsLogTerminated();

	// Log timer
	void logStart();
	void logTimeout();

	// Windows
	void nextTab();
	void previousTab();
	void currentTabChanged(int);
	void setEditorPosition(int line, int column);
	void changeShowSnipetDialogAction();

	// Tools
	void callSnipetAction(int snipetId);
	void callScriptAction();
	void updateToolsMenu();
	void customize();

	// About
	void about();
};

#endif
