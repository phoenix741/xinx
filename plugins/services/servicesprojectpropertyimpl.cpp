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

// Xinx header
#include <project/xinxprojectproject.h>
#include "servicesprojectpropertyimpl.h"

// Qt header
#include <QUrl>
#include <QMessageBox>
#include <QDomDocument>

/* ServicesProjectPropertyImpl */

ServicesProjectPropertyImpl::ServicesProjectPropertyImpl(QWidget* parent, Qt::WFlags fl) : QWidget(parent, fl), Ui::ServicesProjectProperty(), m_project(0)
{
	setupUi(this);

	m_http = new QHttp(this);
	m_httpProgressDialog = new QProgressDialog(this);
	connect(m_http, SIGNAL(dataReadProgress(int,int)), this, SLOT(httpDataReadProgress(int,int)));
	connect(m_http, SIGNAL(done(bool)), this, SLOT(httpRequestFinished(bool)));
	connect(m_httpProgressDialog, SIGNAL(canceled()), m_http, SLOT(abort()));

	m_servicesList->setDefaultVisible(false);
	m_servicesList->setDefaultProposedValue("http://localhost");
	connect(m_servicesList, SIGNAL(itemChanged(QString)), this, SLOT(on_m_refreshWSDL_clicked()));
}

ServicesProjectPropertyImpl::~ServicesProjectPropertyImpl()
{
}

QPixmap ServicesProjectPropertyImpl::image()
{
	return QPixmap();
}

QString ServicesProjectPropertyImpl::name()
{
	return windowTitle();
}

void ServicesProjectPropertyImpl::setProject(XinxProject::ProjectPtr project)
{
	m_project = project;
}

bool ServicesProjectPropertyImpl::loadSettingsDialog()
{
	Q_ASSERT_X(m_project, "ServicesProjectPropertyImpl::saveSettingsDialog()", "Project must be defined");

	QStringList links;
	QString link;
	int index = 0;
	int version = m_project->readProperty("webServiceVersion").toInt();
	if (version == WEBSERVICE_VERSION_1)
	{
		while (!(link = m_project->readProperty(QString("webServiceLink_%1").arg(index)).toString()).isEmpty())
		{
			links.append(link);
			m_wsdlContent[ link ] = m_project->readProperty(QString("webServiceContent_%1").arg(index)).toString();

			index++;
		}
		m_servicesList->setValues(links);
	}
	else
	{
		m_servicesList->setValues(m_project->readProperty("webServiceLink").toString().split(";;", QString::SkipEmptyParts));
	}

	return true;
}

bool ServicesProjectPropertyImpl::saveSettingsDialog()
{
	Q_ASSERT_X(m_project, "ServicesProjectPropertyImpl::saveSettingsDialog()", "Project must be defined");

	int index = 0;
	m_project->writeProperty("webServiceVersion", WEBSERVICE_VERSION_CURRENT);
	foreach(const QString & link, m_servicesList->values())
	{
		m_project->writeProperty(QString("webServiceLink_%1").arg(index), link);
		m_project->writeProperty(QString("webServiceContent_%1").arg(index), m_wsdlContent[ link ]);
		index++;
	}

	return true;
}

bool ServicesProjectPropertyImpl::cancelSettingsDialog()
{
	return true;
}

QWidget * ServicesProjectPropertyImpl::settingsDialog()
{
	return this;
}

bool ServicesProjectPropertyImpl::isSettingsValid(QString & message)
{
	return true;
}

bool ServicesProjectPropertyImpl::isVisible()
{
	return true;
}

void ServicesProjectPropertyImpl::wsdlSelectionChanged()
{
	m_refreshWSDL->setEnabled(! m_servicesList->currentItem().isEmpty());
	if (! m_servicesList->currentItem().isEmpty())
	{
		const QString & wsdl    = m_servicesList->currentItem();
		const QString & content = m_wsdlContent[ wsdl ];
		m_wsdlEdit->setPlainText(content);
		m_wsdlEdit->setEnabled(true);

		QDomDocument document;
		if (! document.setContent(content, true))
		{
			m_adressLineEdit->setText(QString());
			m_adressLineEdit->setEnabled(false);
			return;
		}
		m_adressLineEdit->setEnabled(true);

		QDomElement definitions = document.documentElement();
		QDomElement service     = definitions.firstChildElement("service");
		QDomElement port        = service.firstChildElement("port");
		QDomElement address     = port.firstChildElement("address");

		m_adressLineEdit->setText(address.attribute("location"));
	}
	else
	{
		m_wsdlEdit->setPlainText(QString());
		m_wsdlEdit->setEnabled(false);
		m_adressLineEdit->setText(QString());
		m_adressLineEdit->setEnabled(false);
	}
}

void ServicesProjectPropertyImpl::on_m_refreshWSDL_clicked()
{
	if (! m_servicesList->currentItem().isEmpty())
	{
		m_url = m_servicesList->currentItem();
		QUrl link(m_url);
		m_http->setHost(link.host(), link.port());

		m_httpProgressDialog->setLabelText(tr("Download WSDL from %1 ...").arg(m_url));
		m_httpProgressDialog->show();

		m_http->get(link.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority));
	}
}

void ServicesProjectPropertyImpl::on_m_adressLineEdit_editingFinished()
{
	if (! m_servicesList->currentItem().isEmpty())
	{
		const QString & wsdl    = m_servicesList->currentItem();
		QString content = m_wsdlContent[ wsdl ];

		QDomDocument document;
		if (! document.setContent(content, true)) return;

		QDomElement definitions = document.documentElement();
		QDomElement service     = definitions.firstChildElement("service");
		QDomElement port        = service.firstChildElement("port");
		QDomElement address     = port.firstChildElement("address");
		address.setAttribute("location", m_adressLineEdit->text());

		content = document.toString();
		m_wsdlContent[ wsdl ] = content;
		m_wsdlEdit->setPlainText(content);
	}
}

void ServicesProjectPropertyImpl::on_m_wsdlEdit_textChanged()
{
	if (! m_servicesList->currentItem().isEmpty())
	{
		const QString & wsdl = m_servicesList->currentItem();
		m_wsdlContent[ wsdl ] = m_wsdlEdit->toPlainText();

		QDomDocument document;
		if (! document.setContent(m_wsdlEdit->toPlainText(), true)) return;

		QDomElement definitions = document.documentElement();
		QDomElement service     = definitions.firstChildElement("service");
		QDomElement port        = service.firstChildElement("port");
		QDomElement address     = port.firstChildElement("address");
		m_adressLineEdit->setText(address.attribute("location"));
	}
}

void ServicesProjectPropertyImpl::httpRequestFinished(bool error)
{
	if (! error)
	{
		QString m_wsdl = QString(m_http->readAll());
		m_wsdlContent[ m_url ] = m_wsdl;
		wsdlSelectionChanged();
	}
	else
	{
		QMessageBox::critical(this, tr("Download WSDL from %1 ...").arg(m_url), m_http->errorString());
	}
	m_httpProgressDialog->hide();
}

void ServicesProjectPropertyImpl::httpDataReadProgress(int done, int total)
{
	m_httpProgressDialog->setMinimum(0);
	m_httpProgressDialog->setMaximum(total);
	m_httpProgressDialog->setValue(done);
}

