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

#include <qmainwindow.h>
#include <QMainWindow>
#include "finddialog.h"

/* Declaration of used object */
class QAction;
class QMenu;
class QTextEdit;
class QTabWidget;
class QDockWidget;
class Editor;
class TabEditor;
class ObjectsView;
class CplNodeList;
class XSLItemModel;
class QModelIndex;

/* Main object of application */
class XMlVisualStudio : public QMainWindow {
  Q_OBJECT
public:
  XMlVisualStudio();

  void open(const QString & filename);

protected:
  /* Window event */

  void closeEvent(QCloseEvent *event);
  
private slots:
  /* Action slots */

  void newFile();
  void open();
  bool save();
  bool saveAs();
  bool saveAll();
  
  void print();
  
  void closeTab();
  void closeAllTab();
  
  void openViewObjectList();
  
  void about();
  
  void find();
  void replace();

  /* FindDialog slots */
  
  void findNext();
  void findFirst(const QString &, const QString &, const struct FindDialog::FindOptions &);
  
  /* Editor Change */
  void slotCurrentTabChanged(int);
  
  /* XSL Content Dock Slots */
  void xslDockDoubleClicked( const QModelIndex & );
  
private:
  /* Create Window Menus, Tools, Status, Dock Bar */
  
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows(); 
  
  /* Load and save Settings */
  
  void readSettings();
  void writeSettings();
  
  /* Save an editor into a file */

  bool maybeSave(int index);
  bool saveEditor(int index);
  bool saveEditorAs(int index);
  
  /**** Object declaration ****/
  
  /* Dock Object */
  XSLItemModel * m_xslModel;

  /* Find declaration */
  QString m_findExpression, m_replaceExpression;
  struct FindDialog::FindOptions m_findOptions;
  bool m_yesToAllReplace;

  /* Generix Object */
  ObjectsView * m_javaObjects;
  
  /* Interfaces */
  TabEditor * m_tabEditors;
  FindDialog * m_findDialog;

  /* Menu */
  QMenu * m_fileMenu;
  QMenu * m_projectMenu;
  QMenu * m_editMenu;
  QMenu * m_searchMenu;
  QMenu * m_windowsMenu;
  QMenu * m_helpMenu;

  /* Tool bar */  
  QToolBar * m_fileToolBar;
  QToolBar * m_editToolBar;
  
  /* Dock */
  QDockWidget * m_xpathDock;
  QDockWidget * m_xslContentDock;
  QDockWidget * m_configurationDock;
  
  /* Actions */
  
  /* File */
  QAction * m_newAct;
  
  QAction * m_openAct;
  QAction * m_saveAct;
  QAction * m_saveAsAct;
  QAction * m_saveAllAct;
  
  QAction * m_printAct;
  
  QAction * m_closeAct;
  QAction * m_closeAllAct;
  
  QAction * m_exitAct;
  
  /* Edit */
  QAction * m_undoAct;
  QAction * m_redoAct;

  QAction * m_cutAct;
  QAction * m_copyAct;
  QAction * m_pasteAct;

  QAction * m_selectAllAct;
  QAction * m_duplicateLineAct;

  QAction * m_moveUpLineAct;
  QAction * m_moveDownLineAct;

  QAction * m_upperTextAct;
  QAction * m_lowerTextAct;

  QAction * m_commentLineAct;
  QAction * m_uncommentLineAct;

  QAction * m_completeAct;
  
  /* Search */
  
  QAction * m_searchAct;
  QAction * m_searchNextAct;
  QAction * m_replaceAct;
  
  /* Project */
  
  QAction * m_javaViewObjectListAct;
  
  /* About */
  QAction * m_aboutAct;
  QAction * m_aboutQtAct;
};
 
#endif

