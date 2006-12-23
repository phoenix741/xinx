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

class QAction;
class QMenu;
class QTextEdit;
class QTabWidget;
class QDockWidget;
class Editor;
class TabEditor;
class ObjectsView;
class CplNodeList;

class XMlVisualStudio : public QMainWindow {
  Q_OBJECT
public:
  XMlVisualStudio();

protected:
  void closeEvent(QCloseEvent *event);
private slots:
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
private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void createDockWindows(); 
  void readSettings();
  void writeSettings();

  bool maybeSave(int index);
  bool saveEditor(int index);
  bool saveEditorAs(int index);
  
  TabEditor * m_tabEditors;
  ObjectsView * m_javaObjects;

  QMenu * m_fileMenu;
  QMenu * m_projectMenu;
  QMenu * m_editMenu;
  QMenu * m_searchMenu;
  QMenu * m_windowsMenu;
  QMenu * m_helpMenu;
  
  QToolBar * m_fileToolBar;
  QToolBar * m_editToolBar;
  
  QDockWidget * m_xpathDock;
  QDockWidget * m_xslContentDock;
  QDockWidget * m_configurationDock;
  
  QAction * m_newAct;
  
  QAction * m_openAct;
  QAction * m_saveAct;
  QAction * m_saveAsAct;
  QAction * m_saveAllAct;
  
  QAction * m_printAct;
  
  QAction * m_closeAct;
  QAction * m_closeAllAct;
  
  QAction * m_exitAct;
  
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
  
  QAction * m_searchAct;
  QAction * m_searchNextAct;
  QAction * m_searchPreviousAct;
  QAction * m_replaceAct;
  
  QAction * m_javaViewObjectListAct;
  
  QAction * m_aboutAct;
  QAction * m_aboutQtAct;
};
 
#endif

