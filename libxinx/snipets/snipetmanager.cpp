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
#include "core/xinxcore.h"
#include "snipets/snipetmanager.h"

// Qt header
#include <QSqlError>
#include <QSqlQuery>

/* Static member */

SnipetDatabaseManager * SnipetDatabaseManager::s_self = 0;

/* SnipetDatabaseManager */

SnipetDatabaseManager::SnipetDatabaseManager() {

}

SnipetDatabaseManager::~SnipetDatabaseManager() {
	if( s_self == this )
		s_self = NULL;
}

SnipetDatabaseManager * SnipetDatabaseManager::self() {
	if( s_self == 0 ) {
		s_self = new SnipetDatabaseManager();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

QSqlDatabase SnipetDatabaseManager::database() {
	if( !QSqlDatabase::contains( "SNIPETBASE" ) ) {
		openDatabase();
	}
	return QSqlDatabase::database( "SNIPETBASE", false );
}

SnipetItemModel * SnipetDatabaseManager::createSnipetItemModel( QObject * parent ) {
	return new SnipetItemModel( database(), parent );
}

bool SnipetDatabaseManager::openDatabase() {
	// Create the db object
	QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", "SNIPETBASE" );
	db.setDatabaseName( "datas:snipets.db");
	if( ! db.open() ) {
		qWarning( qPrintable( tr("Can't load snipet database : %1" ).arg( db.lastError().text() ) ) );
		return false;
	}

	// Check the content
	QStringList tables = db.tables();
	if( ! tables.contains("snipets") )
		createDatabase( db );

	return true;
}

void SnipetDatabaseManager::createDatabase( QSqlDatabase db ) {
	QSqlQuery createQuery( db );
	createQuery.exec( "CREATE TABLE category ("
					  "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
					  "    parent_id INTEGER NOT NULL DEFAULT(0),"
					  "    name TEXT NOT NULL,"
					  "    description TEXT,"
					  "    available_script TEXT)" );
	createQuery.exec( "CREATE TABLE snipet ("
					  "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
					  "    name TEXT,"
					  "    description TEXT,"
					  "    shortcut TEXT,"
					  "    icon TEXT,"
					  "    auto INTEGER,"
					  "    text TEXT,"
					  "    available_script TEXT,"
					  "    snipet_order INTEGER,"
					  "    category_id INTEGER  NOT NULL DEFAULT (0),"
					  "    FOREIGN KEY(category_id) REFERENCES category)" );
	createQuery.exec( "CREATE TABLE snipet_extentions ("
					  "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
					  "    snipet_id INTEGER NOT NULL,"
					  "    extention TEXT,"
					  "    FOREIGN KEY(snipet_id) REFERENCES snipet)" );
	createQuery.exec( "CREATE TABLE snipet_params ("
					  "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
					  "    snipet_id INTEGER NOT NULL,"
					  "    name TEXT,"
					  "    default_value TEXT,"
					  "    FOREIGN KEY(snipet_id) REFERENCES snipet)" );
}

void SnipetDatabaseManager::closeDatabase() {
	QSqlDatabase db = database();
	db.close();
	QSqlDatabase::removeDatabase( "SNIPETBASE" );
}

