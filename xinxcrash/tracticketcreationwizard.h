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
#include <QSettings>

class TracXmlRpcProxy;

namespace Ui {
	class TracTicketCreationWizard;
}

class TracTicketCreationWizard : public QWizard
{
	Q_OBJECT
public:
	enum TracPageEnum {
		CRASH_PAGE			= 1,
		CONNECTION_PAGE		= 2,
		CREATE_LOGIN_PAGE	= 3,
		CREATE_TICKET_PAGE	= 4,
		ATTACHEMENT_PAGE	= 5,
		PROGRESS_PAGE		= 6,
		FINISH_PAGE			= 7
	};

	explicit TracTicketCreationWizard(QWidget *parent = 0);
	~TracTicketCreationWizard();

    virtual int nextId() const;

	QString errorMessage() const;
	const QString & version() const;
	const QString & login() const;
	const QString & password() const;
public slots:
	void setVersion(const QString & version);
	void setErrorMessage(const QString & message);
	void setLogin(const QString & login);
	void setPassword(const QString & password);
protected:
    virtual void initializePage(int id);
    virtual void done(int result);
private:
	QScopedPointer<Ui::TracTicketCreationWizard> _ui;
    QString _version, _login, _password;
	QSettings _settings;
    TracXmlRpcProxy* _xmlrpc;
};

#endif // TRACTICKETCREATIONWIZARD_H
