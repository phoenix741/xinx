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
#include "dictionarydockwidgetimpl.h"
#include <contentview/contentviewmodel.h>
#include <contentview/contentviewnode.h>
#include "dictionaryparser.h"
#include <core/configurationfile.h>
#include <core/xinxproject.h>

// Qt header
#include <QVBoxLayout>
#include <QtConcurrentRun>
#include <QFileDialog>

/* Static function */

QString loadDictionaryParser( DictionaryParser * parser ) {
	try {
		parser->loadFromMember();
	} catch( ContentViewException e ) {
		return e.getMessage();
	}
	return QString();
}

/* DictionaryDockWidgetImpl */

DictionaryDockWidgetImpl::DictionaryDockWidgetImpl( QWidget * parent ) : QDockWidget( parent ), m_dictionaryNode(0)  {
	setupUi( this );

	connect( &m_watcher, SIGNAL(finished()), this, SLOT(dictionaryLoaded()) );

	m_dictionaryList->addItem( tr("<No dictionary file>") );
	m_dictionaryList->addItem( tr("<Choose a dictionary file ...>"), "..." );

	// Create node and model to use for the dictionary
	m_dictionaryNode  = new ContentViewNode( "dictionary", -1 );
	m_dictionaryModel = new ContentViewModel( m_dictionaryNode, m_dictionaryTreeView );
	m_dictionaryTreeView->setModel( m_dictionaryModel );
}

DictionaryDockWidgetImpl::~DictionaryDockWidgetImpl() {
	if( m_watcher.isRunning() ) {
		m_watcher.waitForFinished();
	}
}

void DictionaryDockWidgetImpl::loadDictionaryList( const QString & filename ) {
	clearDictionaryList();

	MetaConfigurationFile metaConfigurationFile( filename );
	foreach( const QString & filename, metaConfigurationFile.dictionaryList() ) {
		m_dictionaryList->addItem( QFileInfo( filename ).fileName(), filename );
	}
}

void DictionaryDockWidgetImpl::clearDictionaryList() {
	m_dictionaryList->setCurrentIndex( 0 );
	while( m_dictionaryList->count() > 2 ) {
		m_dictionaryList->removeItem( 2 );
	}
}

void DictionaryDockWidgetImpl::dictionaryLoaded() {
	m_dictionaryList->setEnabled( true );

	QString message = m_watcher.result();
	if( ! message.isEmpty() )
		qWarning( qPrintable( message ) );
}

void DictionaryDockWidgetImpl::on_m_dictionaryList_currentIndexChanged( int index ) {
	if( ! m_dictionaryNode ) return;

	QString filename = m_dictionaryList->itemData( index ).toString();
	m_dictionaryNode->clear();

	if( filename == "..." ) {
		QString projectPath = XINXProjectManager::self()->project() ? XINXProjectManager::self()->project()->projectPath() : QString();
		filename = QFileDialog::getOpenFileName( this, tr("Open a dictionary"), projectPath, tr("Dictionary (*.xml)") );
	}
	if( filename.isEmpty() ) return;

	loadDictionary( filename );
}

void DictionaryDockWidgetImpl::loadDictionary( const QString & filename ) {
	DictionaryParser * parser = 0;
	try {
		m_dictionaryList->setEnabled( false );
		parser = new DictionaryParser( true );
		parser->setRootNode( m_dictionaryNode );
		parser->setFilename( filename );
		m_watcher.setFuture( QtConcurrent::run( loadDictionaryParser, parser ) );
	} catch( ContentViewException e ) {
		m_dictionaryList->setEnabled( true );
		qWarning( qPrintable( e.getMessage() ) );
	}
}

