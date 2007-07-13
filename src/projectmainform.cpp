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
#include <QContextMenuEvent>
#include <QMenu>

#include <assert.h>

#include "globals.h"
#include "xmlvisualstudio.h" 
#include "xinxconfig.h"
#include "xslproject.h"
#include "projectpropertyimpl.h"
#include "editor.h"

#include "rcs_cvs.h"
#include "rcslogdialogimpl.h"

#include "commitmessagedialogimpl.h"

#include "flattreeview.h"

/* DirRCSModel */

class DirRCSModel : public QDirModel {
	Q_OBJECT
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

bool XMLVisualStudio::eventFilter( QObject *obj, QEvent *event ) {
	if ( ( obj == m_projectDirectoryTreeView ) && ( event->type() == QEvent::ContextMenu ) ) {
		QMenu *menu = new QMenu( m_projectDirectoryTreeView );
		menu->addAction( m_selectedUpdateFromRCSAct );
		menu->addAction( m_selectedCommitToRCSAct );
		menu->addAction( m_selectedAddToRCSAct );
		menu->addAction( m_selectedRemoveFromRCSAct );
		menu->exec( static_cast<QContextMenuEvent*>(event)->globalPos() );
		delete menu;
	}
	return QObject::eventFilter( obj, event );
}

void XMLVisualStudio::rcsLogTerminated() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		rcs->disconnect();
	}
}

void XMLVisualStudio::createProjectPart() {
	m_lastProjectOpenedPlace = QDir::currentPath();
	m_dirModel = NULL;
	m_flatModel = NULL;
	m_modelTimer = new QTimer( this );
	m_modelTimer->setInterval( 500 );
	connect( m_modelTimer, SIGNAL(timeout()), this, SLOT(filtreChange()) );
	m_projectDirectoryTreeView->header()->hide();
	m_projectDirectoryTreeView->installEventFilter( this );

	m_rcslogDialog = new RCSLogDialogImpl( this );
	
	m_updateProjectBtn->setDefaultAction( m_globalUpdateFromRCSAct );
	m_commitProjectBtn->setDefaultAction( m_globalCommitToRCSAct );
	m_flatListBtn->setDefaultAction( m_toggledFlatView );
	
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

	closeProject( true, global.m_xinxConfig->saveSessionByDefault() );
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
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open a project"), global.m_xinxConfig->xinxProjectPath(), "Projet (*.prj)" );
	if( ! fileName.isEmpty() )
		openProject( fileName );	
}

void XMLVisualStudio::openProject( const QString & filename ) {
	assert( ! filename.isEmpty() );

	if( global.m_project ) 
		closeProject( true, global.m_xinxConfig->saveSessionByDefault() ); 
	else 
		on_m_closeAllAct_triggered();
		
	global.m_xinxConfig->recentProjectFiles().removeAll( filename );

	try {
		global.m_project      	 = new XSLProject( filename );
		m_lastProjectOpenedPlace = QFileInfo( filename ).absolutePath();
		m_lastPlace              = global.m_project->projectPath();

		if( global.m_project->projectType() == XSLProject::SERVICES )
			setWebServicesView( true );

		global.m_xinxConfig->recentProjectFiles().prepend( filename );
     
		while( global.m_xinxConfig->recentProjectFiles().size() > MAXRECENTFILES )
			global.m_xinxConfig->recentProjectFiles().removeLast();

		m_dirModel = new DirRCSModel( DEFAULT_PROJECT_FILTRE, DEFAULT_PROJECT_FILTRE_OPTIONS, QDir::DirsFirst, m_projectDirectoryTreeView );
		m_iconProvider = new IconProjectProvider();
		m_dirModel->setIconProvider( m_iconProvider );

		m_projectDirectoryTreeView->setModel( m_dirModel );
		for(int i = 1; i < m_dirModel->columnCount(); i++ )
			m_projectDirectoryTreeView->hideColumn( i );
		m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( global.m_project->projectPath() ) );

		m_tabEditors->setUpdatesEnabled( false );
	
		QDomElement element = global.m_project->sessionNode().firstChildElement( "editor" );
		while( ! element.isNull() ) {
			Editor * editor = m_tabEditors->newFileEditor( element.attribute( "filename" ) );
			editor->deserializeEditor( element );
			
			element = element.nextSiblingElement( "editor" );
		}
		m_tabEditors->setUpdatesEnabled( true );

		setCurrentProject( filename );

	} catch( XSLProjectException ) {
		
	}
	updateActions();
	updateRecentProjects();
	updateRecentFiles();
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
	int numRecentFiles = qMin( global.m_xinxConfig->recentProjectFiles().size(), MAXRECENTFILES );

	for( int i = 0; i < numRecentFiles; i++ ) {
		QString text = tr("&%1 %2").arg(i + 1).arg( QFileInfo( global.m_xinxConfig->recentProjectFiles()[i] ).fileName() );
		m_recentProjectActs[i]->setText( text );
		m_recentProjectActs[i]->setData( global.m_xinxConfig->recentProjectFiles()[i] );
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
	if( filtre.isEmpty() ) {
		m_dirModel->setNameFilters( DEFAULT_PROJECT_FILTRE );
		m_toggledFlatView->setChecked( false );
	} else {
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
		m_toggledFlatView->setChecked( true );
	}
	m_modelTimer->stop();
}

