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

#include <QDir>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QPrintDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QCloseEvent>
#include <assert.h>

#include "uniqueapplication.h"

#include "xmlvisualstudio.h"
#include "tabeditor.h"
#include "editor.h"
#include "texteditor.h"
#include "fileeditor.h"
#include "objectview.h"
#include "editorcompletion.h"
#include "replacedialogimpl.h"
#include "xsllistview.h"
#include "xslproject.h"
#include "customdialogimpl.h"

#include "xmlvisualstudio.h"

#include "aboutdialogimpl.h"

#include "xinxconfig.h"

/* XMLVisualStudio */

XMLVisualStudio::XMLVisualStudio( QWidget * parent, Qt::WFlags f ) : QMainWindow(parent, f) {
	setupUi(this);

	m_xslProject       = NULL;
	m_xslModel         = NULL;
	m_sortXslModel     = NULL;
	m_recentSeparator  = NULL;
	m_aboutDialog 	   = NULL;
	
	m_cursorStart 	   = QTextCursor();
	m_cursorEnd		   = QTextCursor();

	m_lastPlace        = QDir::currentPath();

	completionContents = new Completion();
	m_javaObjects      = new ObjectsView();

	m_findDialog       = new ReplaceDialogImpl(this);
	connect( m_findDialog, SIGNAL(find(QString, QString, ReplaceDialogImpl::FindOptions)), this, SLOT(findFirst(QString, QString, ReplaceDialogImpl::FindOptions)) );

	createProjectPart();
	createWebServicesPart();
	
	createDockWindows();
	readSettings();
	createActions();
	createToolBars();
	createStatusBar();
	
	// Connection for auto open
	connect( static_cast<UniqueApplication*>(qApp), SIGNAL(hasFileToOpen(QString)), this, SLOT(open(QString)) );
	connect( m_tabEditors, SIGNAL(fileDragged()), this, SLOT(updateActions()) );
	connect( m_tabEditors, SIGNAL(closeTab(int)), this, SLOT(slotCloseFile(int)) );
	connect( m_tabEditors, SIGNAL(refreshTab(int)), this, SLOT(slotRefreshFile(int)) );
	connect( m_tabEditors, SIGNAL(saveTab(int)), this, SLOT(saveEditor(int)) );
	connect( m_tabEditors, SIGNAL(saveAsTab(int)), this, SLOT(saveEditorAs(int)) );
}

void XMLVisualStudio::createDockWindows() {
	/* XSL Content Dock */
	m_windowsMenu->addAction( m_xslContentDock->toggleViewAction() ); 

	connect( m_tabEditors, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)) );

	m_windowsMenu->addAction( m_webServicesDock->toggleViewAction() ); 
	m_webServicesTreeView->header()->hide();
	
	m_windowsMenu->addAction( m_projectDirectoryDock->toggleViewAction() ); 
}

void XMLVisualStudio::readSettings() {
	resize( xinxConfig->size() );
	move( xinxConfig->position() );
  
	m_javaObjects->setPath( xinxConfig->objectDescriptionPath() );
	m_javaObjects->loadFiles();
  
	completionContents->setPath( QDir( xinxConfig->completionFilesPath() ).filePath( "completion.cpl" ) );

	m_xslContentDock->setVisible( xinxConfig->isDockSet( XINXConfig::contents ) );
	m_webServicesDock->setVisible( xinxConfig->isDockSet( XINXConfig::services ) );
	m_projectDirectoryDock->setVisible( xinxConfig->isDockSet( XINXConfig::files ) );
}

void XMLVisualStudio::writeSettings() {
	xinxConfig->setPosition( pos() );
	xinxConfig->setSize( size() );
	xinxConfig->setObjectDescriptionPath( m_javaObjects->path() );

	xinxConfig->setDock( XINXConfig::contents, m_xslContentDock->isHidden() );
	xinxConfig->setDock( XINXConfig::services, m_webServicesDock->isHidden() );
	xinxConfig->setDock( XINXConfig::files, m_projectDirectoryDock->isHidden() );
	
	xinxConfig->save();
}

void XMLVisualStudio::createActions() {
	QMenu * newMenu = new QMenu( this );
	newMenu->addAction( m_newStylesheetFileAct );
	newMenu->addAction( m_newXMLFileAct );
	newMenu->addAction( m_newJavascriptFileAct );
	newMenu->addAction( m_newWebServicesFileAct );
	
	m_newAct->setMenu( newMenu );
	
	connect( m_tabEditors, SIGNAL(modelCreated()), this, SLOT(slotModelCreated()) );
	connect( m_tabEditors, SIGNAL(modelDeleted()), this, SLOT(slotModelDeleted()) );
	connect( m_tabEditors, SIGNAL(setEditorPosition(int,int)), this, SLOT(setEditorPosition(int,int)));	
	
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
	setupRecentProjectMenu( m_recentProjectMenu );
	setupRecentFileMenu( m_recentFileMenu );

	updateActions();
	updateRecentFiles();
}

