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
#include "snipets/callsnipetdlg.h"
#include "snipets/snipetpropertydlgimpl.h"
#include "snipets/categorypropertydlgimpl.h"
#include "scripts/scriptmanager.h"
#include "snipets/snipetitemmodel.h"
#include "snipets/categoryitemmodel.h"
#include "snipets/snipetmenu.h"

// Qt header
#include <QSqlError>
#include <QSqlQuery>
#include <QFileInfo>
#include <QScriptEngine>
#include <QInputDialog>
#include <QMessageBox>

/* Static member */

SnipetManager * SnipetManager::s_self = 0;

/* SnipetManager */

SnipetManager::SnipetManager() {

}

SnipetManager::~SnipetManager() {
	closeDatabase();
	if( s_self == this )
		s_self = NULL;
}

SnipetManager * SnipetManager::self() {
	if( s_self == 0 ) {
		s_self = new SnipetManager();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

QSqlDatabase SnipetManager::database() {
	if( !QSqlDatabase::contains( "SNIPETBASE" ) ) {
		openDatabase();
	}
	return QSqlDatabase::database( "SNIPETBASE", false );
}

SnipetItemModel * SnipetManager::createSnipetItemModel( QObject * parent ) {
	return new SnipetItemModel( database(), parent );
}

CategoryItemModel * SnipetManager::createCategoryItemModel( QObject * parent ) {
	return new CategoryItemModel( database(), parent );
}

SnipetMenu * SnipetManager::createSnipetMenu( const QString & title, QWidget * parent ) {
	SnipetMenu * menu = new SnipetMenu( parent );
	menu->setTitle( title );

	SnipetItemModel * snipetModel = new SnipetMenuModel( database(), menu );

	menu->setModel( snipetModel );

	return menu;
}

int SnipetManager::getCategoryId( const QStringList & category ) {
	int parentCategory = 1;
	QSqlQuery selectQuery( "SELECT id FROM categories WHERE parent_id=:parentCategory AND LOWER(name) like LOWER(:name)", database() );
	QSqlQuery insertQuery( "INSERT INTO categories(parent_id, name) VALUES(:parentCategory, :name)", database() );
	for( int index = 0; index < category.count(); index++ ) {
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

void SnipetManager::addCategory( int parentId, bool categoryAccess, QWidget * parent ) {
	CategoryPropertyDlgImpl dlg( database(), parent );
	dlg.setParentId( parentId );
	dlg.setCategoryAccess( categoryAccess );
	dlg.exec();
}

void SnipetManager::removeCategory( int id, QWidget * parent ) {
	Q_ASSERT( id != 0 );

	QSqlQuery countQuery( "select (select count(1) from categories where parent_id=:snipet_id1) + (select count(1) from snipets where category_id=:snipet_id2) as count", database() );
	countQuery.bindValue( ":snipet_id1", id );
	countQuery.bindValue( ":snipet_id2", id );
	bool result = countQuery.exec();

	Q_ASSERT( result );

	countQuery.next();

	int count = countQuery.value( 0 ).toInt();
	if( count > 0 ) {
		QMessageBox::critical( parent, tr("Remove a category"), tr("This category contains snipet or other category, you can't remove this category.") );
		return;
	}

	QSqlQuery titleQuery( "select name from categories where id=:id", database() );
	titleQuery.bindValue( ":id", id );

	result = titleQuery.exec();
	Q_ASSERT( result );

	result = titleQuery.next();
	Q_ASSERT( result );

	int r = QMessageBox::question( parent, tr("Remove a category"), tr("Are you sure to delete the category \"%1\"").arg( titleQuery.value( 0 ).toString() ), QMessageBox::Yes | QMessageBox::No );

	if( r == QMessageBox::Yes ) {
		QSqlQuery removeQuery( "DELETE FROM categories WHERE id=:id", database() );
		removeQuery.bindValue( ":id", id );
		result = removeQuery.exec();

		Q_ASSERT( result );
	}
}

void SnipetManager::modifyCategory( int id, QWidget * parent ) {
	CategoryPropertyDlgImpl dlg( id, database(), parent );
	dlg.exec();
}

void SnipetManager::addSnipet( int parentId, QWidget * parent ) {
	SnipetPropertyDlgImpl dlg( database(), parent );
	dlg.setParentId( parentId );
	dlg.exec();
}

void SnipetManager::modifySnipet( int id, QWidget * parent ) {
	SnipetPropertyDlgImpl dlg( id, database(), parent );
	dlg.exec();
}

void SnipetManager::duplicateSnipet( int id, QWidget * parent ) {
	SnipetPropertyDlgImpl dlg( database(), parent );
	dlg.duplicate( id );
	dlg.exec();
}

bool SnipetManager::removeSnipet( int id, QWidget * parent ) {
	QSqlQuery removeSnipetQuery(
	                       "DELETE FROM snipets "
	                       "WHERE id=:id", database() );
	removeSnipetQuery.bindValue( ":id", id );
	QSqlQuery removeSnipetParamsQuery(
	                       "DELETE FROM snipets_params "
	                       "WHERE snipet_id=:id", database() );
	removeSnipetParamsQuery.bindValue( ":id", id );
	QSqlQuery removeSnipetExtQuery(
	                       "DELETE FROM snipets_extentions "
	                       "WHERE snipet_id=:id", database() );
	removeSnipetExtQuery.bindValue( ":id", id );

	if( ! removeSnipetQuery.exec() ) {
		qWarning( qPrintable( removeSnipetQuery.lastError().text() ) );
		return false;
	}
	if( ! removeSnipetParamsQuery.exec() ) {
		qWarning( qPrintable( removeSnipetQuery.lastError().text() ) );
		return false;
	}
	if( ! removeSnipetExtQuery.exec() ) {
		qWarning( qPrintable( removeSnipetQuery.lastError().text() ) );
		return false;
	}
	return true;
}

bool SnipetManager::importSnipetList( const SnipetList & list, QWidget * parent ) {
	QSqlQuery insertSnipetQuery( "INSERT INTO snipets(name, description, shortcut, icon, auto, show_dialog, text, available_script, category_id) "
	                             "VALUES(:name, :description, :shortcut, :icon, :auto, :dialog, :text, :available_script, :category_id)", database() );
	QSqlQuery insertSnipetExtentionsQuery( "INSERT INTO snipets_extentions(snipet_id, extention) VALUES (:snipet_id, :extention)", database() );
	QSqlQuery insertSnipetParamsQuery( "INSERT INTO snipets_params(snipet_id, name, default_value) VALUES (:snipet_id, :name, :default_value)", database() );

	foreach( const Snipet & s, list ) {
		int categoryId = getCategoryId( QStringList() << tr("Imported Snipets") << s.categories() );

		insertSnipetQuery.bindValue( ":name", s.name() );
		insertSnipetQuery.bindValue( ":description", s.description() );
		insertSnipetQuery.bindValue( ":shortcut", s.key() );
		insertSnipetQuery.bindValue( ":icon", s.icon() );
		insertSnipetQuery.bindValue( ":auto", s.callIsAutomatic() );
		insertSnipetQuery.bindValue( ":dialog", s.showDialog() );
		insertSnipetQuery.bindValue( ":text", s.text() );
		insertSnipetQuery.bindValue( ":available_script", s.availableScript() );
		insertSnipetQuery.bindValue( ":category_id", categoryId );

		if( ! insertSnipetQuery.exec() ) {
			qWarning( qPrintable( insertSnipetQuery.lastError().text() ) );
			return false;
		}

		int snipetId = insertSnipetQuery.lastInsertId().toInt();

		insertSnipetExtentionsQuery.bindValue( ":snipet_id", snipetId );
		insertSnipetParamsQuery.bindValue( ":snipet_id", snipetId );

		foreach( const QString & ext, s.extentions() ) {
			insertSnipetExtentionsQuery.bindValue( ":extention", ext );
			if( ! insertSnipetExtentionsQuery.exec() ) {
				qWarning( qPrintable( insertSnipetExtentionsQuery.lastError().text() ) );
				return false;
			}
		}

		foreach( const Snipet::Parameter & param, s.params() ) {
			insertSnipetParamsQuery.bindValue( ":name", param.name );
			insertSnipetParamsQuery.bindValue( ":default_value", param.defaultValue );
			if( ! insertSnipetParamsQuery.exec() ) {
				qWarning( qPrintable( insertSnipetParamsQuery.lastError().text() ) );
				return false;
			}
		}
	}

	return true;
}

bool SnipetManager::callSnipet( int id, QString * result, QWidget * parent ) {
	Q_ASSERT( result );

	CallSnipetDialogImpl dlg( database(), id, parent );
	if( dlg.exec() != QDialog::Accepted ) return false;
	if( ! executeSnipetScript( dlg.snipetText(), dlg.values(), result ) ) return false;
	return true;
}

bool SnipetManager::callSnipet( QString key, QString * result, QWidget * parent ) {
	Q_ASSERT( result );

	QSqlQuery searchId( "select id from snipets where lower(shortcut) = lower(:shortcut)", database() );
	searchId.bindValue( ":shortcut", key );

	if( ! searchId.exec() ) {
		qWarning( qPrintable( tr("Can't search id for key '%1' : %2").arg( key ).arg( searchId.lastError().text() ) ) );
		return false;
	}

	if( ! searchId.next() ) {
		// Id not found
		return false;
	}

	int snipet_id = searchId.value( 0 ).toInt();

	return callSnipet( snipet_id, result, parent );
}

bool SnipetManager::callAutomaticSnipet( QString key, QString * result, QWidget * parent ) {
	QSqlQuery searchId( "select id from snipets where lower(shortcut) = lower(:shortcut) and auto>=:auto", database() );
	searchId.bindValue( ":key", key );
	searchId.bindValue( ":auto", true );

	if( ! searchId.exec() ) {
		qWarning( qPrintable( tr("Can't search id for key '%1' : %2").arg( key ).arg( searchId.lastError().text() ) ) );
		return false;
	}

	if( ! searchId.next() ) {
		// Id not found
		return false;
	}

	int snipet_id = searchId.value( 0 ).toInt();

	return callSnipet( snipet_id, result, parent );
}


bool SnipetManager::executeSnipetScript( const QString & script, const QStringList & values, QString * result ) const {
	/* Process arguments */
	QString text = script;
	for( int i = 0 ; i < values.size() ; i++ ) {
		text = text.arg( values.at( i ) );
	}

	/* Process script */
	QScriptEngine & engine = ScriptManager::self()->engine();

	engine.pushContext();
	QRegExp jsString( "<\\?[^x][^m][^l].*\\?>" );
	jsString.setMinimal( true );
	int from = 0;
	QString processedString;
	while( jsString.indexIn( text, from ) >= 0 ) {
		processedString += text.mid( from, jsString.pos() - from );

		QString js = text.mid( jsString.pos() + 2, jsString.matchedLength() - 4 );
		if( js.at(0) == '=' ) {
			QScriptValue result = engine.evaluate( js.mid(1) );
			if( ! result.isError() )
				processedString += result.toString();
			else {
				qCritical( qPrintable( result.toString() ) );
				engine.popContext();
				return false;
			}
		} else {
			engine.evaluate( js ); // Only for define variable or make pre-process.
			if( engine.hasUncaughtException() ) {
				qCritical( qPrintable( engine.uncaughtException().toString() ) );
				engine.popContext();
				return false;
			}
		}
		from = jsString.pos() + jsString.matchedLength();
	}
	processedString += text.mid( from );

	engine.popContext();
	*result = processedString;
	return true;
}

bool SnipetManager::openDatabase() {
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

bool SnipetManager::createDatabase( QSqlDatabase db ) {
	QSqlQuery createQuery( db );
	/* Create tables */
	if( !
		createQuery.exec( "CREATE TABLE snipet_config ("
						  "    name TEXT PRIMARY KEY,"
						  "    value TEXT)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE TABLE categories ("
						  "    id INTEGER PRIMARY KEY,"
						  "    parent_id INTEGER NOT NULL DEFAULT(0),"
						  "    name TEXT NOT NULL,"
						  "    description TEXT,"
						  "    available_script TEXT)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE TABLE snipets ("
						  "    id INTEGER PRIMARY KEY,"
						  "    name TEXT,"
						  "    description TEXT,"
						  "    shortcut TEXT,"
						  "    icon TEXT,"
						  "    auto INTEGER,"
						  "    show_dialog INTEGER,"
						  "    text TEXT,"
						  "    available_script TEXT,"
						  "    category_id INTEGER  NOT NULL DEFAULT (0),"
						  "    FOREIGN KEY(category_id) REFERENCES category)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE TABLE snipets_extentions ("
						  "    id INTEGER PRIMARY KEY,"
						  "    snipet_id INTEGER NOT NULL,"
						  "    extention TEXT,"
						  "    FOREIGN KEY(snipet_id) REFERENCES snipet)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE TABLE snipets_params ("
						  "    id INTEGER PRIMARY KEY,"
						  "    snipet_id INTEGER NOT NULL,"
						  "    name TEXT,"
						  "    default_value TEXT,"
						  "    FOREIGN KEY(snipet_id) REFERENCES snipet)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	/* Create Index */
	if( !
		createQuery.exec( "CREATE INDEX categories_idx1 on categories (id ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE INDEX categories_idx2 on categories (parent_id ASC, name ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE INDEX snipets_idx1 on snipets (id ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE INDEX snipets_idx2 on snipets (name ASC, description ASC, shortcut ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE UNIQUE INDEX snipets_idx3 on snipets (name ASC, category_id ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE UNIQUE INDEX snipets_idx5 on snipets (shortcut ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE UNIQUE INDEX snipets_idx6 on snipets (shortcut ASC, auto ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE INDEX snipets_extentions_idx1 on snipets_extentions (snipet_id ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	if( !
		createQuery.exec( "CREATE INDEX snipets_params_idx1 on snipets_params (snipet_id ASC)" ) ) {
		qWarning( qPrintable( createQuery.lastError().text() ) );
		return false;
	}
	/* DATAS */
	if( !
	    createQuery.exec( "INSERT INTO snipet_config(name, value) VALUES( 'version', '1.0' )" ) ) {
		    qWarning( qPrintable( createQuery.lastError().text() ) );
		    return false;
	    }
	if( !
	    createQuery.exec( QString( "INSERT INTO categories(parent_id, name, description) VALUES( 0, '%1', '%2' )" ).arg( tr("Categories") ).arg( tr( "List of all categories" ) ) ) ) {
		    qWarning( qPrintable( createQuery.lastError().text() ) );
		    return false;
	    }
	return true;
}

void SnipetManager::closeDatabase() {
	QSqlDatabase db = database();
	db.close();
	QSqlDatabase::removeDatabase( "SNIPETBASE" );
}

