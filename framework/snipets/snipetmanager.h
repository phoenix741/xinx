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

#ifndef _SNIPETMANAGER_H_
#define _SNIPETMANAGER_H_
#pragma once

// Xinx header
#include "snipets/snipetlist.h"

// Qt header
#include <QObject>
#include <QSqlDatabase>

class CategoryItemModel;
class SnipetItemModel;
class SnipetMenu;

/*!
 * The goal of SnipetDataBaseManager is too manage the new snipet interface
 * of XINX. This interface create table if necessary in the SQL database.
 */
class SnipetDatabaseManager : public QObject {
	Q_OBJECT
public:
	~SnipetDatabaseManager();

	static SnipetDatabaseManager * self();

	QSqlDatabase database();

	CategoryItemModel * createCategoryItemModel( QObject * parent = 0 );
	SnipetItemModel * createSnipetItemModel( QObject * parent = 0 );
	SnipetMenu * createSnipetMenu( const QString & title = QString(), QWidget * parent = 0 );

	void addCategory( int parentId = 0, QWidget * parent = 0 );
	void removeCategory( int id, QWidget * parent = 0 );
	void modifyCategory( int id, QWidget * parent = 0 );

	void modifySnipet( int id, QWidget * parent = 0 );
	bool removeSnipet( int id, QWidget * parent = 0 );
	bool importSnipetList( const SnipetList & list, QWidget * parent = 0 );
	bool callSnipet( int id, QString * result, QWidget * parent = 0 );
	bool callSnipet( QString key, QString * result, QWidget * parent = 0 );
	bool callAutomaticSnipet( QString key, QString * result, QWidget * parent = 0 );

	bool executeSnipetScript( const QString & script, const QStringList & values, QString * result ) const;

	int getCategoryId( const QStringList & category );
private:
	SnipetDatabaseManager();
	bool openDatabase();
	bool createDatabase( QSqlDatabase db );
	void closeDatabase();

	static SnipetDatabaseManager * s_self;
};

#endif /* _SNIPETMANAGER_H_ */
