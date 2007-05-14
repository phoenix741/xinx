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

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>

WebServicesEditor::WebServicesEditor( WebServicesList * list, QWidget *parent, XSLProject * project ) :
	FileEditor( parent, project ), m_list( list ) {
	
	QLabel * label = new QLabel( tr("Parameter list:"), this );	
	m_paramList = new QComboBox( this );
	m_paramList->addItem( "" );
	
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget( label );
	hbox->addWidget( m_paramList );
	
	m_vbox->insertLayout( 0, hbox );
}

