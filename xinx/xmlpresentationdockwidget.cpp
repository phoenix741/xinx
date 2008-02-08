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
#include "xmlpresentationdockwidget.h"
#include "private/p_xmlpresentationdockwidget.h"
#include "globals.h"
#include "xslproject.h"

// Qt header
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QDateTime>

/* PrivateXmlPresentationDockWidget */

PrivateXmlPresentationDockWidget::PrivateXmlPresentationDockWidget( XmlPresentationDockWidget * parent ) : m_model(0), m_sortFilterModel(0), m_watcher(0), m_parent( parent ) {
	qRegisterMetaType<QModelIndex>( "" );
	
	QWidget * contentWidget = new QWidget( m_parent );
	m_xmlPresentationWidget = new Ui::XmlPresentationWidget();
	m_xmlPresentationWidget->setupUi( contentWidget );
	m_parent->setWidget( contentWidget );
	
	m_timerTextChanged.setSingleShot( true );
	m_timerTextChanged.setInterval( 500 );
	
	initXmlPresentationCombo();
	connect( &global, SIGNAL(projectChanged()), this, SLOT(initXmlPresentationCombo()) );
	connect( m_xmlPresentationWidget->m_presentationComboBox, SIGNAL(activated(int)), this, SLOT(presentationActivated(int)) );
	connect( this, SIGNAL(finished()), this, SLOT(threadTerminated()) );
	connect( m_xmlPresentationWidget->m_filtreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged(QString)) );
	connect( &m_timerTextChanged, SIGNAL(timeout()), this, SLOT(filterTextChangedTimer()) );
	connect( m_xmlPresentationWidget->m_presentationTreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(adaptColumns()));
}


PrivateXmlPresentationDockWidget::~PrivateXmlPresentationDockWidget() {
	presentationActivated( 0 );
	delete m_xmlPresentationWidget;
}

void PrivateXmlPresentationDockWidget::adaptColumns() {
  	m_xmlPresentationWidget->m_presentationTreeView->resizeColumnToContents(0);
}


void PrivateXmlPresentationDockWidget::initXmlPresentationCombo() {
	m_xmlPresentationWidget->m_presentationComboBox->clear();
	presentationActivated( 0 );
	
	m_xmlPresentationWidget->m_presentationComboBox->addItem( tr("<No presentation file>") );
	m_xmlPresentationWidget->m_presentationComboBox->addItem( tr("<Choose an XML file ...>") );
	
	if( global.m_project ) {
		m_logPath = global.m_project->logProjectDirectory();
		QDir logDir( m_logPath );
		if( logDir.exists() ) {
			QStringList files = logDir.entryList( QStringList() << "Presentation_*.xml", QDir::Files | QDir::Readable );
			foreach( QString file, files ) {
				m_xmlPresentationWidget->m_presentationComboBox->addItem( file );
			}
		}
	}
}

void PrivateXmlPresentationDockWidget::presentationActivated( int index ) {
	if( index == 0 ) {
		delete m_watcher;
		m_watcher = NULL;
		m_xmlPresentationWidget->m_presentationTreeView->setModel( NULL );
		delete m_sortFilterModel;
		m_sortFilterModel = NULL;
		delete m_model;
		m_model = NULL;
		m_openingFile = QString();
		// Clean the tree
		m_xmlPresentationWidget->m_presentationComboBox->setToolTip( QString() );
	} else if( index == 1 ) {
		// Open a file
		QString name = QFileDialog::getOpenFileName( m_parent, 
			tr("Open a presentation file"),
			m_logPath,
			tr("Presentation XML File (*.xml)") );
		setComboToolTip( name );
		open( name );
	} else {
		// Open the file
		QString name = QDir( m_logPath ).absoluteFilePath( m_xmlPresentationWidget->m_presentationComboBox->itemText( index ) );
		setComboToolTip( name );
		open( name );
	}
}

void PrivateXmlPresentationDockWidget::setComboToolTip( const QString & filename ) {
	QFileInfo file( filename );
	char unite = ' ';
	double filesize = file.size();
	while ( filesize > 1024 ) {
		filesize /= 1024;
		switch( unite ) {
		case ' ' : unite = 'k'; break;
		case 'k' : unite = 'm'; break;
		case 'm' : unite = 'g'; break;
		case 'g' : unite = 't'; break;
		default: break;
		}
	}
	
	m_xmlPresentationWidget->m_presentationComboBox->setToolTip( 
			tr( "File name : %1\n"
				"Size : %2 %3b\n"
				"File date : %4" )	.arg( file.canonicalFilePath() )
									.arg( filesize, 0, 'f', 2 ).arg( unite )
									.arg( file.lastModified().toString() )
			);
}

