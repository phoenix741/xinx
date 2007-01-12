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

#include "uniqueapplication.h"

#include "xmlvisualstudio.h"
#include "editor.h"
#include "texteditor.h"
#include "tabeditor.h"
#include "javaobjectfileimpl.h"
#include "objectview.h"
#include "editorcompletion.h"
#include "finddialog.h"
#include "xsllistview.h"
#include "projectpropertyimpl.h"
#include "xslproject.h"

#include "xmlvisualstudio.h"

//
XMLVisualStudio::XMLVisualStudio( QWidget * parent, Qt::WFlags f) : QMainWindow(parent, f) {
	setupUi(this);

	// Detete the first Tab
	//m_tabEditors->removeTab(0);
	
	m_javaObjects = new ObjectsView();
	m_xslProject = NULL;
	completionNodeList = new CplNodeList();

	m_findDialog = new FindDialog(this);
	connect(m_findDialog, SIGNAL(find(QString, QString, FindDialog::FindOptions)), this, SLOT(findFirst(QString, QString, FindDialog::FindOptions)));

	createActions();
	createToolBars();
	createStatusBar();
	createDockWindows();

	readSettings();

	// Connection for auto open
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(hasFileToOpen(QString)), m_tabEditors, SLOT(loadTab(QString)) );
}
//

void XMLVisualStudio::on_m_newAct_triggered() {
	m_tabEditors->newTab();
	updateActions();
}

void XMLVisualStudio::on_m_openAct_triggered() {
	QString filename = QFileDialog::getOpenFileName( this, tr("Open XSL File"), QDir::currentPath(), tr("XSL Files (*.xsl *.xml)") );
	if ( !filename.isEmpty() ) {
		open( filename );
	}
	updateActions();
}

void XMLVisualStudio::on_m_saveAct_triggered() {
	saveEditor( m_tabEditors->currentIndex() );
}

void XMLVisualStudio::on_m_saveAsAct_triggered() {
	saveEditorAs( m_tabEditors->currentIndex() );
}

void XMLVisualStudio::on_m_saveAllAct_triggered() {
	for(int i = 0; i < m_tabEditors->count(); i++) {
		saveEditor(i); 	
	}
}

void XMLVisualStudio::on_m_printAct_triggered() {
	QTextDocument *document = m_tabEditors->currentEditor()->textEdit()->document();
	QPrinter printer;

	QPrintDialog *dlg = new QPrintDialog(&printer, this);
	if (dlg->exec() != QDialog::Accepted) return;

	document->print(&printer);
  
	delete dlg;

	statusBar()->showMessage(tr("Printed"), 2000);
}

void XMLVisualStudio::on_m_closeAct_triggered() {
	int index = m_tabEditors->currentIndex();
	if( maybeSave(index) ) {
		m_tabEditors->removeTab( index );
	}
	updateActions();
}

void XMLVisualStudio::on_m_closeAllAct_triggered() {
	for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) {
		if ( ! maybeSave( i ) ) {
			return;
		} else {
			m_tabEditors->removeTab( i );	
		}
	}
	updateActions();
}

void XMLVisualStudio::on_m_searchAct_triggered() {
	m_findDialog->initialize();
	m_findDialog->setReplaceChecked(false);
	m_findDialog->show();
}

