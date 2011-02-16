/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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

#ifndef SERVICESBATCHDIALOGIMPL_H
#define SERVICESBATCHDIALOGIMPL_H

#include "ui_servicesbatch.h"
#include "webserviceseditor.h"

#include <QPushButton>

class ServicesBatchRow : public QTableWidgetItem
{
public:
	ServicesBatchRow(const QString & title) : QTableWidgetItem(title)
	{

	}

	WebServicesEditor * editor;
};

class ServicesBatchDialogImpl : public QDialog, public Ui::ServicesBatchDialog
{
	Q_OBJECT
public:
	explicit ServicesBatchDialogImpl(QWidget *parent = 0);

signals:

public slots:
	void launchWebServices();
	void resultReadyAt(int index);
private:
	QPushButton * m_launchButton;
	QVector<WebServicesEditorPtr> m_editors;
	QSignalMapper * m_signalMapper;
	int m_progression;
};

#endif // SERVICESBATCHDIALOGIMPL_H
