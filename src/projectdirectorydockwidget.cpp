/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

// Xinx header
#include "projectdirectorydockwidget.h"
#include "private/p_projectdirectorydockwidget.h"
#include "globals.h"
#include "flattreeview.h"
#include "dirrcsmodel.h"
#include "exceptions.h"

// Qt header
#include <QFileInfo>
#include <QFile>
#include <QHeaderView>
#include <QMenu>
#include <QContextMenuEvent>

/* PrivateProjectDirectoryDockWidget */

PrivateProjectDirectoryDockWidget::PrivateProjectDirectoryDockWidget( ProjectDirectoryDockWidget * parent ) : 
	m_selectedUpdateAction(0), m_selectedCommitAction(0), m_selectedAddAction(0), m_selectedRemoveAction(0),
	m_selectedCompareWithHeadAction(0), m_selectedCompareWithStdAction(0), m_selectedCompareAction(0), 
	m_dirModel(0), m_flatModel(0), m_iconProvider(0), m_project(0), m_parent( parent ) {
	XINX_TRACE( "PrivateProjectDirectoryDockWidget", QString( "( %1 )" ).arg( (unsigned long)parent, 0, 16 ) );
			
	QWidget * contentWidget = new QWidget( m_parent );
	m_projectDirWidget = new Ui::ProjectDirectoryWidget();
	m_projectDirWidget->setupUi( contentWidget );
	m_parent->setWidget( contentWidget );

	m_modelTimer = new QTimer( this );
	m_modelTimer->setInterval( 500 );
	connect( m_modelTimer, SIGNAL(timeout()), this, SLOT(filtreChange()) );
	
	m_projectDirWidget->m_projectDirectoryTreeView->header()->hide();
	m_projectDirWidget->m_projectDirectoryTreeView->installEventFilter( this );
	
	connect( parent, SIGNAL(visibilityChanged(bool)), m_projectDirWidget->m_filtreLineEdit, SLOT(setFocus()) );
	connect( m_projectDirWidget->m_filtreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_m_filtreLineEdit_textChanged(QString)) );
	connect( m_projectDirWidget->m_projectDirectoryTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_m_projectDirectoryTreeView_doubleClicked(QModelIndex)) );
}

PrivateProjectDirectoryDockWidget::~PrivateProjectDirectoryDockWidget() {
	XINX_TRACE( "~PrivateProjectDirectoryDockWidget", "()" );
	
}

void PrivateProjectDirectoryDockWidget::filtreChange() {
	XINX_TRACE( "PrivateProjectDirectoryDockWidget::filtreChange", "()" );

	XINX_ASSERT( m_dirModel );
	m_modelTimer->stop();
	
	QString filtre = m_projectDirWidget->m_filtreLineEdit->text();
	if( filtre.isEmpty() ) {
		m_dirModel->setNameFilters( DEFAULT_PROJECT_FILTRE );
		if( m_projectDirWidget->m_flatListBtn->isChecked() )
			m_projectDirWidget->m_flatListBtn->click();
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
		if( ! m_projectDirWidget->m_flatListBtn->isChecked() )
			m_projectDirWidget->m_flatListBtn->click();
	}
}

bool PrivateProjectDirectoryDockWidget::eventFilter( QObject *obj, QEvent *event ) {
	XINX_TRACE( "PrivateProjectDirectoryDockWidget::eventFilter", QString( "( %1, %2 )" ).arg( (unsigned long)obj, 0, 16 ).arg( (unsigned long)event, 0, 16 ) );

	if ( ( obj == m_projectDirWidget->m_projectDirectoryTreeView ) && ( event->type() == QEvent::ContextMenu ) ) {
		int nbSelected = m_projectDirWidget->m_projectDirectoryTreeView->selectionModel()->selectedRows().size(); 
		QMenu *menu = new QMenu( m_projectDirWidget->m_projectDirectoryTreeView );

		if( nbSelected == 1 ) {
			menu->addAction( m_selectedCompareWithStdAction );
		} else
		if( nbSelected == 2 ) {
			menu->addAction( m_selectedCompareAction );
			if( m_parent->rcs() )
				menu->addSeparator();
		}
			
		if( m_parent->rcs() ) {
			if( m_projectDirWidget->m_projectDirectoryTreeView->selectionModel()->selectedRows().size() == 1 ) {
				menu->addAction( m_selectedCompareWithHeadAction );
				menu->addSeparator();
			}
			menu->addAction( m_selectedUpdateAction );
			menu->addAction( m_selectedCommitAction );
			menu->addAction( m_selectedAddAction );
			menu->addAction( m_selectedRemoveAction );
		}
		menu->exec( static_cast<QContextMenuEvent*>( event )->globalPos() );
		delete menu;
	}
	return QObject::eventFilter( obj, event );
}

