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

#ifndef MAINFORMIMPL_H
#define MAINFORMIMPL_H

// Xinx header
#include "ui_mainform.h"
#include <xinxcore.h>
#include <abstracteditor.h>
#include <xinxconfig.h>

// Qt header
#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QHash>

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

class MainformImpl : public QMainWindow, public Ui::MainForm {
	Q_OBJECT
public:
	MainformImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	virtual ~MainformImpl();
public slots:
	// DBUS Function
	void newFile( const QString &filename );
	void newTemplate( const QString &name, const QString &category, const QString &description, const QString &text, const QStringList &arguments = QStringList(), const QString &key = QString(), const QString &type = QString() );
	void openFile( const QString & filename );
	void saveFileAs( const QString & filename = QString() );
	void saveAllFile();
	void closeFile();
	bool closeAllFile();

	void newProject( const QString &name, const QString &path, bool isDerivated, const QString &prefix, const QString &filename );
	void openProject( const QString & filename );
	void saveProject( bool withSessionData );
	void closeProjectNoSessionData();
	void closeProjectWithSessionData();

	void updateFromVersionManager( const QStringList & list = QStringList() );
	void commitToVersionManager( const QStringList & list = QStringList() );
	void addFilesToVersionManager( const QStringList & list );
	void removeFilesFromVersionManager( const QStringList & list );
signals:
	void aboutToClose();
protected:
	void closeEvent( QCloseEvent *event );
private:
	// Creation
	void createTabEditorButton();
	void createShortcut();
	void createSubMenu();
	void createNewSubMenu();
	void createStatusBar();
	void createDockWidget();
	void createActions();
	void createFindReplace();
	void connectDbus();
	void createTools();
	void registerTypes();

	// Editor
	QString m_lastProjectOpenedPlace;
	QStringList m_fileToAdd;

	bool fileEditorMayBeSave( int index );
	void fileEditorSave( int index );
	void fileEditorSaveAs( int index );
	void fileEditorClose( int index );
	void fileEditorRefreshFile( int index );

	// Project
	bool closeProject( bool session );

	// Settings
	void readWindowSettings();
	void storeWindowSettings();

	// Dock
	FileContentDockWidget * m_contentDock;
	ProjectDirectoryDockWidget * m_projectDock;
	LogDockWidget * m_logDock;
	XmlPresentationDockWidget * m_xmlpresentationdock;
	SnipetDockWidget * m_snipetsDock;

	// RCS
	bool m_rcsExecute, m_rcsVisible;
	QString m_headContent, m_compareFileName;
	QTimer * m_timer;

	// Label text
	QLabel * m_editorPosition;
	QLabel * m_threadCount;
	QLabel * m_codecLabel;
	QLabel * m_lineFeedLabel;

	// Menu
	QMenu * m_newMenu;

	// Recent action
	QAction * m_recentProjectActs[ MAXRECENTFILES ];
	QAction * m_recentSeparator;
	QAction * m_recentFileActs[ MAXRECENTFILES ];
	QAction * m_recentFileSeparator;

	void setupRecentMenu( QMenu * menu, QAction * & seperator, QAction * recentActions[ MAXRECENTFILES ] );

	// Snipet
	QHash<QString,QAction*> m_snipetCategoryActs;
	QList<QAction*> m_snipetActs;
	QList<QAction*> m_scriptActs;

	// Find/Replace
	ReplaceDialogImpl * m_findDialog;
	QMessageBox * m_replaceNextDlg;
	bool m_yesToAllReplace, m_searchInverse;
	int m_nbFindedText;
	QString m_findExpression, m_replaceExpression;
	AbstractEditor::SearchOptions m_findOptions;

	// Customize
	AppSettings::struct_extentions extentionOfFileName( const QString & name );
	QToolButton * m_closeTabBtn;
private slots:
	// Actions
	void updateActions();
	void updateConfigElement();
	void updateTitle();
	void updateEditorInformations();
	void updateRecentFiles();
	void updateRecentProjects();

	// File
	void newFile();
	void openFile();
	void openFile( const QString & name, int line );
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
	void findInFiles( const QString & directory, const QString & from, const QString & to, const AbstractEditor::SearchOptions & options );
	void findFirst( const QString & chaine, const QString & dest, const AbstractEditor::SearchOptions & options );
	void findNext();
	void findPrevious();
	void find();
	void replace();

	// Project
	void newProject();
	void openProject();
	void projectProperty();
	void closeProject();
	void globalUpdateFromVersionManager();
	void globalCommitToVersionManager();
	void selectedUpdateFromVersionManager();
	void selectedCommitToVersionManager();
	void selectedAddToVersionManager();
	void selectedRemoveFromVersionManager();
	void selectedCompareWithVersionManager();
	void selectedCompareWithStd();
	void selectedCompare();
	void rcsLogTerminated();

	// Log timer
	void logTimeout();

	// Windows
	void nextTab();
	void previousTab();
	void currentTabChanged(int);
	void setEditorPosition( int line, int column );
	void setThreadCountChange();

	// Tools
	void newTemplate();
	void callSnipetMenu();
	void callScriptAction();
	void updateToolsMenu();
	void customize();

	// About
	void about();
};

#endif
