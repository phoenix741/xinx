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
#ifndef TRACTICKETCREATIONWIZARD_H
#define TRACTICKETCREATIONWIZARD_H

#include <QWizard>

const QString trac_serveur = "https://xinx.shadoware.org/";

namespace Ui {
	class TracTicketCreationWizard;
}

class TracTicketCreationWizard : public QWizard
{
	Q_OBJECT
public:
	explicit TracTicketCreationWizard(QWidget *parent = 0);
	~TracTicketCreationWizard();

	void setErrorMessage(const QString & message);
	const QString & errorMessage();

private:
	QScopedPointer<Ui::TracTicketCreationWizard> ui;
};

#endif // TRACTICKETCREATIONWIZARD_H
