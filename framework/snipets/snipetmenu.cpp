/* *********************************************************************'{}'** *
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
#include "editors/editormanager.h"
#include "editors/abstracteditor.h"
#include "scripts/scriptmanager.h"
#include "snipets/snipetmanager.h"

// Qt header
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QApplication>

/* SnipetMenuModel */

SnipetMenuModel::SnipetMenuModel( QSqlDatabase db, QObject * parent ) : SnipetItemModel( db, parent ) {
}

Qt::ItemFlags SnipetMenuModel::flags( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = sourceModel()->record( sourceIndex.row() );

		// Enable the snipet according to the script stored on the snipet or category
		QString availableScript = record.value( list_availablejs ).toString();
		if( ! SnipetManager::self()->isAvailable( availableScript, record.value( list_type ).toString(), record.value( list_id ).toInt() ) )
			return Qt::ItemIsSelectable;

		// The script is the only restriction for category
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return Qt::ItemIsEnabled;
		}

		// If no editor open, we can't call snipet
		if( EditorManager::self()->currentEditor() ) {
			// Get the filename in the editor
			QString filename = EditorManager::self()->currentEditor()->getTitle();

			if( SnipetManager::self()->isSnipetMatch( filename, record.value( list_id ).toInt() ) )
				return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
		}

		// The snipet can't be called
		return Qt::ItemIsSelectable;
	}
	return 0;
}

/* SnipetMenu */

SnipetMenu::SnipetMenu( QWidget * parent ) : QMenuView( parent ) {
	connect( this, SIGNAL(triggered(QModelIndex)), this, SLOT(snipetTriggered(QModelIndex)) );
}

SnipetMenu::~SnipetMenu() {
}

void SnipetMenu::snipetTriggered( const QModelIndex & index ) const {
	emit snipetTriggered( index.data( SnipetItemModel::SnipetIdRole ).toInt() );
}

void SnipetMenu::createSnipet() const {
	SnipetManager::self()->addSnipet( 1, qApp->activeWindow() );
}

bool SnipetMenu::prePopulated() {
	SnipetItemModel * m = static_cast<SnipetItemModel*>( model() );
	m->select();
	setRootIndex( m->index( 0, 0 ) );

	// Create snipet action
	m_createAction = new QAction( tr("&Create Snipet ..."), this );
	connect( m_createAction, SIGNAL(triggered()), this, SLOT(createSnipet()) );
	addAction( m_createAction );

	return true;
}


