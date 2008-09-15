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
#include "xslproject.h"
#include <xinxconfig.h>

// Qt header
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QDateTime>

/* PrivateXmlPresentationDockWidget */

PrivateXmlPresentationDockWidget::PrivateXmlPresentationDockWidget( XmlPresentationDockWidget * parent ) : m_model(0), m_sortFilterModel(0), m_watcher(0), m_parent( parent ) {
	qRegisterMetaType<QModelIndex>( "QModelIndex" );

	QWidget * contentWidget = new QWidget( m_parent );
	m_xmlPresentationWidget = new Ui::XmlPresentationWidget();
	m_xmlPresentationWidget->setupUi( contentWidget );
	m_parent->setWidget( contentWidget );

	m_xmlPresentationWidget->m_presentationProgressBar->hide();

	m_timerTextChanged.setSingleShot( true );
	m_timerTextChanged.setInterval( 1000 );

	m_xmlPresentationWidget->m_filterComboBox->setCurrentIndex( XINXConfig::self()->config().xmlPres.showFilteredSubTree ? 0 : 1 );

	initXmlPresentationCombo();

	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(initXmlPresentationCombo()) );
	connect( m_xmlPresentationWidget->m_refreshToolButton, SIGNAL(clicked()), this, SLOT(initXmlPresentationCombo()) );

	connect( m_xmlPresentationWidget->m_presentationComboBox, SIGNAL(activated(int)), this, SLOT(presentationActivated(int)) );
	connect( m_xmlPresentationWidget->m_filtreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(filterTextChanged(QString)) );
	connect( this, SIGNAL(finished()), this, SLOT(threadTerminated()) );

	connect( m_xmlPresentationWidget->m_presentationTreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(adaptColumns()));

	connect( m_xmlPresentationWidget->m_filterComboBox, SIGNAL(activated(int)), this, SLOT(updateXinxConf(int)) );
	connect( &m_timerTextChanged, SIGNAL(timeout()), this, SLOT(filterTextChangedTimer()) );
	connect( XINXConfig::self(), SIGNAL(changed()), this, SLOT(filterTextChangedTimer()) );
}


PrivateXmlPresentationDockWidget::~PrivateXmlPresentationDockWidget() {
	presentationActivated( 0 );
	delete m_xmlPresentationWidget;
}

void PrivateXmlPresentationDockWidget::adaptColumns() {
  	m_xmlPresentationWidget->m_presentationTreeView->resizeColumnToContents(0);
}


void PrivateXmlPresentationDockWidget::initXmlPresentationCombo() {
	int index;
	QString filename = m_openingFile;

	m_xmlPresentationWidget->m_presentationComboBox->blockSignals( true );
	m_xmlPresentationWidget->m_presentationComboBox->clear();

	m_xmlPresentationWidget->m_presentationComboBox->addItem( tr("<No presentation file>") );
	m_xmlPresentationWidget->m_presentationComboBox->addItem( tr("<Choose an XML file ...>") );

	if( XINXProjectManager::self()->project() ) {
		m_logPath = XINXProjectManager::self()->project()->logProjectDirectory();
		QDir logDir( m_logPath );
		if( logDir.exists() ) {
			QStringList files = logDir.entryList( QStringList() << "Presentation_*.xml", QDir::Files | QDir::Readable );
			foreach( QString file, files )
				m_xmlPresentationWidget->m_presentationComboBox->addItem( file );
		}
	}

	if( filename.isEmpty() ) {
		m_xmlPresentationWidget->m_presentationComboBox->blockSignals( false );
		presentationActivated( 0 );
		return;
	}

	if( ( index = m_xmlPresentationWidget->m_presentationComboBox->findText( QFileInfo( filename ).fileName() ) ) >= 0 ) {
		m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex( index );
	} else if( QFileInfo( filename ).exists() ) {
		m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex( 1 );
	} else
		presentationActivated( 0 );
	m_xmlPresentationWidget->m_presentationComboBox->blockSignals( false );
}

