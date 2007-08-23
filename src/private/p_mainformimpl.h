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
#include "../globals.h"
#include "../replacedialogimpl.h"
#include "../rcslogdialogimpl.h"

// Qt header
#include <QObject>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QHash>
#include <QTextCursor>

class PrivateMainformImpl : public QObject {
	Q_OBJECT
public:
	PrivateMainformImpl( MainformImpl * parent );
	~PrivateMainformImpl();
	
	// Creation
	void createShortcut();
	void createTabEditorButton();
	void createSubMenu();
	void createStatusBar();
	void createDockWidget();
	void createActions();
	void createFindReplace();
	void createDialogs();

	// Actions
	void updateActions();
	
	// Editor
	QString m_lastProjectOpenedPlace, m_lastPlace;
	
	QString fileEditorCheckPathName( const QString & pathname );
	QString getUserPathName( const QString & pathname, const QString & suffix = QString() );
	QString fileEditorStandardBackup( const QString & oldname, const QString & newname );
	bool fileEditorMayBeSave( int index );
	void fileEditorSave( int index );
	void fileEditorSaveAs( int index );
	void fileEditorClose( int index );
	void fileEditorRefreshFile( int index );
	
	// Project
	void closeProject( bool session );
	void updateTitle();
	
	// Settings
	void readWindowSettings();
	void storeWindowSettings();
	
	// Dock
	FileContentDockWidget * m_contentDock;
	ProjectDirectoryDockWidget * m_projectDock;
	
	// Label text
	QLabel * m_editorPosition;

	// Recent action
	QAction * m_recentProjectActs[ MAXRECENTFILES ]; 
	QAction * m_recentSeparator;
	QAction * m_recentFileActs[ MAXRECENTFILES ]; 
	QAction * m_recentFileSeparator;

	void setupRecentMenu( QMenu * menu, QAction * & seperator, QAction * recentActions[ MAXRECENTFILES ] );
	void updateRecentFiles();
	void updateRecentProjects();
	
	// Snipet
	QHash<QString,QAction*> m_snipetCategoryActs;
	QList<QAction*> m_snipetActs;
	
	// Find/Replace
	ReplaceDialogImpl * m_findDialog;
	QString m_findExpression, m_replaceExpression;
	struct ReplaceDialogImpl::FindOptions m_findOptions;
	bool m_yesToAllReplace, m_searchInverse;
	QTextCursor m_cursorStart, m_cursorEnd;
	int m_nbFindedText;
	
	// RCS
	RCSLogDialogImpl * m_rcslogDialog;
	
public slots:
	// File
	void openFile();
	void openFile( const QString & name, int line );
	void refreshFile();
	void saveFile();
	void saveAsFile();
	void printFile();

	// Recent action
	void openRecentProject();
	void openRecentFile();
	
	// Search
	void findFirst( const QString & chaine, const QString & dest, const struct ReplaceDialogImpl::FindOptions & options );
	void find();
	void findNext();
	void findPrevious();
	void replace();

	// Project
	void openProject();
	void projectProperty();
	void globalUpdateFromVersionManager();
	void globalCommitToVersionManager();
	void selectedUpdateFromVersionManager();
	void selectedCommitToVersionManager();
	void selectedAddToVersionManager();
	void selectedRemoveFromVersionManager();
	void rcsLogTerminated();
	void webServicesReponse( QHash<QString,QString> query, QHash<QString,QString> response, QString errorCode, QString errorString );

	// Windows
	void nextTab();
	void previousTab();
	
	// Tools
	void customize();

	// About
	void about();
private:	
	MainformImpl * m_parent;
};

#endif // __P_MAINFORMIMPL_H__
