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
#include <QTextCursor>

#include "ui_mainform.h"
#include "replacedialogimpl.h"
//

#define MAXRECENTFILES 5

class XSLItemModel;
class QSortFilterProxyModel;
class ObjectsView;
class XSLProject;
class AboutDialogImpl;
class XINXFileDialog;
class QDirModel;
class QMenu;
class FileEditor;
class WebServicesModel;
class WebServices;

typedef QList<WebServices*> WebServicesList;

class XMLVisualStudio : public QMainWindow, public Ui::MainForm {
	Q_OBJECT
public:
	XMLVisualStudio( QWidget * parent = 0, Qt::WFlags f = 0 );
	
public slots:
	/* Open file */
	void open( const QString & filename );

	void updateActions();
protected: 
	/* Window event */

	void closeEvent( QCloseEvent *event );
private slots:
	void on_m_newAct_triggered();
	void on_m_openAct_triggered();
	void on_m_saveAct_triggered();
	void on_m_saveAsAct_triggered();
	void on_m_saveAllAct_triggered();
	void on_m_printAct_triggered();
	void on_m_closeAct_triggered();
	void on_m_closeAllAct_triggered();
	void on_m_aboutAct_triggered();
	void on_m_nextTabAct_triggered();
	void on_m_previousTabAct_triggered();
	void on_m_customApplicationAct_triggered();

	void on_m_searchAct_triggered();
	void on_m_searchNextAct_triggered();
	void on_m_searchPreviousAct_triggered();
	void on_m_replaceAct_triggered();

	void on_m_closeProjectAct_triggered();
	void on_m_filtreLineEdit_textChanged(QString );
	void on_m_projectDirectoryTreeView_doubleClicked(QModelIndex index);

	void on_m_refreshWebServicesListAct_triggered();
	void on_m_callWebServicesAct_triggered();
	void on_m_xslContentTreeView_doubleClicked(QModelIndex index);
private slots:
	/* Manage files opened */
	void slotCloseFile( int );
	void slotRefreshFile( int );

	void saveEditor(int index);
	void saveEditorAs(int index);

	/* Editor Change */
	void slotCurrentTabChanged(int);
	void slotModelDeleted();
	void slotModelCreated();
	
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
  
	/**** Object declaration ****/
	
	/* Open/Save Dialog */	
	QString m_lastPlace;
	
	/* Dock Object */
	QAbstractItemModel * m_xslModel;
	QSortFilterProxyModel * m_sortXslModel;

	/* About Dialog */
	AboutDialogImpl * m_aboutDialog;

	/* Generix Object */
	ObjectsView * m_javaObjects;
  
	
																				/* ***** Project Main Form Part ***** */
public slots:
	void updateRecentFiles();

private slots: /* slots definition */
	void filtreChange();
	void openRecentProject();
	void newProject();
	void openProject();
	void changeProjectProperty();
	void saveProject();

private: /* Private definition */
	/* Functions */
	void createProjectPart();
	void setupRecentProjectMenu( QMenu * menu );
	void openProject( const QString & filename );
	void setCurrentProject( const QString & filename );
	void closeProject( bool closeAll );
	
	/* Variables */	
	QString m_lastProjectOpenedPlace;

	QTimer * m_modelTimer;
	QDirModel * m_dirModel;

	XSLProject * m_xslProject;

	QAction * m_recentProjectActs[MAXRECENTFILES]; 
	QAction * m_recentSeparator;

																				/* ***** WebSerivces Form Part ***** */
private slots: /* slot definition */
	void refreshWebServicesList();
	void webServicesReponse( QString query, QString response, QString errorCode, QString errorString );
	
private: /* private definition */
	/* Functions */
	void createWebServicesPart();
	void setWebServicesView( bool enabled );
	void newWebServices( FileEditor* );
	
	/* Variables */
	WebServicesList*  m_webServices;
	WebServicesModel* m_webServicesModel;
																				/* ***** Search Form Part ***** */
private slots: /* slot definition */
	void findFirst(const QString &, const QString &, const struct ReplaceDialogImpl::FindOptions &);

private: /* private definition */

	/* Variables */
	ReplaceDialogImpl * m_findDialog;
	QString m_findExpression, m_replaceExpression;
	struct ReplaceDialogImpl::FindOptions m_findOptions;
	bool m_yesToAllReplace, m_searchInverse;
	QTextCursor m_cursorStart, m_cursorEnd;
	int m_nbFindedText;

};
#endif