void PrivateXmlPresentationDockWidget::open( const QString& filename ) {
	if( m_watcher ) m_watcher->desactivate();

	m_xmlPresentationWidget->m_presentationProgressBar->setValue( 0 );
	m_xmlPresentationWidget->m_presentationProgressBar->setRange( 0, 0 );
	m_xmlPresentationWidget->m_presentationComboBox->setEnabled( false );
	m_xmlPresentationWidget->m_clearToolButton->setEnabled( false );
	m_xmlPresentationWidget->m_filtreLineEdit->setEnabled( false );
	m_xmlPresentationWidget->m_presentationTreeView->setModel( NULL );

	m_openingFile = filename;
	m_threadAct = THREAD_OPENING;
	start( QThread::IdlePriority );
}

void PrivateXmlPresentationDockWidget::open() {
	if( isRunning() ) return;
	open( m_openingFile );
}

void PrivateXmlPresentationDockWidget::threadrun() {
	if( m_threadAct == THREAD_OPENING ) {
		QFile presentation( m_openingFile );
		QDomDocument document;
		if( presentation.open( QIODevice::ReadOnly | QIODevice::Text ) && document.setContent( &presentation, false ) ) {
			delete m_sortFilterModel; delete m_model; delete m_watcher;
			m_sortFilterModel = NULL; m_model = NULL; m_watcher = NULL;
			
			m_model = new XmlPresentationModel( document );
			m_sortFilterModel = new RecursiveFilterProxyModel( m_model );
			m_sortFilterModel->setSourceModel( m_model );
		}
	} else if( m_threadAct == THREAD_FILTERED ) {
		m_sortFilterModel->setFilterRegExp( m_filteredText );
	}
}

void PrivateXmlPresentationDockWidget::threadTerminated() {
	if( m_threadAct == THREAD_OPENING ) {
		m_watcher = new FileWatcher( m_openingFile );
		connect( m_watcher, SIGNAL(fileChanged()), this, SLOT(open()) );
		m_xmlPresentationWidget->m_presentationTreeView->setModel( m_sortFilterModel );
	} else if( m_threadAct == THREAD_FILTERED ) {
		if( !m_filteredText.isEmpty() ) 
			m_xmlPresentationWidget->m_presentationTreeView->expandAll();
	}
	m_xmlPresentationWidget->m_presentationComboBox->setEnabled( true );
	m_xmlPresentationWidget->m_clearToolButton->setEnabled( true );
	m_xmlPresentationWidget->m_filtreLineEdit->setEnabled( true );
	m_xmlPresentationWidget->m_presentationProgressBar->setRange( 0, 1 );
	m_xmlPresentationWidget->m_presentationProgressBar->setValue( 1 );
}

void PrivateXmlPresentationDockWidget::filterTextChanged( const QString & text ) {
	Q_UNUSED( text );
	if( m_sortFilterModel ) 
		m_timerTextChanged.start();
}

void PrivateXmlPresentationDockWidget::filterTextChangedTimer() {
	if( m_sortFilterModel ) {
		m_xmlPresentationWidget->m_presentationProgressBar->setValue( 0 );
		m_xmlPresentationWidget->m_presentationProgressBar->setRange( 0, 0 );
		m_xmlPresentationWidget->m_presentationComboBox->setEnabled( false );
		m_xmlPresentationWidget->m_clearToolButton->setEnabled( false );
		m_xmlPresentationWidget->m_filtreLineEdit->setEnabled( false );

		m_xmlPresentationWidget->m_presentationTreeView->collapseAll();

		m_filteredText = m_xmlPresentationWidget->m_filtreLineEdit->text();

		m_threadAct = THREAD_FILTERED;
		start( QThread::IdlePriority );
	}
}


/* RecursiveFilterProxyModel */

PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::RecursiveFilterProxyModel( QObject * parent ) : QSortFilterProxyModel( parent ) {
	
}

bool PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::canBeShow( const QModelIndex & index ) const {
	bool show = sourceModel()->data( index ).toString().contains( filterRegExp() );
	
	if( ! show ) {
		int r = sourceModel()->rowCount( index );
		for( int i = 0; i < r; i++ ) {
			QModelIndex child = sourceModel()->index( i, filterKeyColumn(), index );
			if( canBeShow( child ) )
				return true;
		}
		return false;
	} else 
		return true;
}

bool PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const {
	XINX_ASSERT( filterKeyColumn() != -1 );
	
	QModelIndex index = sourceModel()->index( source_row, filterKeyColumn(), source_parent );
	return canBeShow( index );
}

/* XmlPresentationDockWidget */

XmlPresentationDockWidget::XmlPresentationDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	d = new PrivateXmlPresentationDockWidget( this );
}

XmlPresentationDockWidget::XmlPresentationDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	d = new PrivateXmlPresentationDockWidget( this );
}

XmlPresentationDockWidget::~XmlPresentationDockWidget() {
	delete d;
}
