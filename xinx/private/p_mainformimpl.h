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

#ifndef __P_MAINFORMIMPL_H__
#define __P_MAINFORMIMPL_H__

// Xinx header
#include "../mainformimpl.h"
#include "../filecontentdockwidget.h"
#include "../projectdirectorydockwidget.h"
#include "../xmlpresentationdockwidget.h"
#include <xinxcore.h>
#include "../replacedialogimpl.h"
#include "../logdialogimpl.h"
#include "../snipet.h"
#include "../snipetdockwidget.h"
#include <xinxconfig.h>

// Qt header
#include <QObject>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QHash>
#include <QTextCursor>
#include <QTimer>
#include <QMessageBox>

class PrivateMainformImpl : public QObject {
	Q_OBJECT
public:
	PrivateMainformImpl( MainformImpl * parent );
	~PrivateMainformImpl();

	// Creation
	void createTabEditorButton();
	void createShortcut();
	void createSubMenu();
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
public slots:
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
	void callSnipetMenu();
	void callScriptAction();
	void updateToolsMenu();
	void customize();

	// About
	void about();
private:
	MainformImpl * m_parent;
};

Q_DECLARE_METATYPE(Snipet*);

#endif // __P_MAINFORMIMPL_H__
