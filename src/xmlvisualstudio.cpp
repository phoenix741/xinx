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

#include <QtGui>

#include "xmlvisualstudio.h"
#include "Editor.h"
#include "TabEditor.h"
#include "javaobjectfileimpl.h"
#include "ObjectView.h"

XMlVisualStudio::XMlVisualStudio() {
  setWindowTitle(tr("XML Visual Studio"));

  m_tabEditors = new TabEditor( this );
  setCentralWidget(m_tabEditors);
  
  m_javaObjects = new ObjectsView();

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  createDockWindows();

  readSettings();
}

void XMlVisualStudio::closeEvent(QCloseEvent *event) {
  for( int i = 0; i < m_tabEditors->count(); i++ ) {
	if ( ! maybeSave( i ) ) {
      event->ignore();
      return;
     }
  }
  writeSettings();
  event->accept();
}

void XMlVisualStudio::newFile() {
  m_tabEditors->newTab();
}

void XMlVisualStudio::open() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open XSL File"), QDir::currentPath(), tr("XSL Files (*.xsl *.xml)"));
  if (!fileName.isEmpty()) {
  	m_tabEditors->loadTab(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
  }
}

bool XMlVisualStudio::save() {
  return saveEditor(m_tabEditors->currentIndex());
}

bool XMlVisualStudio::saveAs() {
  return saveEditorAs(m_tabEditors->currentIndex());
}

bool XMlVisualStudio::saveAll() {
  bool result = false;
  for(int i = 0; i < m_tabEditors->count(); i++) {
    result &= saveEditor(i); 	
  }
  return result;
}

void XMlVisualStudio::print() {
  QTextDocument *document = m_tabEditors->currentEditor()->textEdit()->document();
  QPrinter printer;

  QPrintDialog *dlg = new QPrintDialog(&printer, this);
  if (dlg->exec() != QDialog::Accepted) return;

  document->print(&printer);

  statusBar()->showMessage(tr("Ready"), 2000);
}


void XMlVisualStudio::closeTab() {
  int index = m_tabEditors->currentIndex();
  if(maybeSave(index)) {
    m_tabEditors->removeTab( index );
  }
}

void XMlVisualStudio::closeAllTab() {
  for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) {
	if ( ! maybeSave( i ) ) {
      return;
     } else {
       m_tabEditors->removeTab( i );	
     }
  }
}

void XMlVisualStudio::about() {
  QMessageBox::about(this, tr("About XML Visual Studio"),
                     tr("<b>XML Visual Studio</b> manage a <i>Generix</i> XSL/XML Project."));
}

void XMlVisualStudio::openViewObjectList() {
  JavaObjectFileImpl dlg(m_javaObjects);
  dlg.exec();
}

