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

#ifndef __CALLSNIPETDIALOG_H__
#define __CALLSNIPETDIALOG_H__
#pragma once

// Xinx header
#include "ui_callsnipetdlg.h"

// Qt header
#include <QDialog>
#include <QSqlDatabase>
#include <QTableWidgetItem>

/*!
 * \internal
 */
class SnipetParameterNameItem : public QTableWidgetItem {
public:
	SnipetParameterNameItem();
	SnipetParameterNameItem( const QString & name );
private:

};

/*!
 * \internal
 */
class SnipetParameterValueItem : public QTableWidgetItem {
public:
	SnipetParameterValueItem();
	SnipetParameterValueItem( const QString & defaultValue );

	void setDefault();
	void setDefaultValue( const QString & value );
	const QString & defaultValue() const;
private:
	QString m_defaultValue;
};

/*!
 * Implementation of snipet dialog. This dialog permit to create, modify.
 * The implementation containts only a constructor who defines default dialog
 * presentation : Windows Style Dialog.
 */
class CallSnipetDialogImpl : public QDialog, public Ui::CallSnipetDialog {
	Q_OBJECT
public:
	/*!
	 * Constructor of the snipet dialog implementation. We defines a default windows flags.
	 * The windows can't be resize.
	 * \param text Text used for the template
	 * \param parent The parent of the dialog
	 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
	 */
	CallSnipetDialogImpl( QSqlDatabase db, int snipetId, QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	/*!
	 * Destroy the dialog.
	 */
	virtual ~CallSnipetDialogImpl();
private slots:
	void m_resultTabWidget_currentChanged( int );
private:
	void setupUi( QSqlDatabase db, int snipetId, QDialog * parent );

	QString m_snipetText;
};

#endif // __CALLSNIPETDIALOG_H__