void PrivateProjectDirectoryDockWidget::on_m_filtreLineEdit_textChanged( QString filtre ) {
	Q_UNUSED( filtre );
	XINX_TRACE( "PrivateProjectDirectoryDockWidget::on_m_filtreLineEdit_textChanged", QString( "( %1 )" ).arg( filtre ) );

	XINX_ASSERT( m_dirModel );

	m_modelTimer->stop();
	m_modelTimer->start();
}

void PrivateProjectDirectoryDockWidget::on_m_projectDirectoryTreeView_doubleClicked( QModelIndex index ) {
	XINX_TRACE( "PrivateProjectDirectoryDockWidget::on_m_projectDirectoryTreeView_doubleClicked", "( index )" );

	QModelIndex idx = index;
	if( m_flatModel ) 
		idx = qobject_cast<FlatModel*>( m_flatModel )->mappingToSource( index );
	if( idx.isValid() && (! m_dirModel->isDir( idx )) )
		emit m_parent->open( m_dirModel->filePath( idx ) );
}

/* ProjectDirectoryDockWidget */
	
ProjectDirectoryDockWidget::ProjectDirectoryDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	XINX_TRACE( "ProjectDirectoryDockWidget", QString( "( %1, %2, flags )" ).arg( title ).arg( (unsigned long)parent, 0, 16 ) );

	d = new PrivateProjectDirectoryDockWidget( this );
}

ProjectDirectoryDockWidget::ProjectDirectoryDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	XINX_TRACE( "ProjectDirectoryDockWidget", QString( "( %1, flags )" ).arg( (unsigned long)parent, 0, 16 ) );

	d = new PrivateProjectDirectoryDockWidget( this );	
}

ProjectDirectoryDockWidget::~ProjectDirectoryDockWidget() {
	XINX_TRACE( "~ProjectDirectoryDockWidget", "()" );

	delete d;
}

void ProjectDirectoryDockWidget::setGlobalUpdateAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setGlobalUpdateAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_projectDirWidget->m_updateProjectBtn->setDefaultAction( action );
}

void ProjectDirectoryDockWidget::setGlobalCommitAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setGlobalCommitAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_projectDirWidget->m_commitProjectBtn->setDefaultAction( action );
}

void ProjectDirectoryDockWidget::setSelectedUpdateAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setSelectedUpdateAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_selectedUpdateAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCommitAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setSelectedCommitAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_selectedCommitAction = action;
}

void ProjectDirectoryDockWidget::setSelectedAddAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setSelectedAddAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_selectedAddAction = action;
}

void ProjectDirectoryDockWidget::setSelectedRemoveAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setSelectedRemoveAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_selectedRemoveAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCompareWithHeadAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setSelectedCompareWithHeadAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_selectedCompareWithHeadAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCompareWithStdAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setSelectedCompareWithStdAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_selectedCompareWithStdAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCompareAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setSelectedCompareAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_selectedCompareAction = action;
}

void ProjectDirectoryDockWidget::setToggledViewAction( QAction * action ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setToggledViewAction", QString( "( %1 )" ).arg( (unsigned long)action, 0, 16 ) );

	d->m_projectDirWidget->m_flatListBtn->setDefaultAction( action );
}
	
bool ProjectDirectoryDockWidget::isViewFlat() {
	XINX_TRACE( "ProjectDirectoryDockWidget::isViewFlat", "()" );

	return d->m_projectDirWidget->m_flatListBtn->isChecked();
}

QStringList ProjectDirectoryDockWidget::selectedFiles() {
	XINX_TRACE( "ProjectDirectoryDockWidget::selectedFiles", "()" );

	QStringList paths;
	QModelIndexList list = d->m_projectDirWidget->m_projectDirectoryTreeView->selectionModel()->selectedRows();
	if( d->m_flatModel ) {
		QModelIndexList indexList = list;
		list.clear();
		foreach( QModelIndex index, indexList ) {
			list << static_cast<FlatModel*>( d->m_flatModel )->mappingToSource( index );
		}
	}
	foreach( QModelIndex index, list )
		paths << d->m_dirModel->filePath( index );
	return paths;
}

void ProjectDirectoryDockWidget::toggledView() {
	XINX_TRACE( "ProjectDirectoryDockWidget::toggledView", "()" );

	toggledView( ! d->m_projectDirWidget->m_flatListBtn->isChecked() );
}

