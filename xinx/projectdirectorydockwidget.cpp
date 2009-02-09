/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "projectdirectorydockwidget.h"
#include <xinxcore.h>
#include <xinxpluginsloader.h>
#include "flattreeview.h"
#include "dirrcsmodel.h"
#include <exceptions.h>
#include <xinxconfig.h>
#include <xinxproject.h>
#include "iconprojectprovider.h"

// Qt header
#include <QTimer>
#include <QDirModel>
#include <QAbstractItemModel>
#include <QFileInfo>
#include <QFile>
#include <QHeaderView>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMetaObject>
#include <QClipboard>

/* ProjectDirectoryDockWidget */

ProjectDirectoryDockWidget::ProjectDirectoryDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ), m_iconProvider( 0 ) {
	init();
}

ProjectDirectoryDockWidget::ProjectDirectoryDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ), m_iconProvider( 0 ) {
	init();
}

void ProjectDirectoryDockWidget::init() {
	m_copyFileNameAction = new QAction( tr("&Copy filename to Clipboard"), this );
	m_copyPathNameAction = new QAction( tr("C&opy path to clipboard"), this );

	QWidget * contentWidget = new QWidget( this );
	m_projectDirWidget = new Ui::ProjectDirectoryWidget();
	m_projectDirWidget->setupUi( contentWidget );
	setWidget( contentWidget );

	m_modelTimer = new QTimer( this );
	connect( m_modelTimer, SIGNAL(timeout()), this, SLOT(filtreChange()) );

	m_projectDirWidget->m_projectDirectoryTreeView->header()->hide();
	m_projectDirWidget->m_projectDirectoryTreeView->installEventFilter( this );

	connect( m_copyFileNameAction, SIGNAL(triggered()), this, SLOT(copyFileNameTriggered()) );
	connect( m_copyPathNameAction, SIGNAL(triggered()), this, SLOT(copyPathNameTriggered()) );
	connect( this, SIGNAL(visibilityChanged(bool)), m_projectDirWidget->m_filtreLineEdit, SLOT(setFocus()) );
	connect( this, SIGNAL(visibilityChanged(bool)), m_projectDirWidget->m_filtreLineEdit, SLOT(selectAll()) );
	connect( m_projectDirWidget->m_filtreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(on_m_filtreLineEdit_textChanged(QString)) );
	connect( m_projectDirWidget->m_clearToolButton, SIGNAL(clicked()), this, SLOT(on_m_filtreLineEdit_returnPressed()) );
	connect( m_projectDirWidget->m_filtreLineEdit, SIGNAL(returnPressed()), this, SLOT(on_m_filtreLineEdit_returnPressed()) );
	connect( m_projectDirWidget->m_projectDirectoryTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_m_projectDirectoryTreeView_doubleClicked(QModelIndex)) );
	connect( m_projectDirWidget->m_prefixComboBox, SIGNAL(activated(QString)), this, SLOT(on_m_prefixComboBox_activated(QString)) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(projectChange()) );
}


ProjectDirectoryDockWidget::~ProjectDirectoryDockWidget() {

}

void ProjectDirectoryDockWidget::setGlobalUpdateAction( QAction * action ) {
	m_projectDirWidget->m_updateProjectBtn->setDefaultAction( action );
}

void ProjectDirectoryDockWidget::setGlobalCommitAction( QAction * action ) {
	m_projectDirWidget->m_commitProjectBtn->setDefaultAction( action );
}

