/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#ifndef SNIPETMENU_H
#define SNIPETMENU_H

// Xinx header
#include <core/lib-config.h>
#include <qmenuview.h>
#include <snipets/snipetitemmodel.h>

class LIBEXPORT SnipetMenuModel : public SnipetItemModel
{
	Q_OBJECT
public:
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
protected:
	friend class SnipetManager;

	explicit SnipetMenuModel(QSqlDatabase db, QObject * parent = 0);
private:
};

/* SnipetMenu */

class LIBEXPORT SnipetMenu : public QMenuView
{
	Q_OBJECT
public:
	explicit SnipetMenu(QWidget * parent = 0);
	virtual ~SnipetMenu();
signals:
	void snipetTriggered(int snipetId) const;
protected:
	virtual bool prePopulated();
private slots:
	void snipetTriggered(const QModelIndex & index) const;
	void createSnipet() const;
private:
	QAction * m_createAction;
};

#endif /* SNIPETMENU_H */
