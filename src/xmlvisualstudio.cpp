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
#include <assert.h>

#include "uniqueapplication.h"

#include "xmlvisualstudio.h"
#include "tabeditor.h"
#include "editor.h"
#include "fileeditor.h"
#include "javaobjectfileimpl.h"
#include "objectview.h"
#include "editorcompletion.h"
#include "finddialog.h"
#include "xsllistview.h"
#include "projectpropertyimpl.h"
#include "xslproject.h"

#include "xmlvisualstudio.h"

#include "aboutdialogimpl.h"

#define OPEN_SAVE_DIALOG_FILTER "*.xsl *.xml *.js"

XMLVisualStudio::XMLVisualStudio( QWidget * parent, Qt::WFlags f) : QMainWindow(parent, f) {
	setupUi(this);

	m_xslProject       = NULL;
	m_xslModel         = NULL;
	m_sortXslModel     = NULL;
	m_recentSeparator  = NULL;
	m_aboutDialog 	   = NULL;

	m_lastPlace    = QDir::currentPath();

	m_settings         = new QSettings("Generix", "XINX");
	m_javaObjects      = new ObjectsView();
	completionNodeList = new CplNodeList();

	m_findDialog       = new FindDialog(this);
	connect( m_findDialog, SIGNAL(find(QString, QString, FindDialog::FindOptions)), this, SLOT(findFirst(QString, QString, FindDialog::FindOptions)) );

	createDockWindows();
	readSettings();
	createActions();
	createToolBars();
	createStatusBar();

	// Connection for auto open
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(hasFileToOpen(QString)), this, SLOT(open(QString)) );
}

void XMLVisualStudio::createDockWindows() {
	/* XSL Content Dock */
	m_windowsMenu->addAction( m_xslContentDock->toggleViewAction() ); 

	connect( m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );
}

void XMLVisualStudio::readSettings() {
	QPoint pos = m_settings->value( "pos", QPoint(200, 200) ).toPoint();
	QSize size = m_settings->value( "size", QSize(400, 400) ).toSize();
	resize(size);
	move(pos);
  
	m_javaObjects->setPath( m_settings->value("xmljavapath", qApp->applicationDirPath() + "/xml").toString() );
	m_javaObjects->loadFiles();
  
	completionNodeList->setPath( m_settings->value("xmljavapath", qApp->applicationDirPath() + "/xml").toString() + "/completion.cpl" );
	completionNodeList->loadFiles();

	m_xslContentDock->setVisible( m_settings->value("xslContent/visible", true).toBool() );
}

void XMLVisualStudio::writeSettings() {
	m_settings->setValue( "pos", pos() );
	m_settings->setValue( "size", size() );
	m_settings->setValue( "xmljavapath", m_javaObjects->path() );

	m_settings->setValue( "xslContent/visible", ! m_xslContentDock->isHidden() );
}

