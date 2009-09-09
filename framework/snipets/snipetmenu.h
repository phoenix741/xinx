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

#ifndef SNIPETMENU_H
#define SNIPETMENU_H
#pragma once

// Xinx header
#include <qmenuview.h>
#include "snipets/snipetmodelindex.h"

class SnipetMenuModel : public SnipetItemModel {
	Q_OBJECT
public:
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
protected:
	friend class SnipetDatabaseManager;

	SnipetMenuModel( QSqlDatabase db, QObject * parent = 0 );
private:
};

/* SnipetMenu */

class SnipetMenu : public QMenuView {
	Q_OBJECT
public:
	SnipetMenu( QWidget * parent = 0 );
	virtual ~SnipetMenu();
signals:
	void snipetTriggered( int snipetId ) const;
protected:
	virtual bool prePopulated();
private slots:
	void snipetTriggered( const QModelIndex & index ) const;
private:
	QAction * m_createAction;
};

#endif /* SNIPETMENU_H */
