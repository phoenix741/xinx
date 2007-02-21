/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef XMLVISUALSTUDIO_H
#define XMLVISUALSTUDIO_H
//
#include "ui_mainform.h"
#include "finddialog.h"
//

#define MAXRECENTFILES 5

class XSLItemModel;
class QSortFilterProxyModel;
class ObjectsView;
class XSLProject;
class QSettings;

class XMLVisualStudio : public QMainWindow, public Ui::MainForm {
	Q_OBJECT
public:
	XMLVisualStudio( QWidget * parent = 0, Qt::WFlags f = 0 );
	
	void updateActions();
	void updateRecentFiles();

public slots:
	/* Open file */
	void open( const QString & filename );
protected: 
	/* Window event */

	void closeEvent( QCloseEvent *event );
private slots:
	/* FindDialog slots */
	void findFirst(const QString &, const QString &, const struct FindDialog::FindOptions &);

	/* Editor Change */
	void slotCurrentTabChanged(int);
	void slotModelDeleted();
	void slotModelCreated();
	
	/* Usefull for recent project */
	
	void openRecentProject();
	
	/* Auto slots */
	
	void on_m_newAct_triggered();
	void on_m_openAct_triggered();
	void on_m_saveAct_triggered();
	void on_m_saveAsAct_triggered();
	void on_m_saveAllAct_triggered();
	void on_m_printAct_triggered();
	void on_m_closeAct_triggered();
	void on_m_closeAllAct_triggered();
	void on_m_searchAct_triggered();
	void on_m_searchNextAct_triggered();
	void on_m_replaceAct_triggered();
	void on_m_javaViewObjectListAct_triggered();
	void on_m_newProjectAct_triggered();
	void on_m_openProjectAct_triggered();
	void on_m_saveProjectAct_triggered();
	void on_m_closeProjectAct_triggered();
	void on_m_projectPropertyAct_triggered();
	void on_m_aboutAct_triggered();
	void on_m_xslContentTreeView_doubleClicked(QModelIndex index);
	void on_m_nextTabAct_triggered();
	void on_m_previousTabAct_triggered();
private:
	/* Create Window Menus, Tools, Status, Dock Bar */
  
	void createActions();
	void createToolBars();
	void createStatusBar();
	void createDockWindows(); 
  
	/* Load and save Settings */
  
	void readSettings();
	void writeSettings();
  
	/* Save an editor into a file */

	bool maybeSave(int index);
	void saveEditor(int index);
	void saveEditorAs(int index);
	
	/* Usefull for Recent project */
	
	void setCurrentProject( const QString & filename );
	void openProject( const QString & filename );
	void closeProject( bool closeAll );
  
	/**** Object declaration ****/
	
	/* Open/Save Dialog */	
	QString m_lastPlace;
	
	/* Settings */
	QSettings * m_settings;
  
	/* Dock Object */
	QAbstractItemModel * m_xslModel;
	QSortFilterProxyModel * m_sortXslModel;

	/* Find declaration */
	QString m_findExpression, m_replaceExpression;
	struct FindDialog::FindOptions m_findOptions;
	bool m_yesToAllReplace;

	/* Generix Object */
	ObjectsView * m_javaObjects;
	XSLProject * m_xslProject;
  
	/* Interfaces */
	FindDialog * m_findDialog;
	QAction * m_recentProjectActs[MAXRECENTFILES];
	QAction * m_recentSeparator;
};
#endif