void ProjectDirectoryDockWidget::toggledView( bool flat ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::toggledView", QString( "( %1 )" ).arg( flat ) );

	if( flat ) {
		d->m_flatModel = new FlatModel( d->m_dirModel, d->m_dirModel->index( d->m_project->projectPath() ) );
		d->m_projectDirWidget->m_projectDirectoryTreeView->setModel( d->m_flatModel );
		d->m_projectDirWidget->m_projectDirectoryTreeView->setRootIndex( QModelIndex() );
		d->m_projectDirWidget->m_projectDirectoryTreeView->setRootIsDecorated( false );
	} else  {
		d->m_projectDirWidget->m_projectDirectoryTreeView->setModel( d->m_dirModel );
		d->m_projectDirWidget->m_projectDirectoryTreeView->setRootIndex( d->m_dirModel->index( d->m_project->projectPath() ) );
		d->m_projectDirWidget->m_projectDirectoryTreeView->setRootIsDecorated( true );
		delete d->m_flatModel;
		d->m_flatModel = NULL;
	}
	
	QAbstractItemModel * model = d->m_projectDirWidget->m_projectDirectoryTreeView->model();
	for( int i = 2; i < model->columnCount(); i++ )
		d->m_projectDirWidget->m_projectDirectoryTreeView->hideColumn( i );
	d->m_projectDirWidget->m_projectDirectoryTreeView->header()->setResizeMode( QHeaderView::Fixed );
	d->m_projectDirWidget->m_projectDirectoryTreeView->header()->resizeSection( 0, 1024 );
}

void ProjectDirectoryDockWidget::setProjectPath( XSLProject * project ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::setProjectPath", QString( "( %1 )" ).arg( (unsigned long)project, 0, 16 ) );

	if( d->m_projectDirWidget->m_flatListBtn->isChecked() )
		d->m_projectDirWidget->m_flatListBtn->click();
	d->m_projectDirWidget->m_filtreLineEdit->setText( "" );
	d->m_modelTimer->stop();
	d->m_projectDirWidget->m_projectDirectoryTreeView->setModel( NULL );
	delete d->m_dirModel;
	d->m_dirModel = NULL;
	delete d->m_iconProvider;
	d->m_iconProvider = NULL;
		
	d->m_project = project;
	
	if( project ) {
		d->m_dirModel = new DirRCSModel( DEFAULT_PROJECT_FILTRE, DEFAULT_PROJECT_FILTRE_OPTIONS, QDir::DirsFirst, d->m_projectDirWidget->m_projectDirectoryTreeView );
		d->m_iconProvider = new IconProjectProvider();
		d->m_dirModel->setIconProvider( d->m_iconProvider );

		d->m_projectDirWidget->m_projectDirectoryTreeView->setModel( d->m_dirModel );
		d->m_projectDirWidget->m_projectDirectoryTreeView->header()->setResizeMode( QHeaderView::Fixed );
		d->m_projectDirWidget->m_projectDirectoryTreeView->header()->resizeSection( 0, 1024 );

		for(int i = 2; i < d->m_dirModel->columnCount(); i++ )
			d->m_projectDirWidget->m_projectDirectoryTreeView->hideColumn( i );
		d->m_projectDirWidget->m_projectDirectoryTreeView->setRootIndex( d->m_dirModel->index( d->m_project->projectPath() ) );
	}
}

void ProjectDirectoryDockWidget::refreshPath( const QString & path ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::refreshPath", QString( "( %1 )" ).arg( path ) );

	XINX_ASSERT( path.contains( d->m_project->projectPath() ) );

	if( d->m_dirModel ) {
		QModelIndex index = d->m_dirModel->index( path );
		d->m_dirModel->refresh( index );
	}
}

bool ProjectDirectoryDockWidget::removeFile( const QString & path ) {
	XINX_TRACE( "ProjectDirectoryDockWidget::removeFile", QString( "( %1 )" ).arg( path ) );

	XINX_ASSERT( path.contains( d->m_project->projectPath() ) );
	
	if( d->m_dirModel ) {
		QModelIndex index = d->m_dirModel->index( path );
		if( index.isValid() ) {
			d->m_dirModel->setReadOnly( false );
			bool result = d->m_dirModel->remove( index );
			d->m_dirModel->setReadOnly( true );
			return result;
		} else
			return false;
	} else
		return QFile::remove( path );
}

RCS * ProjectDirectoryDockWidget::rcs() {
	XINX_TRACE( "ProjectDirectoryDockWidget::rcs", "()" );

	if( qobject_cast<DirRCSModel*>( d->m_dirModel ) )
		return qobject_cast<DirRCSModel*>( d->m_dirModel )->rcs();
	else
		return NULL;
}