void XMLVisualStudio::createActions() {
	connect( m_tabEditors, SIGNAL(modelCreated()), this, SLOT(slotModelCreated()) );
	connect( m_tabEditors, SIGNAL(modelDeleted()), this, SLOT(slotModelDeleted()) );
	
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
	m_pasteAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(pasteAvailable(bool)), m_pasteAct, SLOT(setEnabled(bool)));	
  
	// Select All
	connect(m_selectAllAct, SIGNAL(triggered()), m_tabEditors, SLOT(selectAll()));
	m_selectAllAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_selectAllAct, SLOT(setEnabled(bool)));	
  
	// Duplicate Line
	connect(m_duplicateLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(duplicateCurrentLine()));
	m_duplicateLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_duplicateLineAct, SLOT(setEnabled(bool)));	
  
	// Move Line Up
	connect(m_moveUpLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineUp()));
	m_moveUpLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_moveUpLineAct, SLOT(setEnabled(bool)));	
  
	// Move Line Down
	connect(m_moveDownLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(moveLineDown()));
	m_moveDownLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_moveDownLineAct, SLOT(setEnabled(bool)));	

	// Upper Case
	connect(m_upperTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(upperSelectedText()));
	m_upperTextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_upperTextAct, SLOT(setEnabled(bool)));	

	// Lower Case
	connect(m_lowerTextAct, SIGNAL(triggered()), m_tabEditors, SLOT(lowerSelectedText()));
	m_lowerTextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_lowerTextAct, SLOT(setEnabled(bool)));	

	// Comment
	connect(m_commentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(commentSelectedText()));
	m_commentLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_commentLineAct, SLOT(setEnabled(bool)));	

	// Uncomment
	connect(m_uncommentLineAct, SIGNAL(triggered()), m_tabEditors, SLOT(uncommentSelectedText()));
	m_uncommentLineAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(hasTextSelection(bool)), m_uncommentLineAct, SLOT(setEnabled(bool)));	

	// Complete  
	/* Don't need, process by the editor directly */ 
	connect(m_completeAct, SIGNAL(triggered()), m_tabEditors, SLOT(complete()));
	m_completeAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_completeAct, SLOT(setEnabled(bool)));	
	
	// Search 
	m_searchAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_searchAct, SLOT(setEnabled(bool)));	
	
	// Search next/previous
	m_searchNextAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_searchNextAct, SLOT(setEnabled(bool)));	

	// Replace
	m_replaceAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_replaceAct, SLOT(setEnabled(bool)));	
  
  	// About Qt
	connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	// Previous/Next Tab
	m_previousTabAct->setShortcut( QKeySequence::PreviousChild );
	m_nextTabAct->setShortcut( QKeySequence::NextChild );

	// Indent	
	connect(m_indentAct, SIGNAL(triggered()), m_tabEditors, SLOT(indent()));
	m_indentAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_indentAct, SLOT(setEnabled(bool)));	

	// Unindent	
	connect(m_unindentAct, SIGNAL(triggered()), m_tabEditors, SLOT(unindent()));
	m_unindentAct->setEnabled(false);
	connect(m_tabEditors, SIGNAL(textAvailable(bool)), m_unindentAct, SLOT(setEnabled(bool)));	

	// Recent project file
	m_recentSeparator = m_recentProjectMenu->addSeparator();
	for(int i = 0; i < MAXRECENTFILES; i++) {
		m_recentProjectActs[i] = new QAction( this );
		m_recentProjectActs[i]->setVisible( false );
		m_recentProjectMenu->addAction( m_recentProjectActs[i] );
		connect( m_recentProjectActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()) );
	}

	updateActions();
	updateRecentFiles();
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
	m_previousTabAct->setEnabled( m_tabEditors->count() );
	m_nextTabAct->setEnabled( m_tabEditors->count() );
	
	if( m_tabEditors->count() == 0 ) {
		if( m_sortXslModel ) delete m_sortXslModel;
		m_xslModel = NULL;
		m_sortXslModel = NULL;
		m_xslContentTreeView->setModel( NULL );
	}
}

void XMLVisualStudio::updateRecentFiles() {
	QStringList files = m_settings->value( "Recent Project Files" ).toStringList();
	int numRecentFiles = qMin( files.size(), MAXRECENTFILES );

	for( int i = 0; i < numRecentFiles; i++ ) {
		QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( files[i] ).fileName() );
		m_recentProjectActs[i]->setText( text );
		m_recentProjectActs[i]->setData( files[i] );
		m_recentProjectActs[i]->setVisible( true );
	}
	
	for( int j = numRecentFiles; j < MAXRECENTFILES; j++ )
		m_recentProjectActs[j]->setVisible(false);

	m_recentSeparator->setVisible( numRecentFiles > 0 );
}

void XMLVisualStudio::createToolBars() {
	QToolButton * closeTab = new QToolButton(m_tabEditors);
	closeTab->setDefaultAction(m_closeAct);
	m_tabEditors->setCornerWidget(closeTab);
}

void XMLVisualStudio::createStatusBar() {
	statusBar()->showMessage(tr("Ready"));
}

/* Actions */

void XMLVisualStudio::on_m_newAct_triggered() {
	m_tabEditors->newFileEditor();
	updateActions();
}

