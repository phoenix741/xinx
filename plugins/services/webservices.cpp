/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Qt header
#include <QBuffer>
#include <QFile>
#include <QIODevice>
#include <QHttp>
#include <QMessageBox>
#include <QApplication>
#include <QUrl>
#include <QIcon>
#include <QDomElement>
#include <QPair>
#include <QProgressDialog>

// Xinx header
#include <core/xinxcore.h>
#include <project/xinxprojectmanager.h>
#include <project/xinxprojectproject.h>
#include "webservices.h"
#include "wsdl.h"
#include "servicesprojectpropertyimpl.h"

/* Parameter */

Parameter::Parameter(QString paramString, QString paramType)
{
	m_paramString = paramString;
	m_paramType = paramType;
}

Parameter::~Parameter()
{

}

const QString & Parameter::paramString() const
{
	return m_paramString;
}

const QString & Parameter::paramType() const
{
	return m_paramType;
}

/* Operations */

Operation::Operation(QString name)
{
	m_name = name;
}

Operation::~Operation()
{
	qDeleteAll(m_inputParam);
	qDeleteAll(m_outputParam);
}

QString Operation::name()
{
	return m_name;
}

const QList<Parameter*> & Operation::inputParam()
{
	return m_inputParam;
}

const QList<Parameter*> & Operation::outputParam()
{
	return m_outputParam;
}

QString Operation::encodingStyle()
{
	return m_encodingStyle;
}

QString Operation::namespaceString()
{
	return m_namespaceString;
}

/* WebServices */

WebServices::WebServices(const QString & wsdlLink, const QString & wsdlContent, QObject * parent) : QObject(parent), m_wsdlLink(wsdlLink), m_wsdlContent(wsdlContent)
{
	loadFromContent(wsdlContent);
}

WebServices::~WebServices()
{
	qDeleteAll(m_list);
}

QString WebServices::name()
{
	return m_wsdl.name();
}

const QList<Operation*> & WebServices::operations()
{
	return m_list;
}

const WSDL & WebServices::wsdl() const
{
	return m_wsdl;
}

void WebServices::loadFromElement(const QDomElement & element)
{
	qDeleteAll(m_list);
	m_list.clear();

	m_wsdl = WSDL(element);

	foreach(const WSDLService & service, m_wsdl.services())
	{
		QString tnsBinding = service.port().binding();
		tnsBinding = tnsBinding.mid(tnsBinding.indexOf(":") + 1);

		WSDLBinding binding = m_wsdl.bindings()[ tnsBinding ];
		QString tnsType = binding.type();
		tnsType = tnsType.mid(tnsType.indexOf(":") + 1);

		WSDLPortType portType = m_wsdl.portTypes()[ tnsType ];

		foreach(const WSDLOperation & operation, portType.operations())
		{
			Operation * wsOperation = new Operation(operation.name());

			foreach(const WSDLOperation & bindingOperation, binding.operations())
			{
				if (bindingOperation.name() == operation.name())
				{
					wsOperation->m_encodingStyle = bindingOperation.inputEncodingStyle();
					wsOperation->m_namespaceString = bindingOperation.inputNamespace();
				}
			}

			QString tnsInputMessage = operation.inputMessage();
			tnsInputMessage = tnsInputMessage.mid(tnsInputMessage.indexOf(":") + 1);

			WSDLMessage inputMessage = m_wsdl.messages()[ tnsInputMessage ];
			foreach(const WSDLPart & part, inputMessage.parts())
			{
				Parameter * param = new Parameter(part.name(), part.type());
				wsOperation->m_inputParam.append(param);
			}


			QString tnsOutputMessage = operation.outputMessage();
			tnsOutputMessage = tnsOutputMessage.mid(tnsOutputMessage.indexOf(":") + 1);

			WSDLMessage outputMessage = m_wsdl.messages()[ tnsOutputMessage ];
			foreach(const WSDLPart & part, outputMessage.parts())
			{
				Parameter * param = new Parameter(part.name(), part.type());
				wsOperation->m_outputParam.append(param);
			}

			m_list.append(wsOperation);
		}
	}

	emit updated();
}

