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
#include <configuration/configurationfile.h>
#include <project/xinxproject.h>

// Qt header
#include <QVBoxLayout>
#include <QtConcurrentRun>
#include <QFileDialog>

/* DictionaryDockWidgetImpl */

DictionaryDockWidgetImpl::DictionaryDockWidgetImpl( QWidget * parent ) : QDockWidget( parent ), m_dictionaryModel(0)  {
	setupUi( this );

	// Create node and model to use for the dictionary
	//m_dictionaryModel = new ContentViewModel( m_dictionaryNode, m_dictionaryTreeView );
	//m_dictionaryTreeView->setModel( m_dictionaryModel );
}

DictionaryDockWidgetImpl::~DictionaryDockWidgetImpl() {

}

void DictionaryDockWidgetImpl::loadDictionaryList( const QString & filename ) {


}

void DictionaryDockWidgetImpl::clearDictionaryList() {
	m_dictionaryList->setCurrentIndex( 0 );
	while( m_dictionaryList->count() > 2 ) {
		m_dictionaryList->removeItem( 2 );
	}
}

void DictionaryDockWidgetImpl::dictionaryLoaded() {
	m_dictionaryList->setEnabled( true );
}

void DictionaryDockWidgetImpl::on_m_dictionaryList_currentIndexChanged( int index ) {

}

void DictionaryDockWidgetImpl::loadDictionary( const QString & filename ) {

}

