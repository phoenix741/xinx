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

// Qt header
#include <QObject>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QHash>

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

	// Actions
	void updateActions();
	
	// Editor
	QString m_lastPlace;
	
	QString fileEditorCheckPathName( const QString & pathname );
	QString getUserPathName( const QString & pathname, const QString & suffix = QString() );
	QString fileEditorStandardBackup( const QString & oldname, const QString & newname );
	bool fileEditorMayBeSave( int index );
	void fileEditorSave( int index );
	void fileEditorSaveAs( int index );
	void fileEditorClose( int index );
	
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
public slots:
	// File
	void openFile();
	void saveFile();
	void saveAsFile();

	// Project
	void globalUpdateFromVersionManager();
	void globalCommitToVersionManager();
	void selectedUpdateFromVersionManager();
	void selectedCommitToVersionManager();
	void selectedAddToVersionManager();
	void selectedRemoveFromVersionManager();

	// Recent action
	void openRecentProject();
	void openRecentFile();
private:	
	MainformImpl * m_parent;
};

#endif // __P_MAINFORMIMPL_H__
