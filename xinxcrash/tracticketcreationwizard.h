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
#include <QQueue>

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

	const QString & errorMessage() const;
	const QString & version() const;
	const QStringList & stacktrace() const;
	const QString & login() const;
	const QString & password() const;

    virtual void setVisible(bool visible);
public slots:
	void setVersion(const QString & version);
	void setErrorMessage(const QString & message);
	void setStacktrace(const QStringList & stack);
	void setLogin(const QString & login);
	void setPassword(const QString & password);
protected:
    virtual void initializePage(int id);
    virtual void done(int result);
    virtual bool validateCurrentPage();
private slots:
	void connected();
	void connectionError(const QString & text);
	void components(const QStringList & list);
	void types(const QStringList & list);
	void priorities(const QStringList & list);
	void versions(const QStringList & list);

	void slotCurrentIdChanged(int);
	void slotAddAttachement();
	void slotCreateAccountToggled(bool);

	void ticketCreated(int id);
	void ticketCreationError(const QString & error);
	void fileAttached(const QString & filename);
	void fileAttachError(const QString & error);
private:
	QScopedPointer<Ui::TracTicketCreationWizard> _ui;

	int _ticket_id;
	bool _is_ticket_created;
	QQueue<QString> _attachements;

	QString _message;
	QStringList _stack_trace;
	bool _connected, _connecting, _next_page;
    QString _version, _string_version, _login, _password;
	QSettings _settings;
    TracXmlRpcProxy* _xmlrpc;
};

#endif // TRACTICKETCREATIONWIZARD_H