void PrivateXmlPresentationDockWidget::presentationActivated( int index ) {
	if( index == 0 ) {
		m_xmlPresentationWidget->m_presentationTreeView->setModel( NULL );
		delete m_watcher;
		delete m_sortFilterModel;
		delete m_model;
		m_openingFile = QString();
	} else if( index == 1 ) {
		// Open a file
		QString name = QFileDialog::getOpenFileName( m_parent,
			tr("Open a presentation file"),
			m_logPath,
			tr("Presentation XML File (*.xml)") );
		if( name.isEmpty() ) {
			m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex( 0 );
		} else {
			setComboToolTip( name );
			open( name );
		}
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

	m_xmlPresentationWidget->m_presentationProgressBar->show();
	m_xmlPresentationWidget->m_presentationComboBox->setEnabled( false );
	m_xmlPresentationWidget->m_clearToolButton->setEnabled( false );
	m_xmlPresentationWidget->m_filtreLineEdit->setEnabled( false );
	m_xmlPresentationWidget->m_filterComboBox->setEnabled( false );
	m_xmlPresentationWidget->m_refreshToolButton->setEnabled( false );
	m_xmlPresentationWidget->m_presentationTreeView->setModel( NULL );

	m_openingFile = filename;

	m_filteredText = m_xmlPresentationWidget->m_filtreLineEdit->text();
	m_filteredElement = XINXConfig::self()->config().xmlPres.showFilteredSubTree;

	delete m_sortFilterModel; delete m_model; delete m_watcher;

	if( !QFileInfo( filename ).exists() ) {
		m_openingFile = QString();
		m_xmlPresentationWidget->m_presentationComboBox->setCurrentIndex( 0 );
		m_xmlPresentationWidget->m_presentationProgressBar->hide();
		m_xmlPresentationWidget->m_presentationComboBox->setEnabled( true );
		m_xmlPresentationWidget->m_filterComboBox->setEnabled( true );
		m_xmlPresentationWidget->m_refreshToolButton->setEnabled( true );
		m_xmlPresentationWidget->m_clearToolButton->setEnabled( true );
		m_xmlPresentationWidget->m_filtreLineEdit->setEnabled( true );
		m_xmlPresentationWidget->m_filtreLineEdit->setFocus();
		return;
	}

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
			m_model = new XmlPresentationModel( document );
			m_sortFilterModel = new RecursiveFilterProxyModel( m_model );
			m_sortFilterModel->setSourceModel( m_model );
		}
	}
	if( m_model ) {
		m_sortFilterModel->setShowAllChild( m_filteredElement );
		m_sortFilterModel->setFilterRegExp( m_filteredText );
		m_model->moveToThread( qApp->thread() );
	}
}

void PrivateXmlPresentationDockWidget::threadTerminated() {
	if( m_threadAct == THREAD_OPENING ) {
		m_watcher = new FileWatcher( m_openingFile );
		connect( m_watcher, SIGNAL(fileChanged()), this, SLOT(open()) );
		m_xmlPresentationWidget->m_presentationTreeView->setModel( m_sortFilterModel );
	} else if( m_threadAct == THREAD_FILTERED ) {
	}

	/* Filter part */
	QModelIndex expandIndex = QModelIndex();
	if( !m_filteredText.isEmpty() ) {
		while( m_sortFilterModel->rowCount( expandIndex ) > 0 ) {
			m_xmlPresentationWidget->m_presentationTreeView->expand( expandIndex );
			expandIndex = m_sortFilterModel->index( 0, 0, expandIndex );
		}
	} else {
		QStringList xpath = m_currentXpath.isEmpty() ? XINXConfig::self()->config().xmlPres.autoExpandedPath.split( '/' ) : m_currentXpath.split( '/' );
		foreach( QString path, xpath ) {
			m_xmlPresentationWidget->m_presentationTreeView->expand( expandIndex );
			if( ! path.isEmpty() && m_sortFilterModel->rowCount( expandIndex ) ) {
				QModelIndexList matchedIndex = m_sortFilterModel->match( m_sortFilterModel->index( 0, 0, expandIndex ), XmlPresentationModel::NamedViewRole, path, 1, Qt::MatchExactly );
				if( matchedIndex.count() > 0 )
					expandIndex = matchedIndex.at( 0 );
				else {
					matchedIndex = m_sortFilterModel->match( m_sortFilterModel->index( 0, 0, expandIndex ), Qt::DisplayRole, path, 1, Qt::MatchExactly );
					if( matchedIndex.count() > 0 )
						expandIndex = matchedIndex.at( 0 );
				}
			}
		}
	}

	m_xmlPresentationWidget->m_presentationTreeView->expand( expandIndex );
	m_xmlPresentationWidget->m_presentationTreeView->setCurrentIndex( expandIndex );
	/* End of filter part */

	m_xmlPresentationWidget->m_presentationProgressBar->hide();
	m_xmlPresentationWidget->m_presentationComboBox->setEnabled( true );
	m_xmlPresentationWidget->m_filterComboBox->setEnabled( true );
	m_xmlPresentationWidget->m_refreshToolButton->setEnabled( true );
	m_xmlPresentationWidget->m_clearToolButton->setEnabled( true );
	m_xmlPresentationWidget->m_filtreLineEdit->setText( m_filteredText ); // Au cas o� des caract�res n'ont pas �t� pris en compte
	m_xmlPresentationWidget->m_filtreLineEdit->setEnabled( true );
	m_xmlPresentationWidget->m_filtreLineEdit->setFocus();
}

