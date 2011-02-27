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

#include "tracxmlrpcproxy.h"
#include <QNetworkReply>
#include <QXmlStreamWriter>
#include <QDebug>
#include "xmlrpcrequest.h"
#include "xmlrpcresponse.h"

TracXmlRpcProxy::TracXmlRpcProxy(QObject* parent): QObject(parent)
{
	_network_manager = new QNetworkAccessManager(this);

	connect(_network_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*, QList<QSslError>)));
}

TracXmlRpcProxy::~TracXmlRpcProxy()
{

}

QNetworkRequest TracXmlRpcProxy::getRpcRequest()
{
	QUrl url;
	url.setScheme("https");
	url.setUserName(_username);
	url.setPassword(_password);
	url.setHost(trac_serveur);
	url.setPath("/login/rpc");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");

	return request;
}

void TracXmlRpcProxy::connection(const QString& username, const QString& password)
{
	_username = username;
	_password = password;
	QNetworkRequest rpcRequest = getRpcRequest();
	QByteArray rpcBody = XmlRpcRequest("system.getAPIVersion").getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(connectFinished()));
}

void TracXmlRpcProxy::connectFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::connectFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();

		qDebug() << "Reply " << replyContent;

		XmlRpcResponse response(replyContent);
		if (response.faultCode() == -1)
		{
			QVariant parameter1 = response.parameters().at(0);
			QVariantList list = parameter1.toList();
			int major = list.at(0).toInt();
			int minor = list.at(1).toInt();
			int build = list.at(2).toInt();

			qDebug() << major << minor << build;

			if (!((major == 1) && (minor == 1) && (build = 0)))
			{
				emit connectionError(tr("Wrong version of XML RPC"));
			}
			else
			{
				emit connected();
			}
		}
		else
		{
			emit connectionError(response.faultString());
		}
	}
	else
	{
		emit connectionError(tr("Can't connect to serveur : error %1 ").arg(reply->error()));
	}
}

QStringList TracXmlRpcProxy::components() const
{
	return QStringList();
}

QStringList TracXmlRpcProxy::priorities() const
{
	return QStringList();
}

QStringList TracXmlRpcProxy::types() const
{
	return QStringList();
}

QString TracXmlRpcProxy::getVersion(const QString& version)
{
	return QString();
}

void TracXmlRpcProxy::sslErrors(QNetworkReply* reply, const QList< QSslError >& errors)
{
	reply->ignoreSslErrors();
}