void WebServices::loadFromContent(const QString & wsdlContent)
{
	QDomDocument document;
	QString errorStr;
	int errorLine;
	int errorColumn;
	if (!document.setContent(wsdlContent, true, &errorStr, &errorLine, &errorColumn))
	{
		QMessageBox::information(qApp->activeWindow(), QObject::tr("WSDL WebServices file"), QObject::tr("Parse error at line %1, column %2:\n%3")
		                         .arg(errorLine)
		                         .arg(errorColumn)
		                         .arg(errorStr));
		return;
	}

	loadFromElement(document.documentElement());
}

/* WebServicesManager */

WebServicesManager::WebServicesManager(XinxProject::Project* project) : QObject(project), m_isUpdate(false), _project(project)
{
	m_http = new QHttp(this);
	m_httpDialog = new QProgressDialog(qApp->activeWindow());
	m_httpDialog->setLabelText(tr("Load Web Services List ..."));
	connect(m_httpDialog, SIGNAL(canceled()), m_http, SLOT(abort()));
	connect(m_http, SIGNAL(done(bool)), this, SLOT(responseReady()));

	updateWebServicesList ();
}

WebServicesManager::~WebServicesManager()
{
	qDeleteAll(_list);
}

WebServicesManager* WebServicesManager::manager(XinxProject::Project* project)
{
	if (! project) return NULL;
	QObject * object = project->getObject("webservices");
	return qobject_cast<WebServicesManager*>(object);
}

const WebServicesList& WebServicesManager::list() const
{
	return _list;
}

void WebServicesManager::responseReady()
{
	Q_ASSERT_X(_project, "ServicesProjectPropertyImpl::saveSettingsDialog()", "Project must be defined");

	if (m_http->error() == QHttp::NoError)
	{
		QString content = m_http->readAll();
		if (! content.isEmpty())
		{
			_project->writeProperty("webServiceVersion" , WEBSERVICE_VERSION_CURRENT);
			_project->writeProperty(QString("webServiceLink_%1").arg(m_httpDialog->value()), m_httpString);
			_project->writeProperty(QString("webServiceContent_%1").arg(m_httpDialog->value()), content);
			_list.append(new WebServices(m_httpString, content, this));
		}
	}
	else
	{
		QMessageBox::critical(qApp->activeWindow(), tr("WSDL WebServices file"), tr("Can't load WSDL of link %1 : %2").arg(m_httpString).arg(m_http->errorString()));
	}

	m_httpDialog->setValue(m_httpDialog->value() + 1);
	m_hasFinished = true;
}

void WebServicesManager::updateWebServicesList()
{
	Q_ASSERT_X(_project, "ServicesProjectPropertyImpl::saveSettingsDialog()", "Project must be defined");

	bool enabled = _project && _project->activatedPlugin().contains("ServicesPlugin");
	if (enabled)
	{
		int version = _project->readProperty("webServiceVersion").toInt();
		QHash<QString,QString> wsdlContent;

		QStringList serveurWeb;
		if (version < WEBSERVICE_VERSION_1)
		{
			serveurWeb = _project->readProperty("webServiceLink").toString().split(";;", QString::SkipEmptyParts);
		}
		else
		{
			int index = 0;
			QString link;
			while (!(link = _project->readProperty(QString("webServiceLink_%1").arg(index)).toString()).isEmpty())
			{
				serveurWeb.append(link);
				wsdlContent[ link ] = _project->readProperty(QString("webServiceContent_%1").arg(index)).toString();

				index++;
			}
		}

		m_httpDialog->setMaximum(serveurWeb.count());
		m_httpDialog->setValue(0);
		m_httpDialog->show();

		m_isUpdate = true;
		foreach(const QString & link, serveurWeb)
		{
			if (wsdlContent[ link ].isEmpty())
			{
				m_hasFinished = false;
				m_httpString = link;
				QUrl queryUrl(link);
				m_http->setHost(queryUrl.host(), queryUrl.port());
				m_http->get(queryUrl.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority));
				while (! m_hasFinished)
					qApp->processEvents();
			}
			else
			{
				_list.append(new WebServices(link, wsdlContent[ link ], this));
				m_httpDialog->setValue(m_httpDialog->value() + 1);
			}
		}
		m_isUpdate = false;
		m_httpDialog->reset();
	}

	emit changed();
}


