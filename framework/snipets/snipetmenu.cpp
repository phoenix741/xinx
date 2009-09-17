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

// Qt header
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

/* SnipetMenuModel */

SnipetMenuModel::SnipetMenuModel( QSqlDatabase db, QObject * parent ) : SnipetItemModel( db, parent ) {
}

Qt::ItemFlags SnipetMenuModel::flags( const QModelIndex & index ) const {
	if( index.isValid() ) {
		QModelIndex sourceIndex = mapToSource( index );
		QSqlRecord record = sourceModel()->record( sourceIndex.row() );

		// Enable the snipet according to the script stored on the snipet or category
		QString availableScript = record.value( list_availablejs ).toString();
		if( ! availableScript.isEmpty() ) {
			QScriptEngine & engine = ScriptManager::self()->engine();

			engine.pushContext();
			QScriptValue result = engine.evaluate( availableScript );
			if( result.isError() ) {
				qWarning() << "Error when calling script for record " << record.value( list_id ).toInt() << " : " << result.toString();
			} else {
#if QT_VERSION > 0x040500
				if( ! result.isBool() ) {
					qWarning() << "The script " << record.value( list_id ).toInt() << " of " << record.value( list_type ).toString() << " return neither true or false.\n"<< result.toString();
				} else {
					if( ! result.toBool() )
						return Qt::ItemIsSelectable;
				}
#else
				if( ! result.isBoolean() ) {
					qWarning() << "The script " << record.value( list_id ).toInt() << " of " << record.value( list_type ).toString() << " return neither true or false.\n"<< result.toString();
				} else {
					if( ! result.toBoolean() )
						return Qt::ItemIsSelectable;
				}
#endif
			}
			engine.popContext();
		}

		// The script is the only restriction for category
		if( record.value( list_type ).toString() == "CATEGORY" ) {
			return Qt::ItemIsEnabled;
		}

		// If no editor open, we can't call snipet
		if( EditorManager::self()->currentEditor() ) {
			// Get the filename in the editor
			QString filename = EditorManager::self()->currentEditor()->getTitle();

			// Check extentions to know if the snipet can be used with this editor
			QSqlQuery extentionsQuery( "SELECT DISTINCT extention FROM snipets_extentions WHERE snipet_id=:id", database() );
			extentionsQuery.bindValue( ":id", record.value( list_id ).toInt() );
			if( ! extentionsQuery.exec() ) {
				qWarning() << "Can't lookup extentions for snipet " << record.value( list_id ).toInt();
				return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
			}

			// If no extention selected, all can be used
			if( ! extentionsQuery.next() ) {
				return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
			}

			do {
				QRegExp regExp( extentionsQuery.value( 0 ).toString(), Qt::CaseInsensitive, QRegExp::Wildcard );
				if( regExp.exactMatch( filename ) )
					return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
			} while( extentionsQuery.next() );
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

bool SnipetMenu::prePopulated() {
	SnipetItemModel * m = static_cast<SnipetItemModel*>( model() );
	m->select();
	setRootIndex( m->index( 0, 0 ) );

	// Create snipet action
	m_createAction = new QAction( tr("&Create Snipet ..."), this );
	addAction( m_createAction );

	return true;
}