void PrivateXmlPresentationDockWidget::filterTextChanged( const QString & text ) {
	Q_UNUSED( text );
	if( m_sortFilterModel )
		m_timerTextChanged.start();
}

void PrivateXmlPresentationDockWidget::filterTextChangedTimer() {
	Q_ASSERT( ! isRunning() );
	m_timerTextChanged.stop();

	if( m_sortFilterModel ) {
		// TODO: Delete this line in 4.4
		m_xmlPresentationWidget->m_filtreLineEdit->clearFocus();
		m_xmlPresentationWidget->m_filtreLineEdit->setEnabled( false );
		m_xmlPresentationWidget->m_presentationComboBox->setEnabled( false );
		m_xmlPresentationWidget->m_clearToolButton->setEnabled( false );
		m_xmlPresentationWidget->m_filterComboBox->setEnabled( false );
		m_xmlPresentationWidget->m_refreshToolButton->setEnabled( false );

		if( XINXConfig::self()->config().xmlPres.showFilteredSubTree != ( m_xmlPresentationWidget->m_filterComboBox->currentIndex() == 0 ) ) {
			m_xmlPresentationWidget->m_filterComboBox->setCurrentIndex( XINXConfig::self()->config().xmlPres.showFilteredSubTree ? 0 : 1 );
		}

		m_currentXpath = m_sortFilterModel->data( m_xmlPresentationWidget->m_presentationTreeView->currentIndex(), Qt::UserRole ).toString();

		m_xmlPresentationWidget->m_presentationTreeView->collapseAll();
		m_xmlPresentationWidget->m_presentationProgressBar->show();

		m_filteredText = m_xmlPresentationWidget->m_filtreLineEdit->text();
		m_filteredElement = XINXConfig::self()->config().xmlPres.showFilteredSubTree;

		m_model->moveToThread( this );

		m_threadAct = THREAD_FILTERED;
		start( QThread::IdlePriority );
	}
}

void PrivateXmlPresentationDockWidget::updateXinxConf( int value ) {
	bool filteredElement = value == 0;
	if( XINXConfig::self()->config().xmlPres.showFilteredSubTree != filteredElement ) {
		XINXConfig::self()->config().xmlPres.showFilteredSubTree = filteredElement;
		XINXConfig::self()->save();
	}
}

/* RecursiveFilterProxyModel */

PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::RecursiveFilterProxyModel( QObject * parent ) : QSortFilterProxyModel( parent ), m_showAllChild( true ) {

}

bool PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::canBeShow( const QModelIndex & index ) const {
	if( m_indexCache.contains( index ) ) return m_indexCache.value( index );

	QString data = sourceModel()->data( index ).toString();
	if( XINXConfig::self()->config().xmlPres.hidePath.contains( data ) ) {
		m_indexCache.insert( QPersistentModelIndex( index ), false );
		return false;
	}

	bool show = data.contains( filterRegExp() );

	if( ! show ) {
		int r = sourceModel()->rowCount( index );
		for( int i = 0; i < r; i++ ) {
			QModelIndex child = sourceModel()->index( i, filterKeyColumn(), index );
			if( canBeShow( child ) ) {
				m_indexCache.insert( QPersistentModelIndex( index ), true );
				return true;
			}
		}

		show = m_showAllChild && mustBeShow( index );
		m_indexCache.insert( QPersistentModelIndex( index ), show );
		return show;
	} else {
		m_indexCache.insert( QPersistentModelIndex( index ), true );
		return true;
	}
}

bool PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::mustBeShow( const QModelIndex & index ) const {
	QModelIndex parent = index.parent();
	if( ! parent.isValid() ) return false;
	bool show = parent.data().toString().contains( filterRegExp() );
	return show || mustBeShow( parent );
}

bool PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const {
	Q_ASSERT( filterKeyColumn() != -1 );

	QModelIndex index = sourceModel()->index( source_row, filterKeyColumn(), source_parent );
	return canBeShow( index );
}

bool PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::showAllChild() const {
	return m_showAllChild;
}

void PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::setShowAllChild( bool value ) {
	if( m_showAllChild != value ) {
		m_indexCache.clear();
		m_showAllChild = value;
		m_indexCache.clear();
	}
}

void PrivateXmlPresentationDockWidget::RecursiveFilterProxyModel::setFilterRegExp( const QString & regExp ) {
	m_indexCache.clear();
	QSortFilterProxyModel::setFilterRegExp( regExp );
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