void XMLVisualStudio::on_m_searchNextAct_triggered() {
	StudioTextEdit * textEdit = m_tabEditors->currentEditor()->textEdit();
	QTextDocument * document = textEdit->document();
	QTextCursor cursor(textEdit->textCursor());
	int selectionStart = cursor.selectionStart(),
	    selectionEnd = cursor.selectionEnd();

	if( m_findOptions.searchFromStart ) {
		cursor.movePosition( QTextCursor::Start, QTextCursor::MoveAnchor );
		m_findOptions.searchFromStart = false;
	} else
	if( m_findOptions.selectionOnly && ! m_findOptions.backwardSearch )
		cursor.setPosition ( selectionStart, QTextCursor::MoveAnchor );
	else
	if( m_findOptions.selectionOnly && m_findOptions.backwardSearch )
		cursor.setPosition ( selectionEnd, QTextCursor::MoveAnchor );
	else
	if( m_findOptions.backwardSearch )
		cursor.setPosition ( selectionStart, QTextCursor::MoveAnchor );

	
	QTextDocument::FindFlags flags;
	if( m_findOptions.backwardSearch ) flags ^= QTextDocument::FindBackward;
	if( m_findOptions.matchCase ) flags ^= QTextDocument::FindCaseSensitively;	
	if( m_findOptions.wholeWords ) flags ^= QTextDocument::FindWholeWords;
	
	if( m_findOptions.regularExpression ) {
		cursor = document->find( QRegExp( m_findExpression ), cursor, flags );
	} else {
		cursor = document->find( m_findExpression, cursor, flags );
	}
	
	if( cursor.isNull() || 
		( m_findOptions.selectionOnly && ( ( !m_findOptions.backwardSearch && cursor.position() > selectionEnd ) || 
			( m_findOptions.backwardSearch && cursor.position() < selectionStart ) ) ) ) 
		QMessageBox::warning(this, tr("Application"), tr("%1 not found").arg( m_findExpression ), QMessageBox::Ok);
	else {
		textEdit->setTextCursor( cursor );
		
		if( m_findOptions.replace ) {
			QMessageBox::StandardButton ret = QMessageBox::Yes;
			
			if(! m_yesToAllReplace) 
				ret = QMessageBox::question(this, tr("Application"), tr("Replace this occurence"), QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::Cancel);

			switch(ret) {
			case QMessageBox::Yes: 	
				// Replace chaine
				cursor.insertText( FindDialog::replaceStr( m_findOptions, m_findExpression, m_replaceExpression, cursor.selectedText() ) );

				on_m_searchNextAct_triggered();
				break;
			case QMessageBox::YesToAll: 	
				cursor.insertText( FindDialog::replaceStr( m_findOptions, m_findExpression, m_replaceExpression, cursor.selectedText() ) );
				m_yesToAllReplace = true;
				on_m_searchNextAct_triggered();
				m_yesToAllReplace = false;
				break;
			case QMessageBox::No: 	
				
				on_m_searchNextAct_triggered();
				break;
			default : // Do nothing else
				;
			}
		}
		
	}
}

void XMLVisualStudio::on_m_replaceAct_triggered() {
	m_findDialog->initialize();
	m_findDialog->setReplaceChecked(true);
	m_findDialog->show();
}

void XMLVisualStudio::on_m_javaViewObjectListAct_triggered() {
	JavaObjectFileImpl dlg(m_javaObjects);
	dlg.exec();
	completionNodeList->setPath(m_javaObjects->path() + "/completion.cpl");
	completionNodeList->loadFiles();
}

void XMLVisualStudio::on_m_newProjectAct_triggered() {
	ProjectPropertyImpl property ( this );
	if( property.exec() ) {
		m_xslProject = new XSLProject( );
		property.saveToProject( m_xslProject );
		
		QString fileName = QFileDialog::getSaveFileName( this, tr("Save a project"), m_xslProject->projectPath(), "Projet (*.prj)" );
		
		if( !fileName.isEmpty() )		
			m_xslProject->saveToFile( fileName );
		else 
			on_m_closeProjectAct_triggered();
	}
	updateActions();
}

void XMLVisualStudio::on_m_openProjectAct_triggered() {
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open a project"), QString(), "Projet (*.prj)" );
	
	if( !fileName.isEmpty() ) {
		m_xslProject = new XSLProject( fileName );
	}
	updateActions();
}

void XMLVisualStudio::on_m_saveProjectAct_triggered() {
	// TODO
}

void XMLVisualStudio::on_m_closeProjectAct_triggered() {
	delete m_xslProject;
	m_xslProject = NULL;
	updateActions();
}

