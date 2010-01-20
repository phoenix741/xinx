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
#include <configuration/configurationmanager.h>
#include "projectproperty/generixproject.h"
#include "dictionaryparser.h"

/* DictionaryDockWidgetImpl */

DictionaryDockWidgetImpl::DictionaryDockWidgetImpl( QWidget * parent ) : QDockWidget( parent ), m_dictionaryModel( 0 )  {
	setupUi( this );

	// Create node and model to use for the dictionary
	//m_dictionaryModel = new ContentViewModel( m_dictionaryNode, m_dictionaryTreeView );
	//m_dictionaryTreeView->setModel( m_dictionaryModel );

	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(projectChanged()) );
}

DictionaryDockWidgetImpl::~DictionaryDockWidgetImpl() {

}

void DictionaryDockWidgetImpl::projectChanged() {
	// Desactivate the dictionary
	m_dictionaryTreeView->setModel( 0 );
	delete m_dictionaryModel; m_dictionaryModel = 0;

	// Create the new dictionary
	GenerixProject * project = static_cast<GenerixProject*>( XINXProjectManager::self()->project() );
	if( project && project->isGenerixActivated() ) {
		GceInterface * interface = ConfigurationManager::self()->getInterfaceOfProject( project );
		if( interface ) {
			DictionaryParser * parser = interface->dictionaryParser();
			if( parser ) {
				m_dictionaryModel = new ContentViewModel( parser->rootNode(), m_dictionaryTreeView );
				m_dictionaryTreeView->setModel( m_dictionaryModel );
			}
		}
	}
}