void XMLVisualStudio::on_m_openAct_triggered() {
	QString filename = QFileDialog::getOpenFileName( this, tr("Open XSL File"), m_lastPlace, tr( OPEN_SAVE_DIALOG_FILTER ) );
	if ( ! filename.isEmpty() ) {
		m_lastPlace = QFileInfo( filename ).absolutePath();
		open( filename );
	}
}

void XMLVisualStudio::on_m_saveAct_triggered() {
	assert( m_tabEditors->currentIndex() != -1 );
	saveEditor( m_tabEditors->currentIndex() );
}

void XMLVisualStudio::on_m_saveAsAct_triggered() {
	assert( m_tabEditors->currentIndex() != -1 );
	saveEditorAs( m_tabEditors->currentIndex() );
}

void XMLVisualStudio::on_m_saveAllAct_triggered() {
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		saveEditor(i); 	
	}
}

void XMLVisualStudio::on_m_printAct_triggered() {
	assert( m_tabEditors->currentEditor() != NULL );
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		FileEditor * editor = static_cast<FileEditor*>( m_tabEditors->currentEditor() );
		QTextDocument *document = editor->textEdit()->document();
		QPrinter printer;

		QPrintDialog *dlg = new QPrintDialog( &printer, this );
		if ( dlg->exec() != QDialog::Accepted ) return;

		document->print( &printer );
  
		delete dlg;

		statusBar()->showMessage( tr("Printed"), 2000 );
	}
}

void XMLVisualStudio::on_m_closeAct_triggered() {
	int index = m_tabEditors->currentIndex();
	
	assert( index >= 0 );
	
	if( maybeSave( index ) ) {
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
	QTextEdit * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplaceChecked(false);
	m_findDialog->show();
}

void XMLVisualStudio::on_m_searchNextAct_triggered() {
	assert( m_tabEditors->currentEditor() );
	
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		QTextEdit * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
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
}

void XMLVisualStudio::on_m_replaceAct_triggered() {
	QTextEdit * textEdit = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
	if( ! textEdit->textCursor().selectedText().isEmpty() ){
		m_findDialog->setText( textEdit->textCursor().selectedText() );
	}
	m_findDialog->initialize();
	m_findDialog->setReplaceChecked(true);
	m_findDialog->show();
}

void XMLVisualStudio::on_m_javaViewObjectListAct_triggered() {
	assert( m_javaObjects );
	
	JavaObjectFileImpl dlg( m_javaObjects );
	if( dlg.exec() )  {
		completionNodeList->setPath(m_javaObjects->path() + "/completion.cpl");
		completionNodeList->loadFiles();
	}
}

void XMLVisualStudio::on_m_newProjectAct_triggered() {
	ProjectPropertyImpl property ( this );
	if( property.exec() ) {
		XSLProject* project = new XSLProject( );
		property.saveToProject( project );
		
		QString fileName = QFileDialog::getSaveFileName( this, tr("Save a project"), project->projectPath(), "Projet (*.prj)" );
		
		if( ! fileName.isEmpty() ) {
			project->saveToFile( fileName );
			m_lastPlace = project->projectPath();
			closeProject( true );
			m_xslProject = project;
			setCurrentProject( fileName );
		} else 
			delete project;
	}
	
	updateActions();
}

void XMLVisualStudio::on_m_openProjectAct_triggered() {
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open a project"), QString(), "Projet (*.prj)" );
	openProject( fileName );	
}

void XMLVisualStudio::on_m_saveProjectAct_triggered() {
	assert( m_xslProject != NULL );
	
	m_xslProject->openedFiles().clear();
	
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		if( TabEditor::isFileEditor( m_tabEditors->editor( i ) ) ) {
			m_xslProject->openedFiles().append( static_cast<FileEditor*>( m_tabEditors->editor( i ) )->getFileName() );
		}
	}
	
	m_xslProject->saveToFile();
}

void XMLVisualStudio::on_m_closeProjectAct_triggered() {
	closeProject( true );
}

void XMLVisualStudio::on_m_projectPropertyAct_triggered() {
	assert( m_xslProject != NULL );
	
	ProjectPropertyImpl property ( this );
	property.loadFromProject( m_xslProject );
	if( property.exec() ) {
		property.saveToProject( m_xslProject );
		on_m_saveProjectAct_triggered();
	}
}

