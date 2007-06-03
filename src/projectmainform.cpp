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
#include <QFileIconProvider>

#include <assert.h>

#include "globals.h"
#include "xmlvisualstudio.h" 
#include "xinxconfig.h"
#include "xslproject.h"
#include "projectpropertyimpl.h"
#include "editor.h"

#include "rcs_cvs.h"
#include "rcslogdialogimpl.h"

#define DEFAULT_PROJECT_FILTRE QStringList() << "*.xml" << "*.xsl" << "*.js" << "*.fws"
#define DEFAULT_PROJECT_FILTRE_OPTIONS QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot

/* DirRCSModel */

class DirRCSModel : public QDirModel {
public:
	DirRCSModel( const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent = 0 );
	DirRCSModel( QObject *parent = 0 );
	QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
	RCS * rcs();
private:
	RCS * m_rcs;
};

DirRCSModel::DirRCSModel( const QStringList & nameFilters, QDir::Filters filters, QDir::SortFlags sort, QObject * parent ) : QDirModel( nameFilters, filters, sort, parent ) {
	if( global.m_project && ( global.m_project->projectRCS() == XSLProject::CVS ) ) 
		m_rcs = new RCS_CVS();
	else
		m_rcs = NULL;
}

DirRCSModel::DirRCSModel(QObject *parent) : QDirModel(parent) {
	
}

RCS * DirRCSModel::rcs() { 
	return m_rcs;
}

QVariant DirRCSModel::data(const QModelIndex &index, int role) const {
	if (m_rcs && ( role == Qt::BackgroundRole && index.column() == 0 ) ) {
   		RCS::rcsState state = m_rcs->status( filePath(index) );
   		if( state == RCS::Unknown )
			return QBrush( Qt::gray );
   		if( state == RCS::LocallyModified )
			return QBrush( Qt::yellow );
   		if( state == RCS::LocallyAdded )
			return QBrush( Qt::green );
		if( ( state == RCS::UnresolvedConflict ) || ( state == RCS::FileHadConflictsOnMerge ) )
			return QBrush( Qt::red );
			
		return QDirModel::data(index, role);
	}

	return QDirModel::data(index, role);
}

/* IconProvider */

class IconProjectProvider : public QFileIconProvider {
public:
	IconProjectProvider();
	~IconProjectProvider();
	
	QIcon icon( const QFileInfo & info ) const;
};

IconProjectProvider::IconProjectProvider() : QFileIconProvider() {
	
}

IconProjectProvider::~IconProjectProvider() {
	
}
	
QIcon IconProjectProvider::icon( const QFileInfo & info ) const {
	if( info.suffix() == "xsl" ) {
		return QIcon( ":/typexsl.png" );
	} else
	if( info.suffix() == "xml" ) {
		return QIcon( ":/typexml.png" );
	} else
	if( info.suffix() == "js" ) {
		return QIcon( ":/typejs.png" );
	} else
	if( info.suffix() == "fws" ) {
		return QIcon( ":/typefws.png" );
	} else
		return QFileIconProvider::icon( info );
}

/* XMLVisualStudio */

void XMLVisualStudio::rcsLogTerminated() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		rcs->disconnect();
	}
}

void XMLVisualStudio::on_m_updateProjectBtn_clicked() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(updateTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(updateTerminated()), this, SLOT(rcsLogTerminated()) );
		m_rcslogDialog->init();
		m_rcslogDialog->show();
		rcs->update( global.m_project->projectPath() );
	}
}

void XMLVisualStudio::createProjectPart() {
	m_lastProjectOpenedPlace = QDir::currentPath();
	m_dirModel = NULL;
	m_modelTimer = new QTimer( this );
	m_modelTimer->setInterval( 500 );
	connect( m_modelTimer, SIGNAL(timeout()), this, SLOT(filtreChange()) );
	m_projectDirectoryTreeView->header()->hide();
	
	m_rcslogDialog = new RCSLogDialogImpl( this );
	
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
	
	updateRecentProjects();
}

void XMLVisualStudio::setupRecentFileMenu( QMenu * menu ) {
	m_recentFileSeparator = menu->addSeparator();
	for(int i = 0; i < MAXRECENTFILES; i++) {
		m_recentFileActs[i] = new QAction( this );
		m_recentFileActs[i]->setVisible( false );
		menu->addAction( m_recentFileActs[i] );
		connect( m_recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()) );
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

	closeProject( true, xinxConfig->saveSessionByDefault() );
	project->saveToFile( filename );
	delete project;
	
	openProject( filename );
}

void XMLVisualStudio::openRecentProject() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		openProject( action->data().toString() );	
}

void XMLVisualStudio::openRecentFile() {
	QAction * action = qobject_cast<QAction*>( sender() );
	if( action )
		open( action->data().toString() );	
}

void XMLVisualStudio::openProject() {
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open a project"), xinxConfig->xinxProjectPath(), "Projet (*.prj)" );
	if( ! fileName.isEmpty() )
		openProject( fileName );	
}

