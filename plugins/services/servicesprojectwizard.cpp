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
#include "servicesprojectwizard.h"
#include "servicesprojectpropertyimpl.h"
#include <project/xinxproject.h>

/* ServicesListPageImpl */

ServicesListPageImpl::ServicesListPageImpl()
{
	setupUi(this);
	setTitle(windowTitle());
	setSubTitle(tr("Define the list of WSDL. WSDL is used to describe the web services. This"
	               "list contains link to WSDL."));

	registerField("services.list", m_webServicesWidget);

	m_webServicesWidget->setDefaultProposedValue("http://localhost:8888/gce/services/?WSDL");
	m_webServicesWidget->setDefaultVisible(false);
}

QVariant ServicesListPageImpl::field(const QString &name) const
{
	return QWizardPage::field(name);
}

QString ServicesListPageImpl::pagePluginId() const
{
	return "ServiceInformation";
}

bool ServicesListPageImpl::pageIsVisible() const
{
	return true;
}

bool ServicesListPageImpl::saveSettingsDialog(XinxProject * project)
{
	project->writeProperty("webServiceVersion", WEBSERVICE_VERSION_CURRENT);

	const QStringList services = m_webServicesWidget->values();

	int index = 0;
	foreach(const QString & link, services)
	{
		project->writeProperty(QString("webServiceLink_%1").arg(index), link);
		index++;
	}

	return true;
}
