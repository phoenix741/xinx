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
#include <QDirModel>
#include <QAction>
#include <QFileDialog>
#include <QTimer>
#include <QHeaderView>

#include <assert.h>

#include "xmlvisualstudio.h" 
#include "xinxconfig.h"
#include "xslproject.h"
#include "projectpropertyimpl.h"
#include "fileeditor.h"

#define DEFAULT_PROJECT_FILTRE QStringList() << "*.xml" << "*.xsl" << "*.js" << "*.fws"
#define DEFAULT_PROJECT_FILTRE_OPTIONS QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot

void XMLVisualStudio::createProjectPart() {
	m_lastProjectOpenedPlace = QDir::currentPath();
	m_dirModel = new QDirModel( DEFAULT_PROJECT_FILTRE, DEFAULT_PROJECT_FILTRE_OPTIONS, QDir::DirsFirst, m_projectDirectoryTreeView );
	m_modelTimer = new QTimer( this );
	m_modelTimer->setInterval( 500 );
	connect( m_modelTimer, SIGNAL(timeout()), this, SLOT(filtreChange()) );
	m_projectDirectoryTreeView->header()->hide();
	
	connect( m_newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()) );
	connect( m_openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()) );
	connect( m_saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()) );
	connect( m_projectPropertyAct, SIGNAL(triggered()), this, SLOT(changeProjectProperty()) );
}

void XMLVisualStudio::setupRecentProjectMenu( QMenu * menu ) {
	m_recentSeparator = menu->addSeparator();
	for(int i = 0; i < MAXRECENTFILES; i++) {
		m_recentProjectActs[i] = new QAction( this );
		m_recentProjectActs[i]->setVisible( false );
		menu->addAction( m_recentProjectActs[i] );
		connect( m_recentProjectActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()) );
	}
	
	updateRecentFiles();
}

void XMLVisualStudio::newProject() {
	ProjectPropertyImpl property ( this );
	if( ! property.exec() ) return;
		
	XSLProject* project = new XSLProject( );
	property.saveToProject( project );

	QString filename = QFileDialog::getSaveFileName( this, tr("Save a project"), project->projectPath(), "Projet (*.prj)" );
	if( filename.isEmpty() ) {
		delete project;
		return;
	}
	m_lastProjectOpenedPlace = project->projectPath();

	closeProject( true );
	project->saveToFile( filename );
	delete project;
	
	openProject( filename );
}

void XMLVisualStudio::openRecentProject() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		openProject( action->data().toString() );	
}

void XMLVisualStudio::openProject() {
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open a project"), xinxConfig->xinxProjectPath(), "Projet (*.prj)" );
	openProject( fileName );	
}

void XMLVisualStudio::openProject( const QString & filename ) {
	assert( ! filename.isEmpty() );

	if( m_xslProject ) 
		closeProject( true ); 
	else 
		on_m_closeAllAct_triggered();
		
	m_xslProject             = new XSLProject( filename );
	m_lastProjectOpenedPlace = m_xslProject->projectPath();

	xinxConfig->recentProjectFiles().removeAll( filename );
	xinxConfig->recentProjectFiles().prepend( filename );
     
	while( xinxConfig->recentProjectFiles().size() > MAXRECENTFILES )
		xinxConfig->recentProjectFiles().removeLast();

	setCurrentProject( filename );
		
	foreach( QString str, m_xslProject->openedFiles() )
		if( ! str.isEmpty() ) 
			open( str );

	if( m_xslProject->projectType() == XSLProject::SERVICES )
		setWebServicesView( true );

	m_projectDirectoryTreeView->setModel( m_dirModel );
	for(int i = 1; i < m_dirModel->columnCount(); i++ )
		m_projectDirectoryTreeView->hideColumn( i );
	m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( m_xslProject->projectPath() ) );

	updateActions();
	updateRecentFiles();
	
}

void XMLVisualStudio::saveProject() {
	assert( m_xslProject != NULL );
	
	m_xslProject->openedFiles().clear();
	
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		if( TabEditor::isFileEditor( m_tabEditors->editor( i ) ) ) {
			m_xslProject->openedFiles().append( qobject_cast<FileEditor*>( m_tabEditors->editor( i ) )->getFileName() );
		}
	}
	
	m_xslProject->saveToFile();
}

void XMLVisualStudio::changeProjectProperty() {
	assert( m_xslProject != NULL );
	
	ProjectPropertyImpl property ( this );
	property.loadFromProject( m_xslProject );
	if( property.exec() ) {
		property.saveToProject( m_xslProject );
		refreshWebServicesList();
		saveProject();
	}
}

void XMLVisualStudio::on_m_closeProjectAct_triggered() {
	closeProject( true );
}

void XMLVisualStudio::closeProject( bool closeAll ) {
	if( ! m_xslProject ) return;
		
	saveProject();
	if( closeAll ) on_m_closeAllAct_triggered();

	m_projectDirectoryTreeView->setModel( NULL );
	setWebServicesView( false );

	delete m_xslProject;
	m_xslProject = NULL;

	updateActions();
	setCurrentProject( "" );
}

void XMLVisualStudio::updateRecentFiles() {
	int numRecentFiles = qMin( xinxConfig->recentProjectFiles().size(), MAXRECENTFILES );

	for( int i = 0; i < numRecentFiles; i++ ) {
		QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( xinxConfig->recentProjectFiles()[i] ).fileName() );
		m_recentProjectActs[i]->setText( text );
		m_recentProjectActs[i]->setData( xinxConfig->recentProjectFiles()[i] );
		m_recentProjectActs[i]->setVisible( true );
	}
	
	for( int j = numRecentFiles; j < MAXRECENTFILES; j++ )
		m_recentProjectActs[j]->setVisible(false);

	m_recentSeparator->setVisible( numRecentFiles > 0 );
}

void XMLVisualStudio::filtreChange() {
	QString filtre = m_filtreLineEdit->text();
	if( filtre.isEmpty() ) 
		m_dirModel->setNameFilters( DEFAULT_PROJECT_FILTRE );
	else {
		QString extention = QFileInfo( filtre ).suffix();
		QString filename = QFileInfo( filtre ).fileName();
		if( extention.isEmpty() )
			m_dirModel->setNameFilters( 
				QStringList() 
					<< QString("*%1*.xsl").arg( filtre )
					<< QString("*%1*.js").arg( filtre )
					<< QString("*%1*.xml").arg( filtre )
					<< QString("*%1*.fws").arg( filtre )
				);
		else
			m_dirModel->setNameFilters( QStringList() << QString( "*%1*" ).arg( filename ) );
		
	}
	m_modelTimer->stop();
}

void XMLVisualStudio::on_m_filtreLineEdit_textChanged( QString filtre ) {
	Q_UNUSED( filtre );
	
	m_modelTimer->stop();
	m_modelTimer->start();
}

void XMLVisualStudio::on_m_projectDirectoryTreeView_doubleClicked( QModelIndex index ) {
	open( m_dirModel->filePath( index ) );
}

