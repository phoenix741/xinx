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
#include <QDebug>
#include <QMessageBox>

TracTicketCreationWizard::TracTicketCreationWizard(QWidget *parent) : QWizard(parent), _ui(new Ui::TracTicketCreationWizard)
{
	_ui->setupUi(this);

	_ui->attachementPage->setCommitPage(true);
	_ui->attachFile->setDirectory(false);
	connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)));
	connect(_ui->addButton, SIGNAL(clicked(bool)), this, SLOT(slotAddAttachement()));

	_settings.beginGroup("Ticket Settings");
	_login		= _settings.value("Login").toString();
	_password	= _settings.value("Password").toString();
	_settings.endGroup();

	setErrorMessage(QString());

	connect(_ui->loginEdit, SIGNAL(textChanged(QString)), this, SLOT(setLogin(QString)));
	connect(_ui->passwordEdit, SIGNAL(textChanged(QString)), this, SLOT(setPassword(QString)));

	_xmlrpc = new TracXmlRpcProxy(this);

	connect(_xmlrpc, SIGNAL(connected()), this, SLOT(connected()));
	connect(_xmlrpc, SIGNAL(connectionError(QString)), this, SLOT(connectionError(QString)));
	connect(_xmlrpc, SIGNAL(components(QStringList)), this, SLOT(components(QStringList)));
	connect(_xmlrpc, SIGNAL(types(QStringList)), this, SLOT(types(QStringList)));
	connect(_xmlrpc, SIGNAL(priorities(QStringList)), this, SLOT(priorities(QStringList)));
	connect(_xmlrpc, SIGNAL(versions(QStringList)), this, SLOT(versions(QStringList)));

	connect(_xmlrpc, SIGNAL(ticketCreated(int)), this, SLOT(ticketCreated(int)));
	connect(_xmlrpc, SIGNAL(ticketCreationError(QString)), this, SLOT(ticketCreationError(QString)));

	connect(_xmlrpc, SIGNAL(fileAttached(QString)), this, SLOT(fileAttached(QString)));
	connect(_xmlrpc, SIGNAL(fileAttachError(QString)), this, SLOT(fileAttachError(QString)));
}

TracTicketCreationWizard::~TracTicketCreationWizard()
{

}

void TracTicketCreationWizard::setErrorMessage(const QString& message)
{
	_ui->crashMessage->setText(tr("The application crash with the following message. This assistant will help you "
								  "to create a new ticket to help us to have a stable application. Please add the "
								  "maximum information you have.\n\n%1").arg(message));
	_ui->descriptionEdit->setPlainText(message.isEmpty() ? "" : QString("\n\n---------\n%1 : %2").arg(tr("Detailled message")).arg(message));
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
		_connected = false;
		_ui->connectionProgressBar->setVisible(false);
		break;
	case CREATE_LOGIN_PAGE:
		_ui->createUserProgressBar->setVisible(false);
		break;
	case PROGRESS_PAGE:
		_is_ticket_created = false;
		break;
	case FINISH_PAGE:
		_ui->completeLabel->setText(tr("The ticket as completely created with the id %1. You can add more information on <a href=\"http://%2/ticket/%1\">http://%2/ticket/%1</a>.").arg(_ticket_id).arg(trac_serveur));
		break;
	}
}

void TracTicketCreationWizard::components(const QStringList& list)
{
	_ui->componentsCombo->clear();
	_ui->componentsCombo->addItems(list);
	_ui->componentsCombo->setCurrentIndex(_ui->componentsCombo->findText("XINX"));
}

void TracTicketCreationWizard::priorities(const QStringList& list)
{
	_ui->prioritiesCombo->clear();
	_ui->prioritiesCombo->addItems(list);
	_ui->prioritiesCombo->setCurrentIndex(_ui->prioritiesCombo->findText("normal"));
}

void TracTicketCreationWizard::types(const QStringList& list)
{
	_ui->typeCombo->clear();
	_ui->typeCombo->addItems(list);
	_ui->typeCombo->setCurrentIndex(_ui->typeCombo->findText("defect"));
}

static bool versionCompare(const QString &s1, const QString &s2)
{
	// Cas spécifique
	if (s1 == "trunk-svn" && s2 == "trunk-svn")
	{
		return false;
	}
	else if (s1 == "trunk-svn")
	{
		return false;
	}
	else if (s2 == "trunk-svn")
	{
		return true;
	}

	// Numéro de version standard
	QStringList sl1 = s1.split (".");
	QStringList sl2 = s2.split (".");

	int i = 0;
	while (true)
	{
		int v1 = i < sl1.size () ? sl1.at (i).toInt() : 0;
		int v2 = i < sl2.size () ? sl2.at (i).toInt() : 0;

		if (v1 < v2)
		{
			return true;
		}
		else if (v1 > v2)
		{
			return false;
		}

		if ((i >= sl1.size ()) || (i >= sl2.size ()))
			break;

		i++;
	}
	return false;
}

void TracTicketCreationWizard::versions(const QStringList& list)
{
	QStringList versions = list;
	qSort(versions.begin (), versions.end (), versionCompare);
	QStringList::const_iterator it = qLowerBound(versions.constBegin (), versions.constEnd (), _version, versionCompare);
	_ui->versionLabel->setText(*it);
	_version = *it;
}