void XMLVisualStudio::on_m_aboutAct_triggered() {
	//QMessageBox::about(this, tr("About XINX"),
							 //tr("<b>XINX</b> is not XDME. \n"
							    //"<b>XINX</b> is a software written by U. VAN DEN HEKKE. \n"
							    //"Thanks to report detailled bug ;)"));
	if( ! m_aboutDialog ) 
		m_aboutDialog = new AboutDialogImpl( this );
	m_aboutDialog->show();
}

void XMLVisualStudio::on_m_xslContentTreeView_doubleClicked( QModelIndex index ) {
	assert( m_tabEditors->currentEditor() != NULL );
	
	if( TabEditor::isFileEditor( m_tabEditors->currentEditor() ) ) {
		QModelIndex mappingIndex = m_sortXslModel->mapToSource( index );
		
		struct XSLItemModel::user_data data = m_xslModel->data( mappingIndex, Qt::UserRole ).value<XSLItemModel::user_data>();
		int line = data.line;

		if( ! data.filename.isEmpty() )
			open( data.filename );
		
		// Deplace to rigth line.
		QTextEdit * ed = static_cast<FileEditor*>( m_tabEditors->currentEditor() )->textEdit();
		QTextCursor cursor = ed->textCursor();
		cursor.movePosition( QTextCursor::Start );
		cursor.movePosition( QTextCursor::NextBlock, QTextCursor::MoveAnchor, line - 1 );
		ed->setTextCursor( cursor );
		ed->setFocus( Qt::OtherFocusReason );
	}
}

void XMLVisualStudio::on_m_nextTabAct_triggered() {
	m_tabEditors->setCurrentIndex( ( m_tabEditors->currentIndex() + 1 ) % m_tabEditors->count() );
}

void XMLVisualStudio::on_m_previousTabAct_triggered() {
	m_tabEditors->setCurrentIndex( ( m_tabEditors->currentIndex() - 1 ) % m_tabEditors->count() );
}

void XMLVisualStudio::closeEvent( QCloseEvent *event ) {
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		if ( ! maybeSave( i ) ) {
			event->ignore();
			return;
		}
	}
	closeProject( false );
	
	writeSettings();
	
	event->accept();
}

void XMLVisualStudio::open( const QString & filename ) {
	assert( ! filename.isEmpty() );
	
	m_tabEditors->loadFileEditor( filename, m_xslProject );
	updateActions();
	statusBar()->showMessage(tr("File loaded"), 2000);
}

void XMLVisualStudio::findFirst(const QString & chaine, const QString & dest, const struct FindDialog::FindOptions & options) {
	m_findExpression    = chaine;
	m_replaceExpression = dest;
	m_findOptions       = options;
	m_yesToAllReplace   = false;

	on_m_searchNextAct_triggered();
}

void XMLVisualStudio::slotModelDeleted() {
	m_xslContentTreeView->setModel( NULL );

	if( m_sortXslModel ) {
		delete m_sortXslModel;
		m_sortXslModel = NULL;
		m_xslModel = NULL;
	}
}

void XMLVisualStudio::slotModelCreated() {
	m_xslModel = m_tabEditors->currentEditor( )->model();
	if( m_xslModel ) {
		m_sortXslModel = new QSortFilterProxyModel( this );
		m_sortXslModel->setSourceModel( m_xslModel );
		m_xslContentTreeView->setModel( m_sortXslModel );
	}
}

void XMLVisualStudio::slotCurrentTabChanged( int tab ) {
	assert( tab < m_tabEditors->count() );
	assert( 0   < m_tabEditors->count() );

	slotModelDeleted();
	slotModelCreated();
}

void XMLVisualStudio::openRecentProject() {
     QAction * action = qobject_cast<QAction *>( sender() );
     if( action )
         openProject( action->data().toString() );	
}

