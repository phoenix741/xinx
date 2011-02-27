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

const QString trac_serveur = "xinx.shadoware.org";

class TracXmlRpcProxy : public QObject
{
	Q_OBJECT
public:
    explicit TracXmlRpcProxy(QObject* parent = 0);
    virtual ~TracXmlRpcProxy();

	void connection(const QString & username, const QString & password);

	QStringList components() const;
	QStringList priorities() const;
	QStringList types() const;

	QString getVersion(const QString & version);
signals:
	void connected();
	void connectionError(const QString & error);
private slots:
	void connectFinished();

	void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);
private:
	QNetworkRequest getRpcRequest();

	QNetworkAccessManager* _network_manager;
	QString _username;
	QString _password;
};

#endif // TRACXMLRPCPROXY_H