void XMLVisualStudio::openProject( const QString & filename ) {
	assert( ! filename.isEmpty() );

	if( global.m_project ) 
		closeProject( true, xinxConfig->saveSessionByDefault() ); 
	else 
		on_m_closeAllAct_triggered();
		
	global.m_project      = new XSLProject( filename );
	m_lastProjectOpenedPlace = QFileInfo( filename ).absolutePath();
	m_lastPlace              = global.m_project->projectPath();

	xinxConfig->recentProjectFiles().removeAll( filename );
	xinxConfig->recentProjectFiles().prepend( filename );
     
	while( xinxConfig->recentProjectFiles().size() > MAXRECENTFILES )
		xinxConfig->recentProjectFiles().removeLast();

	if( global.m_project->projectType() == XSLProject::SERVICES )
		setWebServicesView( true );


	m_dirModel = new DirRCSModel( DEFAULT_PROJECT_FILTRE, DEFAULT_PROJECT_FILTRE_OPTIONS, QDir::DirsFirst, m_projectDirectoryTreeView );
	m_iconProvider = new IconProjectProvider();
	m_dirModel->setIconProvider( m_iconProvider );

	m_projectDirectoryTreeView->setModel( m_dirModel );
	for(int i = 1; i < m_dirModel->columnCount(); i++ )
		m_projectDirectoryTreeView->hideColumn( i );
	m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( global.m_project->projectPath() ) );

	QDomElement element = global.m_project->sessionNode().firstChildElement( "editor" );
	while( ! element.isNull() ) {
		Editor * editor;
		if( element.attribute( "class" ) == "XMLFileEditor" ) 
			editor = m_tabEditors->newFileEditorXML() ;
		else
		if( element.attribute( "class" ) == "XSLFileEditor" ) 
			editor = m_tabEditors->newFileEditorXSL() ;
		else
		if( element.attribute( "class" ) == "JSFileEditor" ) 
			editor = m_tabEditors->newFileEditorJS() ;
		else
		if( element.attribute( "class" ) == "WebServicesEditor" ) 
			editor = m_tabEditors->newFileEditorWS() ;
		else
			editor = m_tabEditors->newFileEditorTxt() ;
		editor->deserializeEditor( element );
		
		element = element.nextSiblingElement( "editor" );
	}
	updateActions();
	updateRecentProjects();
	updateRecentFiles();

	setCurrentProject( filename );
	global.emitProjectChanged();
}

void XMLVisualStudio::saveProject() {
	assert( global.m_project != NULL );
	
	global.m_project->saveToFile();
}

void XMLVisualStudio::changeProjectProperty() {
	assert( global.m_project != NULL );
	
	ProjectPropertyImpl property ( this );
	property.loadFromProject( global.m_project );
	if( property.exec() ) {
		property.saveToProject( global.m_project );
		global.emitProjectChanged();
		refreshWebServicesList();
		saveProject();
	}
}

void XMLVisualStudio::on_m_closeProjectAct_triggered() {
	closeProject( true, false );
}

void XMLVisualStudio::on_m_closeProjectSessionAct_triggered() {
	closeProject( true, true );
}

void XMLVisualStudio::closeProject( bool closeAll, bool saveSession ) {
	if( ! global.m_project ) return;		
		
	global.m_project->clearSessionNode();
	for( int i = 0; i < m_tabEditors->count(); i++ ) {
		QDomElement node = global.m_project->sessionDocument().createElement( "editor" );
		m_tabEditors->editor( i )->serializeEditor( node, saveSession );
		global.m_project->sessionNode().appendChild( node );
	}
	saveProject();

	if( closeAll && ( ! saveSession ) ) on_m_closeAllAct_triggered(); else
	if( closeAll ) {
		for( int i = m_tabEditors->count() - 1; i >= 0; i-- ) 
			m_tabEditors->removeTab( i );	
		updateActions();
	}

	m_projectDirectoryTreeView->setModel( NULL );
	delete m_dirModel;
	m_dirModel = NULL;
	setWebServicesView( false );

	delete global.m_project;
	global.m_project = NULL;

	updateActions();
	updateRecentFiles();
	setCurrentProject( "" );
	global.emitProjectChanged();
}

void XMLVisualStudio::updateRecentProjects() {
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

void XMLVisualStudio::updateRecentFiles() {
	int numRecentFiles;
	if( global.m_project ) {
		numRecentFiles = qMin( global.m_project->lastOpenedFile().size(), MAXRECENTFILES );

		for( int i = 0; i < numRecentFiles; i++ ) {
			QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( global.m_project->lastOpenedFile()[i] ).fileName() );
			m_recentFileActs[i]->setText( text );
			m_recentFileActs[i]->setData( global.m_project->lastOpenedFile()[i] );
			m_recentFileActs[i]->setVisible( true );
		}
	} else 
		numRecentFiles = 0;
	
	for( int j = numRecentFiles; j < MAXRECENTFILES; j++ )
		m_recentFileActs[j]->setVisible(false);

	m_recentFileSeparator->setVisible( numRecentFiles > 0 );
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
	if( ! m_dirModel->fileInfo( index ).isDir() )
		open( m_dirModel->filePath( index ) );
}