bool XMLVisualStudio::maybeSave( int index ) {
	assert( index >= 0 );
	assert( index < m_tabEditors->count() );
	
	if ( m_tabEditors->editor( index )->isModified() ) {
		QMessageBox::StandardButton ret;
		ret = QMessageBox::warning(this, tr("Application"),
									tr("The document %1 has been modified.\n"
									"Do you want to save your changes?").arg( m_tabEditors->editor(index)->getTitle() ),
									QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save) {
			saveEditor(index);
		} else if (ret == QMessageBox::Cancel)
			return false;
	}
	return true;
}

void XMLVisualStudio::saveEditor( int index ) {
	assert( index >= 0 );
	assert( index < m_tabEditors->count() );
	
	if ( ! m_tabEditors->editor( index )->hasName() ) {
		on_m_saveAsAct_triggered();
	} else {
		QString fileName = dynamic_cast<FileEditor*>( m_tabEditors->editor(index) )->getFileName();
		if( m_xslProject && (! QFileInfo( fileName ).fileName().startsWith( m_xslProject->specifPrefix().toLower() + "_" ) ) ) {
			QMessageBox::warning( this, tr("Save standard XSL"), tr("You're being to save standard file, please save it as specifique") );
			saveEditorAs( index );
		} else
			dynamic_cast<FileEditor*>( m_tabEditors->editor( index ) )->saveFile();
	}
	
	m_tabEditors->editor( index )->setModified( false );

	statusBar()->showMessage( tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
}

void XMLVisualStudio::saveEditorAs( int index ) {
	assert( index >= 0 );
	assert( index < m_tabEditors->count() );
	
	QString fileName = dynamic_cast<FileEditor*>( m_tabEditors->editor(index) )->getFileName();
	if( ( ! fileName.isEmpty() ) && ( m_xslProject ) ) {
		fileName = QFileInfo( fileName ).fileName();
		if( ! fileName.startsWith( m_xslProject->specifPrefix().toLower() + "_" ) ) {
			fileName = m_xslProject->specifPrefix().toLower() + "_" + fileName;
			fileName = QDir( m_xslProject->specifPath() ).absoluteFilePath( fileName );
		} else 
			fileName = QDir( m_lastPlace ).absoluteFilePath( fileName );
	} else 
		fileName = m_lastPlace;
	
	fileName = QFileDialog::getSaveFileName( 
		this, 
		tr("Save XSL File"), 
		fileName, 
		tr( OPEN_SAVE_DIALOG_FILTER )
	);
	
	if( fileName.isEmpty() ) return ;

	m_lastPlace = QFileInfo( fileName ).absolutePath();

	dynamic_cast<FileEditor*>( m_tabEditors->editor(index) )->saveFile( fileName );
	m_tabEditors->editor( index )->setModified( true );
	m_tabEditors->editor( index )->setModified( false );

	statusBar()->showMessage(tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
}

void XMLVisualStudio::openProject( const QString & filename ) {
	if( ! filename.isEmpty() ) {
		if( m_xslProject ) closeProject( true );
		
		m_xslProject = new XSLProject( filename );
		m_lastPlace = m_xslProject->projectPath();
		setCurrentProject( filename );
		
		foreach( QString str, m_xslProject->openedFiles() ) {
			open( str );
		}

		updateActions();
	}
}

void XMLVisualStudio::closeProject( bool closeAll ) {
	if( m_xslProject ) {
		on_m_saveProjectAct_triggered();
		
		if( closeAll )
			on_m_closeAllAct_triggered();

		delete m_xslProject;
		m_xslProject = NULL;
		
		updateActions();
		setCurrentProject( "" );
	}
}

void XMLVisualStudio::setCurrentProject( const QString & filename ) {
	if( filename.isEmpty() )
		setWindowTitle( tr("XINX") );
	else {
		setWindowTitle( tr("%1 - %2").arg( QFileInfo( filename ).fileName() ).arg( tr("XINX") ) );

		QStringList files = m_settings->value( "Recent Project Files" ).toStringList();
		files.removeAll( filename );
		files.prepend( filename );
     
		while( files.size() > MAXRECENTFILES )
			files.removeLast();

		m_settings->setValue( "Recent Project Files", files );

		updateRecentFiles();
	}
}