void XMLVisualStudio::on_m_filtreLineEdit_textChanged( QString filtre ) {
	Q_UNUSED( filtre );
	
	m_modelTimer->stop();
	m_modelTimer->start();
}

void XMLVisualStudio::on_m_projectDirectoryTreeView_doubleClicked( QModelIndex index ) {
	QModelIndex idx = index;
	if( m_flatModel ) 
		idx = qobject_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
	if( ! m_dirModel->fileInfo( idx ).isDir() )
		open( m_dirModel->filePath( idx ) );
}

void XMLVisualStudio::on_m_globalUpdateFromRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();
		rcs->update( QStringList() << global.m_project->projectPath() );
		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_globalCommitToRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		CommitMessageDialogImpl dlg;
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();

		dlg.setFilesOperation( rcs->operations( QStringList() << global.m_project->projectPath() ) );
		if( ! dlg.exec() ) return ;
		QString message = dlg.messages();

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();

		rcs->commit( dlg.filesOperation(), message );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedUpdateFromRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();
		
		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list )
			paths << m_dirModel->filePath( index );
		rcs->update( paths );
		
		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedCommitToRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		CommitMessageDialogImpl dlg;
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();

		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list )
			paths << m_dirModel->filePath( index );

		dlg.setFilesOperation( rcs->operations( paths ) );
		if( ! dlg.exec() ) return ;
		QString message = dlg.messages();

		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();

		rcs->commit( dlg.filesOperation(), message );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedAddToRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();

		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list )
			paths << m_dirModel->filePath( index );
		rcs->add( paths );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_selectedRemoveFromRCSAct_triggered() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) ) {
		RCS * rcs = qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
		
		connect( rcs, SIGNAL(log(RCS::rcsLog,QString)), m_rcslogDialog, SLOT(log(RCS::rcsLog,QString)) );
		connect( rcs, SIGNAL(operationTerminated()), m_rcslogDialog, SLOT(logTerminated()) );
		connect( rcs, SIGNAL(operationTerminated()), this, SLOT(rcsLogTerminated()) );
		connect( m_rcslogDialog, SIGNAL(abort()), rcs, SLOT(abort()) );
		m_rcslogDialog->init();
		m_rcslogDialog->show();

		m_rcslogDialog->init();

		QStringList paths;
		QModelIndexList list = m_projectDirectoryTreeView->selectionModel()->selectedRows();
		if( m_flatModel ) {
			QModelIndexList trf = list;
			list.clear();
			foreach( QModelIndex index, trf ) {
				list << static_cast<FlatModel*>(m_flatModel)->mappingToSource( index );
			}
		}
		foreach( QModelIndex index, list ) {
			paths << m_dirModel->filePath( index );
			QFile::remove( m_dirModel->filePath( index ) );
		}
		rcs->remove( paths );

		m_rcslogDialog->exec();
	}
}

void XMLVisualStudio::on_m_toggledFlatView_toggled( bool value ) {
	if( value ) {
		m_flatModel = new FlatModel( m_dirModel, m_dirModel->index( global.m_project->projectPath() ) );
		m_projectDirectoryTreeView->setModel( m_flatModel );
		m_projectDirectoryTreeView->setRootIndex( QModelIndex() );
		for(int i = 1; i < m_flatModel->columnCount(); i++ )
			m_projectDirectoryTreeView->hideColumn( i );
		m_projectDirectoryTreeView->setRootIsDecorated( false );
	} else  {
		m_projectDirectoryTreeView->setModel( m_dirModel );
		m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( global.m_project->projectPath() ) );
		for(int i = 1; i < m_dirModel->columnCount(); i++ )
			m_projectDirectoryTreeView->hideColumn( i );
		m_projectDirectoryTreeView->setRootIsDecorated( true );
		delete m_flatModel;
		m_flatModel = NULL;
	}
}


#include "projectmainform.moc"
