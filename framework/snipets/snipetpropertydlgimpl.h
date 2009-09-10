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

#ifndef _SNIPETPROPERTYDLGIMPL_H_
#define _SNIPETPROPERTYDLGIMPL_H_
#pragma once

// Xinx header
#include "ui_snipetpropertydlg.h"

// Qt header
#include <QDialog>
#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QSqlDatabase>

class SnipetPropertyDlgImpl : public QDialog, private Ui::SnipetPropertyDialog {
	Q_OBJECT
public:
	enum {
		Snipets_Id = 0,
		Snipets_Name = 1,
		Snipets_Description = 2,
		Snipets_Shortcut = 3,
		Snipets_Icon = 4,
		Snipets_Auto = 5,
		Snipets_Text = 6,
		Snipets_AvailableScript = 7,
		Snipets_Order = 8,
		Snipets_Category = 9
	};

	virtual ~SnipetPropertyDlgImpl();
protected:
	SnipetPropertyDlgImpl( int snipetId, QSqlDatabase db, QWidget * parent = 0, Qt::WindowFlags f = Qt::MSWindowsFixedSizeDialogHint );

	friend class SnipetDatabaseManager;
private:
	QSqlRelationalTableModel * m_snipetModel;
	QDataWidgetMapper * m_mapper;
};

#endif /* _SNIPETPROPERTYDLGIMPL_H_ */
