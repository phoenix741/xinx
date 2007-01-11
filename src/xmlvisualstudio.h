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

class XSLItemModel;
class ObjectsView;
class XSLProject;

class XMlVisualStudio : public QMainWindow, public Ui::MaintForm {
	Q_OBJECT
public:
	XMlVisualStudio( QWidget * parent = 0, Qt::WFlags f = 0 );
	
	void open( const QString & filename );
protected: 
	/* Window event */

	void closeEvent( QCloseEvent *event );
private slots:
	/* FindDialog slots */
	void findFirst(const QString &, const QString &, const struct FindDialog::FindOptions &);

	/* Editor Change */
	void slotCurrentTabChanged(int);
	
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
	void on_m_aboutAct_triggered();
	void on_m_xslContentTreeView_doubleClicked(QModelIndex index);
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
  
	/**** Object declaration ****/
  
	/* Dock Object */
	XSLItemModel * m_xslModel;

	/* Find declaration */
	QString m_findExpression, m_replaceExpression;
	struct FindDialog::FindOptions m_findOptions;
	bool m_yesToAllReplace;

	/* Generix Object */
	ObjectsView * m_javaObjects;
	XSLProject * m_xslObject;
  
	/* Interfaces */
	FindDialog * m_findDialog;
};
#endif





