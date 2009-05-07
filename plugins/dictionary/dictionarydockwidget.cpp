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
#include "dictionarydockwidget.h"
#include <contentview/contentviewmodel.h>
#include <contentview/contentviewnode.h>

/* DictionaryDockWidget */

DictionaryDockWidget::DictionaryDockWidget( const QString & name, QWidget * parent ) : QDockWidget( name, parent ) {
	setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
	m_dictionaryTreeView = new QTreeView( this );
	setWidget( m_dictionaryTreeView );

	m_dictionaryNode  = new ContentViewNode( "dictionary", -1 );
	m_dictionaryModel = new ContentViewModel( m_dictionaryNode, m_dictionaryTreeView );
	m_dictionaryTreeView->setModel( m_dictionaryModel );
}
