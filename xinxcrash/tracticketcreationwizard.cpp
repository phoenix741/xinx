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
#include "tracxmlrpcproxy.h"

TracTicketCreationWizard::TracTicketCreationWizard(QWidget *parent) : QWizard(parent), _ui(new Ui::TracTicketCreationWizard)
{
	_ui->setupUi(this);

	_settings.beginGroup("Ticket Settings");
	_login		= _settings.value("Login").toString();
	_password	= _settings.value("Password").toString();
	_settings.endGroup();

	setErrorMessage(QString());

	connect(_ui->loginEdit, SIGNAL(textChanged(QString)), this, SLOT(setLogin(QString)));
	connect(_ui->passwordEdit, SIGNAL(textChanged(QString)), this, SLOT(setPassword(QString)));

	_xmlrpc = new TracXmlRpcProxy(this);
}

TracTicketCreationWizard::~TracTicketCreationWizard()
{

}

void TracTicketCreationWizard::setErrorMessage(const QString& message)
{
	_ui->crashMessage->setText(message);
	if (! message.isEmpty())
	{
		setStartId(CRASH_PAGE);
	}
	else
	{
		if (! (_login.isEmpty() || _password.isEmpty()))
		{
			setStartId(CREATE_TICKET_PAGE);
		}
		else
		{
			setStartId(CONNECTION_PAGE);
		}
	}
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

void TracTicketCreationWizard::setLogin(const QString& login)
{
	if (login != _login)
	{
		_login = login;
		_ui->loginEdit->setText(login);
	}
}

const QString& TracTicketCreationWizard::login() const
{
	return _login;
}

void TracTicketCreationWizard::setPassword(const QString& password)
{
	if (_password != password)
	{
		_password = password;
		_ui->passwordEdit->setText(password);
	}
}

const QString& TracTicketCreationWizard::password() const
{
	return _password;
}

void TracTicketCreationWizard::initializePage(int id)
{
	switch(id)
	{
	case CONNECTION_PAGE:
		_ui->connectionProgressBar->setVisible(false);
	case CREATE_LOGIN_PAGE:
		break;
	}
}

int TracTicketCreationWizard::nextId() const
{
    if (currentPage() == _ui->crashPage)
	{
		if (! (_login.isEmpty() || _password.isEmpty()))
		{
			return CREATE_TICKET_PAGE;
		}
		else
		{
			return CONNECTION_PAGE;
		}
	}
	else if (currentPage() == _ui->connectionPage)
	{
		if (_ui->loginRadio->isChecked())
		{
			_ui->connectionProgressBar->setVisible(false);
			_ui->connectionProgressBar->setMinimum(0);
			_ui->connectionProgressBar->setMaximum(0);

			_xmlrpc->connection(_login, _password);

			return CREATE_TICKET_PAGE;
		}
		else
		{
			return CREATE_LOGIN_PAGE;
		}
	}
	else if (currentPage() == _ui->createLoginPage)
	{
		return CREATE_TICKET_PAGE;
	}
	else if (currentPage() == _ui->createTicket)
	{
		return ATTACHEMENT_PAGE;
	}
	else if (currentPage() == _ui->attachementPage)
	{
		return PROGRESS_PAGE;
	}
	else if (currentPage() == _ui->progressPage)
	{
		return FINISH_PAGE;
	}
	else if (currentPage() == _ui->finishPage)
	{
		return -1;
	}
	else
	{
		return -1;
	}
}

void TracTicketCreationWizard::done(int result)
{
	if (result == QDialog::Accepted)
	{
		_settings.beginGroup("Ticket Settings");
		_settings.setValue("Login", _login);
		_settings.setValue("Password", _password);
		_settings.endGroup();
	}


    QWizard::done(result);
}
