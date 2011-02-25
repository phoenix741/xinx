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

#include "tracticketcreationwizard.h"
#include "ui_tracticketcreationwizard.h"

TracTicketCreationWizard::TracTicketCreationWizard(QWidget *parent) : QWizard(parent), _ui(new Ui::TracTicketCreationWizard)
{
	_ui->setupUi(this);
	_ui->registerView->load (trac_serveur + "register");
}

TracTicketCreationWizard::~TracTicketCreationWizard()
{

}

void TracTicketCreationWizard::setErrorMessage(const QString& message)
{
	_ui->crashMessage->setText(message);
}

QString TracTicketCreationWizard::errorMessage() const
{
	return _ui->crashMessage->text();
}

void TracTicketCreationWizard::setVersion(const QString& version)
{
	_version = version;
}

const QString& TracTicketCreationWizard::version() const
{
	return _version;
}
