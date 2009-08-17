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
#include "snipets/snipetmodelindex.h"

// Qt header
#include <QObject>
#include <QSqlDatabase>

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

	SnipetItemModel * createSnipetItemModel( QObject * parent = 0 );
private:
	SnipetDatabaseManager();
	bool openDatabase();
	bool createDatabase( QSqlDatabase db );
	void closeDatabase();

	static SnipetDatabaseManager * s_self;
};

#endif /* _SNIPETMANAGER_H_ */
