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
#include "snipets/snipetmenu.h"
#include "snipets/snipetmodelindex.h"

/* SnipetMenu */

SnipetMenu::SnipetMenu( QWidget * parent ) : QMenuView( parent ) {
	connect( this, SIGNAL(triggered(QModelIndex)), this, SLOT(snipetTriggered(QModelIndex)) );
}

SnipetMenu::~SnipetMenu() {
}

void SnipetMenu::snipetTriggered( const QModelIndex & index ) const {
	emit snipetTriggered( index.data( SnipetItemModel::SnipetIdRole ).toInt() );
}

bool SnipetMenu::prePopulated() {
	SnipetItemModel * m = static_cast<SnipetItemModel*>( model() );
	m->select();

	// Create snipet action
	m_createAction = new QAction( tr("&Create Snipet ..."), this );
	addAction( m_createAction );

	return true;
}


