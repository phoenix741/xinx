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
#include <QWebFrame>
#include <QWebElement>
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

			if (!((major == 1) && (minor == 1) && (build == 0)))
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
		emit connectionError(reply->errorString());
	}
}

void TracXmlRpcProxy::helpFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::helpFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();
		qDebug() << "Reply " << replyContent;
	}
}

void TracXmlRpcProxy::help(const QString& methodeName)
{
	QNetworkRequest rpcRequest = getRpcRequest();
	XmlRpcRequest request("system.methodHelp");
	request.addParameters(methodeName);
	QByteArray rpcBody = request.getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(helpFinished()));
}

void TracXmlRpcProxy::componentsFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::componentsFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();

		qDebug() << "Reply " << replyContent;

		XmlRpcResponse response(replyContent);
		if (response.faultCode() == -1)
		{
			QVariant parameter1 = response.parameters().at(0);
			QStringList list = parameter1.toStringList();
			emit components(list);
		}
	}
}

void TracXmlRpcProxy::getComponents()
{
	QNetworkRequest rpcRequest = getRpcRequest();
	QByteArray rpcBody = XmlRpcRequest("ticket.component.getAll").getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(componentsFinished()));
}

void TracXmlRpcProxy::prioritiesFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::prioritiesFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();

		qDebug() << "Reply " << replyContent;

		XmlRpcResponse response(replyContent);
		if (response.faultCode() == -1)
		{
			QVariant parameter1 = response.parameters().at(0);
			QStringList list = parameter1.toStringList();
			emit priorities(list);
		}
	}
}

void TracXmlRpcProxy::getPriorities()
{
	QNetworkRequest rpcRequest = getRpcRequest();
	QByteArray rpcBody = XmlRpcRequest("ticket.priority.getAll").getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(prioritiesFinished()));
}

void TracXmlRpcProxy::typesFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::typesFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();

		qDebug() << "Reply " << replyContent;

		XmlRpcResponse response(replyContent);
		if (response.faultCode() == -1)
		{
			QVariant parameter1 = response.parameters().at(0);
			QStringList list = parameter1.toStringList();
			emit types(list);
		}
	}
}

void TracXmlRpcProxy::getTypes()
{
	QNetworkRequest rpcRequest = getRpcRequest();
	QByteArray rpcBody = XmlRpcRequest("ticket.type.getAll").getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(typesFinished()));
}

void TracXmlRpcProxy::versionsFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::typesFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();

		qDebug() << "Reply " << replyContent;

		XmlRpcResponse response(replyContent);
		if (response.faultCode() == -1)
		{
			QVariant parameter1 = response.parameters().at(0);
			QStringList list = parameter1.toStringList();
			emit versions(list);
		}
	}
}

void TracXmlRpcProxy::getVersions()
{
	QNetworkRequest rpcRequest = getRpcRequest();
	QByteArray rpcBody = XmlRpcRequest("ticket.version.getAll").getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(versionsFinished()));
}

void TracXmlRpcProxy::createTicketFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::typesFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();

		qDebug() << "Reply " << replyContent;

		XmlRpcResponse response(replyContent);
		if (response.faultCode() == -1)
		{
			QVariant parameter1 = response.parameters().at(0);
			int id = parameter1.toInt();
			emit ticketCreated(id);
		}
		else
		{
			emit ticketCreationError(response.faultString());
		}
	}
	else
	{
		emit ticketCreationError(reply->errorString());
	}
}

void TracXmlRpcProxy::createTicket(const QString& summary, const QString& description, const QString& type, const QString& priority, const QString& component)
{
	QNetworkRequest rpcRequest = getRpcRequest();
	XmlRpcRequest request("ticket.create");
	request.addParameters(summary);
	request.addParameters(description);
	QVariantMap structure;
	structure.insert("type", type);
	structure.insert("component", component);
	structure.insert("priority", priority);
	request.addParameters(structure);
	request.addParameters(true);

	QByteArray rpcBody = request.getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(createTicketFinished()));

}

void TracXmlRpcProxy::attachFileFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT_X(reply, "TracXmlRpcProxy::attachFileFinished", "Reply must exist");

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyContent = reply->readAll();

		qDebug() << "Reply " << replyContent;

		XmlRpcResponse response(replyContent);
		if (response.faultCode() == -1)
		{
			QVariant parameter1 = response.parameters().at(0);
			const QString filename = parameter1.toString();
			emit fileAttached(filename);
		}
		else
		{
			emit fileAttachError(response.faultString());
		}
	}
	else
	{
		emit fileAttachError(reply->errorString());
	}
}

void TracXmlRpcProxy::attachFile(int ticketId, const QString& filename, const QString& description, const QByteArray& datas)
{
	QNetworkRequest rpcRequest = getRpcRequest();
	XmlRpcRequest request("ticket.putAttachment");
	request.addParameters(ticketId);
	request.addParameters(filename);
	request.addParameters(description);
	request.addParameters(datas);

	QByteArray rpcBody = request.getRequest();

	qDebug() << "Request is ";
	qDebug() << rpcRequest.url();
	qDebug() << rpcBody;

	QNetworkReply * reply = _network_manager->post(rpcRequest, rpcBody);
	connect(reply, SIGNAL(finished()), this, SLOT(attachFileFinished()));
}

void TracXmlRpcProxy::sslErrors(QNetworkReply* reply, const QList< QSslError >& errors)
{
	reply->ignoreSslErrors();
}

void TracXmlRpcProxy::tryConnect(bool ok)
{
	qDebug() << "Result page : " << registerPage.mainFrame()->toPlainText();
	qDebug() << "Try connect the user";

	connection(_username, _password);
}

void TracXmlRpcProxy::submitForm(bool ok)
{
	qDebug() << "Try to submit form";
	disconnect(&registerPage, SIGNAL(loadFinished(bool)), this, SLOT(submitForm(bool)));
	connect(&registerPage, SIGNAL(loadFinished(bool)), this, SLOT(tryConnect(bool)));

	if (!ok)
	{
		emit connectionError(tr("Can't create the user."));
		return;
	}


	QWebFrame * frame = registerPage.mainFrame();

	frame->evaluateJavaScript(QString("document.getElementsByName('user')[0].value = '%1';").arg(_username));
	frame->evaluateJavaScript(QString("document.getElementsByName('password')[0].value = '%1';").arg(_password));
	frame->evaluateJavaScript(QString("document.getElementsByName('password_confirm')[0].value = '%1';").arg(_password));
	frame->evaluateJavaScript(QString("document.getElementsByName('name')[0].value = '%1';").arg(_name));
	frame->evaluateJavaScript(QString("document.getElementsByName('email')[0].value = '%1';").arg(_email));


	QWebElement submitForm = frame->findFirstElement("#acctmgr_registerform");
	submitForm.evaluateJavaScript("this.submit()");
}

void TracXmlRpcProxy::registerUser(const QString& username, const QString& password, const QString& name, const QString& email)
{
	_username = username;
	_password = password;
	_name = name;
	_email = email;

	QWebSettings * webSettings = registerPage.settings();
	webSettings->setAttribute(QWebSettings::AutoLoadImages, false);
	webSettings->setAttribute(QWebSettings::PluginsEnabled, false);
	webSettings->setAttribute(QWebSettings::JavaEnabled, false);
	webSettings->setAttribute(QWebSettings::JavascriptEnabled, true);
	webSettings->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);

	registerPage.setNetworkAccessManager(_network_manager);

	connect(&registerPage, SIGNAL(loadFinished(bool)), this, SLOT(submitForm(bool)));

	registerPage.mainFrame()->load("https://" + trac_serveur + "/register");
}
