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

#ifndef MAINFORMIMPL_H
#define MAINFORMIMPL_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>
#include <core/xinxconfig.h>
#include <editors/abstracteditor.h>
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

class XinxJobProgressDock;
namespace ContentView3
{
class DockWidget;
};

class ProjectDirectoryWidgetImpl;
class ErrorDockWidgetImpl;
class SearchDockWidgetImpl;
class RCSLogDockWidgetImpl;
class XmlPresentationDockWidget;
class EditorTabDockWidget;
class SnipetDockWidget;
class ReplaceDialogImpl;
class XinxJobProgressBar;

class QLabel;
class QMessageBox;
class QAction;
class QToolButton;
class SnipetMenu;

class LIBEXPORT MainformImpl : public DMainWindow
{
	Q_OBJECT
public:
	MainformImpl(QWidget * parent = 0);
	virtual ~MainformImpl();

protected:
	virtual void closeEvent(QCloseEvent *event);
private:
	// Creation
	void createMainForm();

	void createMenus();
	void createNewSubMenu();
	void createStatusBar();
	void createDockWidget();
	void createActions();
	void createFindReplace();
	void createTools();
	void connectSignals();

	// Settings
	void readWindowSettings();
	void storeWindowSettings();

	// Dock
	ContentView3::DockWidget * m_contentDock;
	ProjectDirectoryWidgetImpl * m_projectDock;
	EditorTabDockWidget * m_editorTabDock;
	ErrorDockWidgetImpl * m_errorDock;
	RCSLogDockWidgetImpl * m_rcsLogDock;
	SearchDockWidgetImpl * m_searchDock;
	SnipetDockWidget * m_snipetsDock;
	DToolView * m_logDockView;
	XinxJobProgressDock * m_progressDock;

	// Status bar
	QLabel * m_editorPosition;
	QLabel * m_codecLabel;
	QLabel * m_lineFeedLabel;

	XinxJobProgressBar * m_jobProgressBar;

	// Status bar
	QStatusBar * m_statusBar;

	// Menus
	QMenuBar * m_menuBar;
	QHash<QString, QMenu*> m_menus;
	QHash<QString, QToolBar*> m_toolBars;
	QMenu * m_newMenu, * m_scriptMenu;
	SnipetMenu *m_snipetMenu;

	// Scripts
	QList<QAction*> m_scriptActs;

	// Find/Replace
	ReplaceDialogImpl * m_findDialog;
	QMessageBox * m_replaceNextDlg;
	bool m_yesToAllReplace, m_searchInverse;
	int m_nbFindedText;
	QString m_findExpression, m_replaceExpression;
	AbstractEditor::SearchOptions m_findOptions;

	// Actions
	QAction *m_exitAct;
	QAction *m_searchAct, *m_searchNextAct, *m_replaceAct, *m_searchPreviousAct;
	QAction *m_aboutAct, *m_aboutQtAct, *m_createTicketAct;
	QAction *m_customApplicationAct, *m_refreshScripts, *m_alwaysShowRunDialog;
signals:
	void aboutToClose();

private slots:
	// EditorManager Slot
	void sOpenedFile(const QString & filename);
	void sPrintedFile(const QString & filename);
	void sSavedFile(const QString & filename);
	void sClosedFile(const QString & filename);

	// Actions
	void updateActions();
	void updateConfigElement();
	void updateTitle();
	void updateEditorInformations();
	void createPluginsActions();

	// Search
	void findEnd(bool abort);
	void findInFiles(const QStringList & directories, const QStringList & filter, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options);
	void findFirst(const QString & chaine, const QString & dest, const AbstractEditor::SearchOptions & options);
	void findNext();
	void findPrevious();
	void find();
	void replace();

	// Windows
	void currentTabChanged(int);
	void setEditorPosition(const QString & text);
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
