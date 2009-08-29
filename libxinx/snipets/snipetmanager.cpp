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
#include <QFileInfo>

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

int SnipetDatabaseManager::getCategoryId( const QStringList & category ) {
	int parentCategory = 0;
	QSqlQuery selectQuery( "SELECT id FROM category WHERE parent_id=:parentCategory AND LOWER(name) like LOWER(:name)", database() );
	QSqlQuery insertQuery( "INSERT INTO category(parent_id, name) VALUES(:parentCategory, :name)", database() );
	for( int index; index < category.count(); index++ ) {
		const QString & categoryLevelName = category.at( index );
	
		selectQuery.bindValue( ":parentCategory", parentCategory );
		selectQuery.bindValue( ":name", categoryLevelName );
		
		bool result = selectQuery.exec();
		Q_ASSERT( result );
		
		if( selectQuery.next() ) {
			parentCategory = selectQuery.value( 0 ).toInt();
		} else {
			insertQuery.bindValue( ":parentCategory", parentCategory );
			insertQuery.bindValue( ":name", categoryLevelName );
			result = insertQuery.exec();
			
			Q_ASSERT( result );
			
			// Afin de relancer la recherche courante.
			index--;
		}
	}
	return parentCategory;
}

bool SnipetDatabaseManager::removeSnipet( int id ) {
	QSqlQuery removeSnipetQuery(
	                       "DELETE FROM snipets "
	                       "WHERE id=:id", database() );
	removeSnipetQuery.bindValue( ":id", id );

	if( ! removeSnipetQuery.exec() ) {
		qWarning( qPrintable( removeSnipetQuery.lastError().text() ) );
		return false;
	}
	return true;
}

bool SnipetDatabaseManager::importSnipetList( const SnipetList & list ) {
	QSqlQuery insertSnipetQuery( "INSERT INTO snipets(name, description, shortcut, icon, auto, text, available_script, category_id) "
				     "VALUES(:name, :description, :shortcut, :icon, :auto, :text, :available_script, :category_id)", database() );
	QSqlQuery selectSnipetQuery( "SELECT id FROM snipets WHERE rowid=last_insert_rowid()", database() );
	QSqlQuery insertSnipetParamsQuery( "INSERT INTO snipets_extentions(snipet_id, extention) VALUES (:snipet_id, :extention)", database() );
	QSqlQuery insertSnipetExtentionsQuery( "INSERT INTO snipets_params(snipet_id, name, default_value) VALUES (:snipet_id, :name, :default_value)", database() );
	
	foreach( const Snipet & s, list ) {
		int categoryId = getCategoryId( s.categories() );
		
		insertSnipetQuery.bindValue( ":name", s.name() );
		insertSnipetQuery.bindValue( ":description", s.description() );
		insertSnipetQuery.bindValue( ":shortcut", s.key() );
		insertSnipetQuery.bindValue( ":icon", s.icon() );
		insertSnipetQuery.bindValue( ":auto", s.callIsAutomatic() );
		insertSnipetQuery.bindValue( ":text", s.text() );
		insertSnipetQuery.bindValue( ":available_script", s.availableScript() );
		insertSnipetQuery.bindValue( ":category_id", categoryId );
		
		if( ! insertSnipetQuery.exec() ) {
			qWarning( qPrintable( insertSnipetQuery.lastError().text() ) );
			return false;
		}
		
		bool result = selectSnipetQuery.exec();
		Q_ASSERT( result );
		
		int snipetId = selectSnipetQuery.value( 0 ).toInt();
		
		insertSnipetExtentionsQuery.bindValue( ":snipet_id", snipetId );
		insertSnipetParamsQuery.bindValue( ":snipet_id", snipetId );
		
		foreach( const QString & ext, s.extentions() ) {
			insertSnipetExtentionsQuery.bindValue( ":extention", ext );
			if( insertSnipetExtentionsQuery.exec() ) {
				qWarning( qPrintable( insertSnipetExtentionsQuery.lastError().text() ) );
				return false;
			}
		}
		
		foreach( const Snipet::Parameter & param, s.params() ) {
			insertSnipetParamsQuery.bindValue( ":name", param.name );
			insertSnipetParamsQuery.bindValue( ":defaultValue", param.defaultValue );	
			if( insertSnipetParamsQuery.exec() ) {
				qWarning( qPrintable( insertSnipetParamsQuery.lastError().text() ) );
				return false;
			}
		}
	}
	
	return true;
}

bool SnipetDatabaseManager::openDatabase() {
	// Create the db object
	QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", "SNIPETBASE" );
	QString databaseFileName = QFileInfo( "datas:snipets.db" ).absoluteFilePath();
	db.setDatabaseName( databaseFileName );
	if( ! db.open() ) {
		qWarning( qPrintable( tr("Can't load snipet database : %1" ).arg( db.lastError().text() ) ) );
		return false;
	}

	// Check the content
	QStringList tables = db.tables();
	if( ! tables.contains("snipets") )
		return createDatabase( db );

	return true;
}

bool SnipetDatabaseManager::createDatabase( QSqlDatabase db ) {
	QSqlQuery createQuery( db );
	if( !
		createQuery.exec( "CREATE TABLE category ("
						  "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
						  "    parent_id INTEGER NOT NULL DEFAULT(0),"
						  "    name TEXT NOT NULL,"
						  "    description TEXT,"
						  "    available_script TEXT,"
						  "    category_order INTEGER)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE TABLE snipets ("
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
						  "    FOREIGN KEY(category_id) REFERENCES category)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE TABLE snipets_extentions ("
						  "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
						  "    snipet_id INTEGER NOT NULL,"
						  "    extention TEXT,"
						  "    FOREIGN KEY(snipet_id) REFERENCES snipet)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE TABLE snipets_params ("
						  "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
						  "    snipet_id INTEGER NOT NULL,"
						  "    name TEXT,"
						  "    default_value TEXT,"
						  "    FOREIGN KEY(snipet_id) REFERENCES snipet)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	return true;
}

void SnipetDatabaseManager::closeDatabase() {
	QSqlDatabase db = database();
	db.close();
	QSqlDatabase::removeDatabase( "SNIPETBASE" );
}