void TracTicketCreationWizard::connected()
{
	qDebug() << "TracTicketCreationWizard::connected";
	_ui->connectionProgressBar->setMaximum(1);
	_ui->connectionProgressBar->setValue(1);
	_ui->connectionProgressBar->setVisible(false);

	_connected = true;

	_xmlrpc->getComponents();
	_xmlrpc->getPriorities();
	_xmlrpc->getTypes();
	if (! _version.isEmpty ())
	{
		_xmlrpc->getVersions ();
	}
	else
	{
		_ui->versionLabel->setText("trunk-svn");
		_version = "trunk-svn";
	}

	next();
}

void TracTicketCreationWizard::connectionError(const QString& text)
{
	_ui->createUserProgressBar->setVisible(false);
	_ui->connectionProgressBar->setVisible(false);
	QMessageBox::critical(this, tr("Connection error"), tr("Can't connect to the ticket system : \"%1\".").arg(text));
}

void TracTicketCreationWizard::ticketCreated(int id)
{
	_ticket_id = id;

	_ui->progressBar->setValue(_ui->progressBar->value() + 1);

	if (_attachements.count())
	{
		const QString filename = _attachements.dequeue();
		QFile file(filename);
		if (file.open(QFile::ReadOnly))
		{
			_xmlrpc->attachFile(_ticket_id, QFileInfo(filename).fileName(), QString(), file.readAll());
		}
		else
		{
			ticketCreationError(file.errorString());
		}
	}
	else
	{
		_is_ticket_created = true;
		next();
	}
}

void TracTicketCreationWizard::ticketCreationError(const QString& error)
{
	QMessageBox::critical(this, tr("Creation error"), tr("Can't create the ticket : \"%1\"").arg(error));
}

void TracTicketCreationWizard::fileAttached(const QString& filename)
{
	_ui->progressBar->setValue(_ui->progressBar->value() + 1);

	ticketCreated(_ticket_id);
}

void TracTicketCreationWizard::fileAttachError(const QString& error)
{
	QMessageBox::critical(this, tr("Creation error"), tr("Can't attach the file to the ticket : \"%1\"").arg(error));
	ticketCreated(_ticket_id);
}

void TracTicketCreationWizard::slotCurrentIdChanged(int ticketId)
{
	if (ticketId == PROGRESS_PAGE)
	{
		_ui->progressBar->setMaximum(_attachements.count() + 1);
		_ui->progressBar->setValue(0);

		_xmlrpc->createTicket(_ui->titleEdit->text(), _ui->descriptionEdit->toPlainText(), _ui->typeCombo->currentText(), _ui->prioritiesCombo->currentText(), _ui->componentsCombo->currentText());
	}
}

void TracTicketCreationWizard::slotAddAttachement()
{
	_ui->attachementList->addItem(new QListWidgetItem(_ui->attachFile->lineEdit()->text()));
	_ui->attachFile->lineEdit()->clear();
}

bool TracTicketCreationWizard::validateCurrentPage()
{
	qDebug() << "TracTicketCreationWizard::validateCurrentPage";
	if ((currentPage() == _ui->connectionPage) && !_connected && _ui->loginRadio->isChecked())
	{
		_ui->connectionProgressBar->setVisible(true);
		_ui->connectionProgressBar->setMinimum(0);
		_ui->connectionProgressBar->setMaximum(0);

		_xmlrpc->connection(_login, _password);

		return false;
	}
	else if(currentPage() == _ui->createTicketPage)
	{
		if (_ui->titleEdit->text().isEmpty())
		{
			QMessageBox::critical(this, tr("Create ticket"), tr("You must write a title"));
			return false;
		}
		if (_ui->descriptionEdit->toPlainText().isEmpty())
		{
			QMessageBox::critical(this, tr("Create ticket"), tr("You must write a description"));
			return false;
		}
	}
	else if(currentPage() == _ui->attachementPage)
	{
		_attachements.clear();
		for(int i = 0; i < _ui->attachementList->count(); i++)
		{
			_attachements.enqueue(_ui->attachementList->item(i)->text());
		}
	}
	else if((currentPage() == _ui->progressPage) && !_is_ticket_created)
	{
		return false;
	}
	else if((currentPage() == _ui->createLoginPage) && !_connected)
	{
		if (_ui->newPasswordEdit->text() != _ui->newPassword2Ediit->text())
		{
			QMessageBox::critical(this, tr("Register"), tr("Password must be equals"));
		}
		else
		{
			_ui->createUserProgressBar->setVisible(true);
			_ui->createUserProgressBar->setMaximum(0);
			_ui->createUserProgressBar->setValue(0);

			_xmlrpc->registerUser(_ui->newLoginEdit->text(), _ui->newPasswordEdit->text(), _ui->newNameEdit->text(), _ui->newEmailEdit->text());
		}
		return false;
	}

	return QWizard::validateCurrentPage();
}

int TracTicketCreationWizard::nextId() const
{
	qDebug() << "TracTicketCreationWizard::nextId";
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
	else if (currentPage() == _ui->createTicketPage)
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