void ProjectDirectoryDockWidget::setSelectedUpdateAction( QAction * action ) {
	m_selectedUpdateAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCommitAction( QAction * action ) {
	m_selectedCommitAction = action;
}

void ProjectDirectoryDockWidget::setSelectedAddAction( QAction * action ) {
	m_selectedAddAction = action;
}

void ProjectDirectoryDockWidget::setSelectedRemoveAction( QAction * action ) {
	m_selectedRemoveAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCompareWithHeadAction( QAction * action ) {
	m_selectedCompareWithHeadAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCompareWithStdAction( QAction * action ) {
	m_selectedCompareWithStdAction = action;
}

void ProjectDirectoryDockWidget::setSelectedCompareAction( QAction * action ) {
	m_selectedCompareAction = action;
}

void ProjectDirectoryDockWidget::setToggledViewAction( QAction * action ) {
	m_projectDirWidget->m_flatListBtn->setDefaultAction( action );
}

bool ProjectDirectoryDockWidget::isViewFlat() {
	return m_projectDirWidget->m_flatListBtn->isChecked();
}

QStringList ProjectDirectoryDockWidget::selectedFiles() {
	QStringList paths;
	QModelIndexList list = m_projectDirWidget->m_projectDirectoryTreeView->selectionModel()->selectedRows();
	if( m_flatModel ) {
		QModelIndexList indexList = list;
		list.clear();
		foreach( const QModelIndex & index, indexList ) {
			list << static_cast<FlatModel*>( m_flatModel.data() )->mappingToSource( index );
		}
	}
	foreach( const QModelIndex & index, list )
		paths << m_dirModel->filePath( index );
	return paths;
}

void ProjectDirectoryDockWidget::toggledView() {
	toggledView( ! m_projectDirWidget->m_flatListBtn->isChecked() );
}

void ProjectDirectoryDockWidget::toggledView( bool flat ) {
	if( flat ) {
		m_flatModel = new FlatModel( m_dirModel, m_dirModel->index( m_project->projectPath() ) );
		m_projectDirWidget->m_projectDirectoryTreeView->setModel( m_flatModel );
		m_projectDirWidget->m_projectDirectoryTreeView->setRootIndex( QModelIndex() );
		m_projectDirWidget->m_projectDirectoryTreeView->setRootIsDecorated( false );
	} else  {
		m_projectDirWidget->m_projectDirectoryTreeView->setModel( m_dirModel );
		m_projectDirWidget->m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( m_project->projectPath() ) );
		m_projectDirWidget->m_projectDirectoryTreeView->setRootIsDecorated( true );
		delete m_flatModel;
	}

	QAbstractItemModel * model = m_projectDirWidget->m_projectDirectoryTreeView->model();
	for( int i = 2; i < model->columnCount(); i++ )
		m_projectDirWidget->m_projectDirectoryTreeView->hideColumn( i );
	m_projectDirWidget->m_projectDirectoryTreeView->header()->setResizeMode( QHeaderView::Fixed );
	m_projectDirWidget->m_projectDirectoryTreeView->header()->resizeSection( 0, 1024 );
}

void ProjectDirectoryDockWidget::setProjectPath( XinxProject * project ) {
	if( m_projectDirWidget->m_flatListBtn->isChecked() )
		m_projectDirWidget->m_flatListBtn->click();
	m_projectDirWidget->m_filtreLineEdit->setText( "" );
	m_modelTimer->stop();
	m_projectDirWidget->m_projectDirectoryTreeView->setModel( NULL );
	delete m_dirModel;
	delete m_iconProvider; m_iconProvider = NULL;
	m_projectDirWidget->m_prefixComboBox->clear();

	m_project = project;

	if( project ) {
		m_dirModel = new DirRCSModel( XinxPluginsLoader::self()->managedFilters(), DEFAULT_PROJECT_FILTRE_OPTIONS, QDir::DirsFirst, m_projectDirWidget->m_projectDirectoryTreeView );
		m_iconProvider = new IconProjectProvider();
		m_dirModel->setIconProvider( m_iconProvider );

		m_projectDirWidget->m_projectDirectoryTreeView->setModel( m_dirModel );
		m_projectDirWidget->m_projectDirectoryTreeView->header()->setResizeMode( QHeaderView::Fixed );
		m_projectDirWidget->m_projectDirectoryTreeView->header()->resizeSection( 0, 1024 );

		for(int i = 2; i < m_dirModel->columnCount(); i++ )
			m_projectDirWidget->m_projectDirectoryTreeView->hideColumn( i );
		m_projectDirWidget->m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( m_project->projectPath() ) );

		m_projectDirWidget->m_prefixComboBox->addItems( project->specifiquePrefixes() );
	}
}

void ProjectDirectoryDockWidget::refreshPath( const QString & path ) {
	if( ! ( m_project && path.contains( m_project->projectPath() )) ) return;

	if( m_dirModel ) {
		QModelIndex index = m_dirModel->index( path );
		m_dirModel->refresh( index );
	}
}

bool ProjectDirectoryDockWidget::removeFile( const QString & path ) {
	Q_ASSERT( path.contains( m_project->projectPath() ) );

	if( m_dirModel ) {
		QModelIndex index = m_dirModel->index( path );
		if( index.isValid() ) {
			m_dirModel->setReadOnly( false );
			bool result = m_dirModel->remove( index );
			m_dirModel->setReadOnly( true );
			return result;
		} else
			return false;
	} else
		return QFile::remove( path );
}

RCS * ProjectDirectoryDockWidget::rcs() {
	if( qobject_cast<DirRCSModel*>( m_dirModel ) )
		return qobject_cast<DirRCSModel*>( m_dirModel )->rcs();
	else
		return NULL;
}

void ProjectDirectoryDockWidget::projectChange() {
	m_projectDirWidget->m_prefixComboBox->clear();
	if( m_project ) {
		if( m_flatModel ) {
			m_projectDirWidget->m_projectDirectoryTreeView->setModel( NULL );
			delete m_flatModel;
			m_flatModel = new FlatModel( m_dirModel, m_dirModel->index( m_project->projectPath() ) );
			m_projectDirWidget->m_projectDirectoryTreeView->setModel( m_flatModel );
			m_projectDirWidget->m_projectDirectoryTreeView->header()->setResizeMode( QHeaderView::Fixed );
			m_projectDirWidget->m_projectDirectoryTreeView->header()->resizeSection( 0, 1024 );
		} else {
			m_projectDirWidget->m_projectDirectoryTreeView->setRootIndex( m_dirModel->index( m_project->projectPath() ) );
		}

		m_projectDirWidget->m_prefixComboBox->addItems( m_project->specifiquePrefixes() );
		m_projectDirWidget->m_prefixComboBox->setCurrentIndex(
				m_projectDirWidget->m_prefixComboBox->findText( m_project->specifiquePrefix() ) );
	}
}

void ProjectDirectoryDockWidget::copyFileNameTriggered() {
	QStringList list = selectedFiles();
	QString names;
	foreach( const QString & name, list ) {
		names += QFileInfo( name ).fileName() + "\n";
	}
	qApp->clipboard()->setText( names );
}

void ProjectDirectoryDockWidget::copyPathNameTriggered() {
	QStringList list = selectedFiles();
	qApp->clipboard()->setText( list.join( "\n" ) );
}

void ProjectDirectoryDockWidget::on_m_prefixComboBox_activated( QString prefix ) {
	m_project->setSpecifiquePrefix( prefix );
}

void ProjectDirectoryDockWidget::filtreChange() {
	Q_ASSERT( m_dirModel );
	m_modelTimer->stop();

	QString filtre = m_projectDirWidget->m_filtreLineEdit->text();
	if( filtre.isEmpty() ) {
		m_dirModel->setNameFilters( XinxPluginsLoader::self()->managedFilters() );
		if( m_projectDirWidget->m_flatListBtn->isChecked() )
			m_projectDirWidget->m_flatListBtn->click();
	} else {
		QString extention = QFileInfo( filtre ).completeSuffix();
		QString filename = QFileInfo( filtre ).baseName();

		QStringList filters;
		foreach( const QString & pluginFilter, XinxPluginsLoader::self()->managedFilters() ) {
			QString pluginFilename  = QFileInfo( pluginFilter ).baseName();
			QString pluginExtention = QFileInfo( pluginFilter ).completeSuffix();
			if( ! filename.isEmpty() )
				pluginFilename.replace( '*', "*" + filename + "*" );
			if( ! extention.isEmpty() )
				pluginExtention.replace( '*', "*" + extention + "*" );

			if( ! ( extention.isEmpty() || pluginExtention.contains( extention ) ) ) continue;

			filters += (pluginFilename + "." + pluginExtention);
		}
		if( filters.isEmpty() )
			filters += ( "*" + filename + "*." + extention );

		m_dirModel->setNameFilters( filters );

		if( ! m_projectDirWidget->m_flatListBtn->isChecked() )
			m_projectDirWidget->m_flatListBtn->click();
	}
}

bool ProjectDirectoryDockWidget::eventFilter( QObject *obj, QEvent *event ) {
	if ( ( obj == m_projectDirWidget->m_projectDirectoryTreeView ) && ( event->type() == QEvent::ContextMenu ) ) {
		int nbSelected = m_projectDirWidget->m_projectDirectoryTreeView->selectionModel()->selectedRows().size();
		QMenu *menu = new QMenu( m_projectDirWidget->m_projectDirectoryTreeView );

		if( nbSelected == 1 ) {
			menu->addAction( m_selectedCompareWithStdAction );
		} else
		if( nbSelected == 2 ) {
			menu->addAction( m_selectedCompareAction );
			if( rcs() )
				menu->addSeparator();
		}

		if( rcs() ) {
			if( m_projectDirWidget->m_projectDirectoryTreeView->selectionModel()->selectedRows().size() == 1 ) {
				menu->addAction( m_selectedCompareWithHeadAction );
				menu->addSeparator();
			}
			menu->addAction( m_selectedUpdateAction );
			menu->addAction( m_selectedCommitAction );
			menu->addAction( m_selectedAddAction );
			menu->addAction( m_selectedRemoveAction );
			menu->addSeparator();
		}
		menu->addAction( m_copyFileNameAction );
		menu->addAction( m_copyPathNameAction );

		menu->exec( static_cast<QContextMenuEvent*>( event )->globalPos() );
		delete menu;
	}
	return QObject::eventFilter( obj, event );
}

void ProjectDirectoryDockWidget::on_m_filtreLineEdit_returnPressed() {
	int timeout = XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout;
	if( timeout == 0 ) {
		filtreChange();
	}
}

void ProjectDirectoryDockWidget::on_m_filtreLineEdit_textChanged( QString filtre ) {
	Q_UNUSED( filtre );
	Q_ASSERT( m_dirModel );

	m_modelTimer->stop();
	int timeout = XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout;
	if( timeout > 0 ) {
		m_modelTimer->setInterval( timeout );
		m_modelTimer->start();
	}
}

void ProjectDirectoryDockWidget::on_m_projectDirectoryTreeView_doubleClicked( QModelIndex index ) {
	QModelIndex idx = index;
	if( m_flatModel )
		idx = qobject_cast<FlatModel*>( m_flatModel )->mappingToSource( index );
	if( idx.isValid() && (! m_dirModel->isDir( idx )) )
		emit open( m_dirModel->filePath( idx ) );
}