void XMlVisualStudio::createActions() {
  m_newAct = new QAction(QIcon(":/images/filenew.png"), tr("&New"), this);
  m_newAct->setShortcut(tr("Ctrl+N"));
  m_newAct->setStatusTip(tr("Create a new file"));
  connect(m_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  m_openAct = new QAction(QIcon(":/images/fileopen.png"), tr("&Open..."), this);
  m_openAct->setShortcut(tr("Ctrl+O"));
  m_openAct->setStatusTip(tr("Open an existing file"));
  connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

  m_saveAct = new QAction(QIcon(":/images/filesave.png"), tr("&Save"), this);
  m_saveAct->setShortcut(tr("Ctrl+S"));
  m_saveAct->setStatusTip(tr("Save the document to disk"));
  connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

  m_saveAsAct = new QAction(QIcon(":/images/filesaveas.png"), tr("Save &As..."), this);
  m_saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  m_saveAllAct = new QAction(QIcon(":/images/filesaveall.png"), tr("Save A&ll..."), this);
  m_saveAllAct->setStatusTip(tr("Save all documents to disk"));
  connect(m_saveAllAct, SIGNAL(triggered()), this, SLOT(saveAll()));
  
  m_closeAct = new QAction(QIcon(":/images/fileclose.png"), tr("&Close"), this);
  m_closeAct->setShortcut(tr("Ctrl+W"));
  m_closeAct->setStatusTip(tr("Close the current document"));
  connect(m_closeAct, SIGNAL(triggered()), this, SLOT(closeTab()));

  m_printAct = new QAction(QIcon(":/images/fileprint.png"), tr("&Print"), this);
  m_printAct->setShortcut(tr("Ctrl+P"));
  m_printAct->setStatusTip(tr("Print current document"));
  connect(m_printAct, SIGNAL(triggered()), this, SLOT(print()));

  m_closeAllAct = new QAction(tr("&Close all"), this);
  m_closeAllAct->setStatusTip(tr("Close all opened document"));
  connect(m_closeAllAct, SIGNAL(triggered()), this, SLOT(closeAllTab()));

  m_exitAct = new QAction(QIcon(":/images/exit.png"), tr("E&xit"), this);
  m_exitAct->setShortcut(tr("Ctrl+Q"));
  m_exitAct->setStatusTip(tr("Exit the application"));
  connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  m_undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
  m_undoAct->setShortcut(tr("Ctrl+Z"));
  m_undoAct->setStatusTip(tr("Undo the last action"));
  connect(m_undoAct, SIGNAL(triggered()), m_tabEditors, SLOT(undo()));
  m_undoAct->setEnabled(false);
  connect(m_tabEditors, SIGNAL(undoAvailable(bool)), m_undoAct, SLOT(setEnabled(bool)));	

  m_redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
  m_redoAct->setShortcut(tr("Ctrl+Shift+Z"));
  m_redoAct->setStatusTip(tr("Redo the next action"));
  connect(m_redoAct, SIGNAL(triggered()), m_tabEditors, SLOT(redo()));
  m_redoAct->setEnabled(false);
  connect(m_tabEditors, SIGNAL(redoAvailable(bool)), m_redoAct, SLOT(setEnabled(bool)));	

  m_cutAct = new QAction(QIcon(":/images/editcut.png"), tr("Cu&t"), this);
  m_cutAct->setShortcut(tr("Ctrl+X"));
  m_cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                             "clipboard"));
  connect(m_cutAct, SIGNAL(triggered()), m_tabEditors, SLOT(cut()));
  m_cutAct->setEnabled(false);
  connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_cutAct, SLOT(setEnabled(bool)));

  m_copyAct = new QAction(QIcon(":/images/editcopy.png"), tr("&Copy"), this);
  m_copyAct->setShortcut(tr("Ctrl+C"));
  m_copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                           "clipboard"));
  connect(m_copyAct, SIGNAL(triggered()), m_tabEditors, SLOT(copy()));
  m_copyAct->setEnabled(false);
  connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_copyAct, SLOT(setEnabled(bool)));	

  m_pasteAct = new QAction(QIcon(":/images/editpaste.png"), tr("&Paste"), this);
  m_pasteAct->setShortcut(tr("Ctrl+V"));
  m_pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                            "selection"));
  connect(m_pasteAct, SIGNAL(triggered()), m_tabEditors, SLOT(paste()));
                            
  m_selectAllAct = new QAction("&Select All", this);
  m_selectAllAct->setShortcut(tr("Ctrl+A"));
  m_selectAllAct->setStatusTip(tr("Select all the text of the current editor"));  
  connect(m_selectAllAct, SIGNAL(triggered()), m_tabEditors, SLOT(selectAll()));
  
  m_duplicateLineAct = new QAction("&Duplicate current line", this);
  m_duplicateLineAct->setShortcut(tr("Ctrl+D"));
  m_duplicateLineAct->setStatusTip(tr("Duplicate the current line"));  
  connect(m_duplicateLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(duplicateCurrentLine()));
                            
  m_moveUpLineAct = new QAction("&Move line up", this);
  m_moveUpLineAct->setShortcut(tr("Ctrl+Alt+P"));
  m_moveUpLineAct->setStatusTip(tr("Move up the current line"));
  connect(m_moveUpLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineUp()));
  
  m_moveDownLineAct = new QAction("&Move line down", this);
  m_moveDownLineAct->setShortcut(tr("Ctrl+Alt+M"));
  m_moveDownLineAct->setStatusTip(tr("Move down the current line"));
  connect(m_moveDownLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineDown()));

  m_upperTextAct = new QAction("&Upper Case", this);
  connect(m_upperTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(upperSelectedText()));

  m_lowerTextAct = new QAction("&Lower Case", this);
  connect(m_lowerTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(lowerSelectedText()));

  m_commentLineAct = new QAction("&Comment", this);
  connect(m_commentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(commentSelectedText()));

  m_uncommentLineAct = new QAction("&Uncomment", this);
  connect(m_uncommentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(uncommentSelectedText()));

  m_javaViewObjectListAct = new QAction(tr("Java Object List"), this);
  m_javaViewObjectListAct->setStatusTip(tr("Change the path of the Java ViewObject. This files containts "
                                           "all fields of a non Dynamic ViewObject"));
  connect(m_javaViewObjectListAct, SIGNAL(triggered()), this, SLOT(openViewObjectList()));

  m_aboutAct = new QAction(tr("&About"), this);
  m_aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  m_aboutQtAct = new QAction(tr("About &Qt"), this);
  m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void XMlVisualStudio::createMenus() {
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_newAct);
  m_fileMenu->addAction(m_openAct);
  m_fileMenu->addAction(m_saveAct);
  m_fileMenu->addAction(m_saveAsAct);
  m_fileMenu->addAction(m_saveAllAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_closeAct);
  m_fileMenu->addAction(m_closeAllAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_printAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_exitAct);

  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_undoAct);
  m_editMenu->addAction(m_redoAct);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_cutAct);
  m_editMenu->addAction(m_copyAct);
  m_editMenu->addAction(m_pasteAct);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_selectAllAct);
  m_editMenu->addAction(m_duplicateLineAct);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_moveUpLineAct);
  m_editMenu->addAction(m_moveDownLineAct);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_upperTextAct);
  m_editMenu->addAction(m_lowerTextAct);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_commentLineAct);
  m_editMenu->addAction(m_uncommentLineAct);
  
  m_projectMenu = menuBar()->addMenu(tr("&Project"));
  m_projectMenu->addAction(m_javaViewObjectListAct);

  m_windowsMenu = menuBar()->addMenu(tr("&Windows"));

  menuBar()->addSeparator();

  m_helpMenu = menuBar()->addMenu(tr("&Help"));
  m_helpMenu->addAction(m_aboutAct);
  m_helpMenu->addAction(m_aboutQtAct);
}

