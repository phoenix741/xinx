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

class CallSnipetDialogImpl : public QDialog, public Ui::CallSnipetDialog
{
	Q_OBJECT
public:
	CallSnipetDialogImpl(QSqlDatabase db, int snipetId, QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint);
	virtual ~CallSnipetDialogImpl();

	const QString & snipetText() const;
	QStringList values() const;

	int exec();
private slots:
	void on_m_tabWidget_currentChanged(int);
private:
	void setupUi(QSqlDatabase db, int snipetId, QDialog * parent);

	QString m_snipetText;
	bool m_isAutomatic, m_showDialog;
};

#endif // __CALLSNIPETDIALOG_H__
