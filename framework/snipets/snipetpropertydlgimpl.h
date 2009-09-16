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
#include "snipets/categoryitemmodel.h"

// Qt header
#include <QDialog>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QSqlDatabase>

class SnipetPropertyDlgImpl : public QDialog, private Ui::SnipetPropertyDialog {
	Q_OBJECT
public:
	virtual ~SnipetPropertyDlgImpl();

	void setParentId( int id );
	void duplicate();
protected:
	SnipetPropertyDlgImpl( QSqlDatabase db, QWidget * parent = 0, Qt::WindowFlags f = Qt::MSWindowsFixedSizeDialogHint );
	SnipetPropertyDlgImpl( int snipetId, QSqlDatabase db, QWidget * parent = 0, Qt::WindowFlags f = Qt::MSWindowsFixedSizeDialogHint );

	void setupUi();
	void createMapper();
private slots:
	void on_m_categoryTreeView_activated ( const QModelIndex & index );
	void on_m_addCategoryButton_clicked();
	void on_m_removeCategoryButton_clicked();
	void on_m_buttons_accepted();
	void m_mapper_currentIndexChanged( int index );
private:
	enum {
		snipet_params_id = 0,
		snipet_params_snipet_id = 1,
		snipet_params_name = 2,
		snipet_params_default_value = 3,
		snipet_params_order = 4
	};
	friend class SnipetDatabaseManager;

	QSqlDatabase m_db;
	QSqlTableModel * m_snipetModel, * m_paramsModel;
	QDataWidgetMapper * m_mapper;
	CategoryItemModel * m_categoryModel;
	int m_id;
};

#endif /* _SNIPETPROPERTYDLGIMPL_H_ */