void XMLVisualStudio::on_m_projectPropertyAct_triggered() {
	ProjectPropertyImpl property ( this );
	property.loadFromProject( m_xslProject );
	if( property.exec() ) {
		property.saveToProject( m_xslProject );
		on_m_saveProjectAct_triggered();
	}
}

void XMLVisualStudio::on_m_aboutAct_triggered() {
	QMessageBox::about(this, tr("About XML Visual Studio"),
							 tr("<b>XML Visual Studio</b> manage a <i>Generix</i> XSL/XML Project."));
}

void XMLVisualStudio::on_m_xslContentTreeView_doubleClicked(QModelIndex index) {
	QVariant line = m_xslModel->data( index, Qt::UserRole );
	StudioTextEdit * ed = m_tabEditors->currentEditor()->textEdit();
	QTextCursor cursor = ed->textCursor();
	cursor.movePosition( QTextCursor::Start );
	cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, line.toInt() - 1 );
	ed->setTextCursor( cursor );
	ed->setFocus( Qt::OtherFocusReason );
}

void XMLVisualStudio::closeEvent( QCloseEvent *event ) {
  for( int i = 0; i < m_tabEditors->count(); i++ ) {
	if ( ! maybeSave( i ) ) {
      event->ignore();
      return;
     }
  }
  writeSettings();
  event->accept();
}

void XMLVisualStudio::open( const QString & filename ) {
	m_tabEditors->loadTab(filename);
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void XMLVisualStudio::findFirst(const QString & chaine, const QString & dest, const struct FindDialog::FindOptions & options) {
	m_findExpression = chaine;
	m_replaceExpression = dest;
	m_findOptions    = options;
	m_yesToAllReplace = false;
	on_m_searchNextAct_triggered();
}

void XMLVisualStudio::slotCurrentTabChanged( int tab ) {
	Editor * ed = m_tabEditors->editor(tab);
	m_xslModel->updateModel( ed->textEdit()->toPlainText() );
}

void XMLVisualStudio::createActions() {
	// Undo
	connect(m_undoAct, SIGNAL(triggered()), m_tabEditors, SLOT(undo()));
	m_undoAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(undoAvailable(bool)), m_undoAct, SLOT(setEnabled(bool)));	

	// Redo
	connect(m_redoAct, SIGNAL(triggered()), m_tabEditors, SLOT(redo()));
	m_redoAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(redoAvailable(bool)), m_redoAct, SLOT(setEnabled(bool)));	

	// Cut
	connect(m_cutAct, SIGNAL(triggered()), m_tabEditors, SLOT(cut()));
	m_cutAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_cutAct, SLOT(setEnabled(bool)));
	
	// Copy
	connect(m_copyAct, SIGNAL(triggered()), m_tabEditors, SLOT(copy()));
	m_copyAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_copyAct, SLOT(setEnabled(bool)));	

	// Paste
	connect(m_pasteAct, SIGNAL(triggered()), m_tabEditors, SLOT(paste()));
  
	// Select All
	connect(m_selectAllAct, SIGNAL(triggered()), m_tabEditors, SLOT(selectAll()));
  
	// Duplicate Line
	connect(m_duplicateLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(duplicateCurrentLine()));
  
	// Move Line Up
	connect(m_moveUpLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineUp()));
  
	// Move Line Down
	connect(m_moveDownLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineDown()));

	// Upper Case
	connect(m_upperTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(upperSelectedText()));
	m_upperTextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_upperTextAct, SLOT(setEnabled(bool)));	

	// Lower Case
	connect(m_lowerTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(lowerSelectedText()));
	m_lowerTextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_lowerTextAct, SLOT(setEnabled(bool)));	

	// Comment
	connect(m_commentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(commentSelectedText()));
	m_commentLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_commentLineAct, SLOT(setEnabled(bool)));	

	// Uncomment
	connect(m_uncommentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(uncommentSelectedText()));
	m_uncommentLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(copyAvailable(bool)), m_uncommentLineAct, SLOT(setEnabled(bool)));	

	// Complete  
	/* Don't need, process by the editor directly */ 
	connect(m_completeAct, SIGNAL(triggered()), m_tabEditors, SLOT(complete()));
  
  	// About Qt
	connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	updateActions();
}

