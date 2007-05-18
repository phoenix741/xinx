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

#include "webserviceseditor.h"
#include "xmleditor.h"

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>

WebServicesEditor::WebServicesEditor( WebServicesList * list, QWidget *parent, XSLProject * project ) :
	FileEditor( new XMLEditor( parent ), parent, project ), m_list( list ) {
	
	QLabel * label1 = new QLabel( tr("WebServices : "), this );
	m_servicesList = new QComboBox( this );
	m_servicesList->addItem( "crudManager" );

	QLabel * label2 = new QLabel( tr("Action : "), this );
	m_actionList = new QComboBox( this );
	m_actionList->addItem( "create" );
		
	QLabel * label3 = new QLabel( tr("Parameter : "), this );	
	m_paramList = new QComboBox( this );
	m_paramList->addItem( "String_1" );
	
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( label1 );
	hbox->addWidget( m_servicesList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label2 );
	hbox->addWidget( m_actionList );
	hbox->addSpacing( 10 );
	hbox->addWidget( label3 );
	hbox->addWidget( m_paramList );
	hbox->addStretch();
	
	m_vbox->insertLayout( 0, hbox );
}

