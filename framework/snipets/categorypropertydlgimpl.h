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

#ifndef _CATEGORYPROPERTYDLGIMPL_H_
#define _CATEGORYPROPERTYDLGIMPL_H_
#pragma once

// Xinx header
#include "ui_categorypropertydlg.h"
#include "snipets/categoryitemmodel.h"

// Qt header
#include <QDialog>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QSqlDatabase>

class CategoryPropertyDlgImpl : public QDialog, private Ui::CategoryPropertyDialog {
	Q_OBJECT
public:
	virtual ~CategoryPropertyDlgImpl();

	void setParentId( int id );
protected:
	CategoryPropertyDlgImpl( int categoryId, QSqlDatabase db, QWidget * parent = 0, Qt::WindowFlags f = Qt::MSWindowsFixedSizeDialogHint );
	CategoryPropertyDlgImpl( QSqlDatabase db, QWidget * parent = 0, Qt::WindowFlags f = Qt::MSWindowsFixedSizeDialogHint );

	void setupUi();
	void createMapper();
	void setCategoryAccess( bool value );
private slots:
	void on_m_categoryTreeView_activated ( const QModelIndex & index );
	void on_m_addCategoryButton_clicked();
	void on_m_removeCategoryButton_clicked();
	void on_m_buttons_accepted();
private:
	friend class SnipetManager;

	QSqlTableModel * m_categoryTableModel;
	QDataWidgetMapper * m_mapper;
	CategoryItemModel * m_categoryModel;
};

#endif /* _CATEGORYPROPERTYDLGIMPL_H_ */