void XMLVisualStudio::createToolBars() {
	QToolButton * closeTab = new QToolButton(m_tabEditors);
	closeTab->setDefaultAction(m_closeAct);
	m_tabEditors->setCornerWidget(closeTab);
}

void XMLVisualStudio::createStatusBar() {
	statusBar()->showMessage(tr("Ready"));
}

void XMLVisualStudio::createDockWindows() {
  /* XSL Content Dock */
  m_xslModel = new XSLItemModel( this );
  m_xslContentTreeView->setModel( m_xslModel );

  m_windowsMenu->addAction( m_xslContentDock->toggleViewAction() ); 

  connect( m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}

void XMLVisualStudio::readSettings() {
  QSettings settings("Generix", "XML Visual Studio");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
  
  m_javaObjects->setPath(settings.value("xmljavapath", qApp->applicationDirPath() + "/xml").toString());
  m_javaObjects->loadFiles();
  completionNodeList->setPath(settings.value("xmljavapath", qApp->applicationDirPath() + "/xml").toString() + "/completion.cpl");
  completionNodeList->loadFiles();

  m_xslContentDock->setVisible(settings.value("xslContent/visible", true).toBool());
}

void XMLVisualStudio::writeSettings() {
  QSettings settings("Generix", "XML Visual Studio");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  settings.setValue("xmljavapath", m_javaObjects->path());

  settings.setValue("xslContent/visible", ! m_xslContentDock->isHidden());
}


bool XMLVisualStudio::maybeSave(int index) {
  if ( m_tabEditors->editor(index)->textEdit()->document()->isModified() ) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
                               tr("The document %1 has been modified.\n"
                                  "Do you want to save your changes?").arg( m_tabEditors->editor(index)->getCurrentFile() ),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
     if (ret == QMessageBox::Save) {
       saveEditor(index);
     } else if (ret == QMessageBox::Cancel)
       return false;
  }
  return true;
}

void XMLVisualStudio::saveEditor(int index) {
  if ( m_tabEditors->editor(index)->getCurrentFile().isEmpty() ) {
    on_m_saveAsAct_triggered();
  } else {
    m_tabEditors->editor( index )->saveFile();
  }
  m_tabEditors->updateTitle( index );

  statusBar()->showMessage( tr("File %1 saved").arg( m_tabEditors->editor(index)->getCurrentFile() ), 2000 );
}

void XMLVisualStudio::saveEditorAs(int index) {
  QString fileName = QFileDialog::getSaveFileName(this);
  if (fileName.isEmpty()) return ;

  m_tabEditors->editor(index)->saveFile(fileName);
  m_tabEditors->updateTitle(index);

  statusBar()->showMessage(tr("File %1 saved").arg( m_tabEditors->editor(index)->getCurrentFile() ), 2000);
}

void XMLVisualStudio::updateActions() {
	/* Project action */
	m_saveProjectAct->setEnabled( m_xslProject != NULL );
	m_closeProjectAct->setEnabled( m_xslProject != NULL );
	m_projectPropertyAct->setEnabled( m_xslProject != NULL );

	/* Files */
	m_saveAct->setEnabled( m_tabEditors->count() );
	m_saveAsAct->setEnabled( m_tabEditors->count() );
	m_saveAllAct->setEnabled( m_tabEditors->count() );
	m_closeAct->setEnabled( m_tabEditors->count() );
	m_closeAllAct->setEnabled( m_tabEditors->count() );
	m_printAct->setEnabled( m_tabEditors->count() );
}