void XMlVisualStudio::createToolBars() {
  m_fileToolBar = addToolBar(tr("File"));
  m_fileToolBar->addAction(m_newAct);
  m_fileToolBar->addAction(m_openAct);
  m_fileToolBar->addAction(m_saveAct);

  m_editToolBar = addToolBar(tr("Edit"));
  m_editToolBar->addAction(m_undoAct);
  m_editToolBar->addSeparator();
  m_editToolBar->addAction(m_cutAct);
  m_editToolBar->addAction(m_copyAct);
  m_editToolBar->addAction(m_pasteAct);

  QToolButton * closeTab = new QToolButton(m_tabEditors);
  closeTab->setDefaultAction(m_closeAct);
  m_tabEditors->setCornerWidget(closeTab);
}

void XMlVisualStudio::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}

void XMlVisualStudio::createDockWindows() {
  m_xpathDock = new QDockWidget(tr("XPath"), this);
  m_xpathDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  QListWidget * customerList = new QListWidget(m_xpathDock);
  m_xpathDock->setWidget(customerList);
  addDockWidget(Qt::RightDockWidgetArea, m_xpathDock);

  m_windowsMenu->addAction(m_xpathDock->toggleViewAction());
  
  m_xslContentDock = new QDockWidget(tr("XSL Content"), this);
  m_xslContentDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  customerList = new QListWidget(m_xslContentDock);
  m_xslContentDock->setWidget(customerList);
  addDockWidget(Qt::RightDockWidgetArea, m_xslContentDock);

  m_windowsMenu->addAction(m_xslContentDock->toggleViewAction()); 

  m_configurationDock = new QDockWidget(tr("Configuration"), this);
  m_configurationDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  customerList = new QListWidget(m_configurationDock);
  m_configurationDock->setWidget(customerList);
  addDockWidget(Qt::LeftDockWidgetArea, m_configurationDock);

  m_windowsMenu->addAction(m_configurationDock->toggleViewAction()); 
}

void XMlVisualStudio::readSettings() {
  QSettings settings("Generix", "XML Visual Studio");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
  
  m_javaObjects->setPath(settings.value("xmljavapath", qApp->applicationDirPath() + "/xml").toString());
  m_javaObjects->loadFiles();
  
  m_xpathDock->setVisible(settings.value("xpath/visible", true).toBool());
  m_xslContentDock->setVisible(settings.value("xslContent/visible", true).toBool());
  m_configurationDock->setVisible(settings.value("configuration/visible", true).toBool());
}

void XMlVisualStudio::writeSettings() {
  QSettings settings("Generix", "XML Visual Studio");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  settings.setValue("xmljavapath", m_javaObjects->path());

  settings.setValue("xpath/visible", ! m_xpathDock->isHidden());
  settings.setValue("xslContent/visible", ! m_xslContentDock->isHidden());
  settings.setValue("configuration/visible", ! m_configurationDock->isHidden());
}


bool XMlVisualStudio::maybeSave(int index) {
  if (m_tabEditors->editor(index)->textEdit()->document()->isModified()) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
                               tr("The document %1 has been modified.\n"
                                  "Do you want to save your changes?").arg( m_tabEditors->editor(index)->getCurrentFile() ),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
     if (ret == QMessageBox::Save)
       return saveEditor(index);
     else if (ret == QMessageBox::Cancel)
       return false;
  }
  return true;
}

bool XMlVisualStudio::saveEditor(int index) {
  bool result;
  if (m_tabEditors->editor(index)->getCurrentFile().isEmpty()) {
    result = saveAs();
  } else {
    result = m_tabEditors->editor(index)->saveFile();
  }
  m_tabEditors->updateTitle(index);

  statusBar()->showMessage(tr("File %1 saved").arg( m_tabEditors->editor(index)->getCurrentFile() ), 2000);
  return result;
}

bool XMlVisualStudio::saveEditorAs(int index) {
  bool result;
  QString fileName = QFileDialog::getSaveFileName(this);
  if (fileName.isEmpty())
    return false;

  result = m_tabEditors->editor(index)->saveFile(fileName);

  m_tabEditors->updateTitle(index);
  statusBar()->showMessage(tr("File %1 saved").arg( m_tabEditors->editor(index)->getCurrentFile() ), 2000);
  return result;
}