void XMLVisualStudio::updateActions() {
	/* Project action */
	m_saveProjectAct->setEnabled( m_xslProject != NULL );
	m_closeProjectAct->setEnabled( m_xslProject != NULL );
	m_closeProjectSessionAct->setEnabled( m_xslProject != NULL );
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

void XMLVisualStudio::createToolBars() {
	QToolButton * closeTab = new QToolButton(m_tabEditors);
	closeTab->setDefaultAction(m_closeAct);
	m_tabEditors->setCornerWidget(closeTab);
}

void XMLVisualStudio::createStatusBar() {
	m_editorPosition = new QLabel( "000x000" );
	statusBar()->addPermanentWidget( m_editorPosition );
	setEditorPosition( 1, 1 );
	
	statusBar()->showMessage(tr("Ready"), 2000);
}

void XMLVisualStudio::setEditorPosition( int line, int column ) {
	m_editorPosition->setText( QString("   %1 x %2   ").arg(line, 3, 10, QLatin1Char('0')).arg(column, 3, 10, QLatin1Char('0')) );
}


/* Actions */

void XMLVisualStudio::on_m_newAct_triggered() {
	if( m_xslProject && ( m_xslProject->projectType() == XSLProject::SERVICES ) ) 
		on_m_newWebServicesFileAct_triggered();
	else
		on_m_newStylesheetFileAct_triggered();
}


void XMLVisualStudio::on_m_newStylesheetFileAct_triggered() {
	m_tabEditors->newFileEditorXSL( m_xslProject );
	updateActions();
}

void XMLVisualStudio::on_m_newXMLFileAct_triggered() {
	m_tabEditors->newFileEditorXML( m_xslProject );
	updateActions();
}

void XMLVisualStudio::on_m_newJavascriptFileAct_triggered() {
	m_tabEditors->newFileEditorJS( m_xslProject );
	updateActions();
}

void XMLVisualStudio::on_m_newWebServicesFileAct_triggered() {
	m_tabEditors->newFileEditorWS( m_xslProject, m_webServices );
//	if( m_xslProject && ( m_xslProject->projectType() == XSLProject::SERVICES ) ) 
//		newWebServices( qobject_cast<FileEditor*>( m_tabEditors->currentEditor() ) );
	updateActions();
}



void XMLVisualStudio::on_m_openAct_triggered() {
	QStringList selectedFiles = QFileDialog::getOpenFileNames( this, tr("Open text file"), m_lastPlace, xinxConfig->dialogFilters().join(";;") );
	foreach( QString filename, selectedFiles ) {
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

	slotCloseFile( index );
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

void XMLVisualStudio::on_m_aboutAct_triggered() {
	if( ! m_aboutDialog ) 
		m_aboutDialog = new AboutDialogImpl( this );
	m_aboutDialog->exec();
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

void XMLVisualStudio::on_m_customApplicationAct_triggered() {
	CustomDialogImpl * custom = new CustomDialogImpl( this );
	writeSettings();
	custom->loadFromConfig( xinxConfig );
	if( custom->exec() ) {
		custom->saveToConfig( xinxConfig );
		xinxConfig->save();	
		readSettings();	
	}
	
	delete custom;
}

void XMLVisualStudio::closeEvent( QCloseEvent *event ) {
	if( ! xinxConfig->saveSessionByDefault() ) {
		for( int i = 0; i < m_tabEditors->count(); i++ ) {
			if ( ! maybeSave( i ) ) {
				event->ignore();
				return;
			}
		}
	}
	closeProject( false, xinxConfig->saveSessionByDefault() );
	
	writeSettings();
	
	event->accept();
}

void XMLVisualStudio::open( const QString & filename ) {
	assert( ! filename.isEmpty() );

	if( m_xslProject ) {
		m_xslProject->lastOpenedFile().removeAll( filename );
		m_xslProject->lastOpenedFile().prepend( filename );
     
		while( m_xslProject->lastOpenedFile().size() > MAXRECENTFILES )
			m_xslProject->lastOpenedFile().removeLast();
	}

	m_tabEditors->loadFileEditor( filename, m_xslProject );
	updateRecentFiles();
	updateActions();
	statusBar()->showMessage(tr("File loaded"), 2000);
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

void XMLVisualStudio::slotCloseFile( int index ) {
	if( maybeSave( index ) ) {
		m_tabEditors->removeTab( index );
	}
	
	updateActions();
}

void XMLVisualStudio::slotRefreshFile( int index ) {
	assert( index >= 0 );
	
	if( TabEditor::isFileEditor( m_tabEditors->editor( index ) ) ) {
		FileEditor * ed = static_cast<FileEditor*>( m_tabEditors->editor( index ) );
		bool act = ! ed->isModified();
		
		if( ! act ) {
			QMessageBox::StandardButton ret;
			ret = QMessageBox::question(this, tr("Application"),
										tr("The document %1 has been modified.\n"
										"Do you really want refresh this?").arg( ed->getTitle() ),
										QMessageBox::Yes | QMessageBox::No);
			act = ( ret == QMessageBox::Yes );
		}
		if( act ) {
			ed->loadFile();
			ed->setModified( false );
		}
	}
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
		if( xinxConfig->isAlertWhenStdFile() && m_xslProject && (! QFileInfo( fileName ).fileName().startsWith( m_xslProject->specifPrefix().toLower() + "_" ) ) && xinxConfig->managedFile4Name( fileName ).canBeCustomize ) {
			QMessageBox::StandardButton res = QMessageBox::warning( this, tr("Save standard XSL"), tr("You're being to save standard file, do you whant make it specifique"), QMessageBox::Yes | QMessageBox::No );
			if( res == QMessageBox::Yes )
				saveEditorAs( index );
			else
				dynamic_cast<FileEditor*>( m_tabEditors->editor( index ) )->saveFile();
		} else
			dynamic_cast<FileEditor*>( m_tabEditors->editor( index ) )->saveFile();
	}
	
	m_tabEditors->editor( index )->setModified( false );

	statusBar()->showMessage( tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
}

void XMLVisualStudio::saveEditorAs( int index ) {
	assert( index >= 0 );
	assert( index < m_tabEditors->count() );

	/*
	QFileDialog dlg( this, tr("Save text file") );
	dlg.setAcceptMode( QFileDialog::AcceptSave );
    dlg.setFileMode( QFileDialog::AnyFile );
	dlg.setFilters( xinxConfig->dialogFilters() );
	*/
	
	QString fileName    = qobject_cast<FileEditor*>( m_tabEditors->editor(index) )->getFileName(),
			fileSuffix  = qobject_cast<FileEditor*>( m_tabEditors->editor(index) )->getSuffix(),
			specifPath,
			filter = xinxConfig->dialogFilter( fileSuffix ),
			newFileName;
	/*dlg.setDefaultSuffix( fileSuffix );*/

	struct XINXConfig::managedFile customFile = xinxConfig->managedFile4Suffix( fileSuffix );

	if( m_xslProject ) 
		specifPath = QDir( m_xslProject->specifPath() ).absoluteFilePath( customFile.customPath );
	else
	 	specifPath = m_lastPlace;
 	

	if( fileName.isEmpty() ) {
		if( m_xslProject ) 
			newFileName = QDir( specifPath ).
				absoluteFilePath( m_xslProject->specifPrefix().toLower() + "_" );
		else
			newFileName = specifPath;
		/*dlg.setDirectory( specifPath );
		dlg.selectFile( m_xslProject->specifPrefix().toLower() + "_" );*/
	} else {
		bool isCustomized = 
			m_xslProject && QFileInfo( fileName ).fileName().startsWith( m_xslProject->specifPrefix().toLower() );
		if( m_xslProject && customFile.canBeCustomize && (!isCustomized) ) {
			newFileName = QDir( specifPath ).
				absoluteFilePath( m_xslProject->specifPrefix().toLower() + "_" + QFileInfo( fileName ).fileName() );
			/*dlg.setDirectory( specifPath );
			dlg.selectFile( m_xslProject->specifPrefix().toLower() + "_" + QFileInfo( fileName ).fileName() );*/
		} else {
			newFileName = fileName;
			/*dlg.setDirectory( QFileInfo( fileName ).absolutePath() );
			dlg.selectFile( QFileInfo( fileName ).fileName() );*/
		}
	}
	
	fileName = QFileDialog::getSaveFileName( this, tr("Save text file"), newFileName, xinxConfig->dialogFilters().join(";;"), &filter );
	
	/*if( dlg.exec() == QDialog::Accepted ) {
		if( dlg.selectedFiles().count() == 1 )
			fileName = dlg.selectedFiles()[ 0 ];
		else
			return;
	} else
		return;*/
	
	if( fileName.isEmpty() ) return ;

	m_lastPlace = QFileInfo( fileName ).absolutePath();

	dynamic_cast<FileEditor*>( m_tabEditors->editor(index) )->saveFile( fileName );
	m_tabEditors->editor( index )->setModified( true );
	m_tabEditors->editor( index )->setModified( false );

	statusBar()->showMessage(tr("File %1 saved").arg( m_tabEditors->editor(index)->getTitle() ), 2000 );
}

void XMLVisualStudio::setCurrentProject( const QString & filename ) {
	Q_UNUSED( filename );
	if( ! m_xslProject )
		setWindowTitle( tr("XINX") );
	else {
		setWindowTitle( tr("%1 - %2").arg( m_xslProject->projectName() ).arg( tr("XINX") ) );
	}
}


