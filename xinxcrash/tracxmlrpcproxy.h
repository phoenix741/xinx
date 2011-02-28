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

#ifndef TRACXMLRPCPROXY_H
#define TRACXMLRPCPROXY_H

#include <QObject>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebPage>

const QString trac_serveur = "xinx.shadoware.org";

class TracXmlRpcProxy : public QObject
{
	Q_OBJECT
public:
    explicit TracXmlRpcProxy(QObject* parent = 0);
    virtual ~TracXmlRpcProxy();

public slots:
	void connection(const QString & username, const QString & password);
	void createTicket(const QString& summary, const QString& description, const QString& type, const QString& priority, const QString& component);
	void attachFile(int ticketId, const QString & filename, const QString & description, const QByteArray & datas);
	void registerUser(const QString& username, const QString& password, const QString& name, const QString& email);

	void getComponents();
	void getPriorities();
	void getTypes();

	void help(const QString & methodeName);
signals:
	void components(const QStringList & list);
	void priorities(const QStringList & list);
	void types(const QStringList & list);

	void ticketCreated(int id);
	void ticketCreationError(const QString & error);
	void fileAttached(const QString & filename);
	void fileAttachError(const QString & error);

	void connected();
	void connectionError(const QString & error);
private slots:
	void connectFinished();
	void componentsFinished();
	void prioritiesFinished();
	void typesFinished();
	void helpFinished();
	void createTicketFinished();
	void attachFileFinished();

	void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);

	void tryConnect(bool ok);
	void submitForm(bool ok);
private:
	QNetworkRequest getRpcRequest();

	QWebPage registerPage;
	QNetworkAccessManager* _network_manager;
	QString _username, _password, _email, _name;
};

#endif // TRACXMLRPCPROXY_H
