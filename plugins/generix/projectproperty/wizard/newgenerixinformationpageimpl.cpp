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
#include "newgenerixinformationpageimpl.h"
#include "configuration/gceinterfacefactory.h"
#include <project/xinxproject.h>
#include <project/externalfileresolver.h>
#include "projectproperty/generixproject.h"
#include "pluginresolver/gce150fileresolver.h"

// Qt header
#include <QLineEdit>

NewGenerixInformationPageImpl::NewGenerixInformationPageImpl()
{
	setupUi(this);
	labelDataStream->setBuddy(m_dataStreamEdit->lineEdit());

	qRegisterMetaType<ConfigurationVersion>("ConfigurationVersion");
	registerField("generix.webmodule*", m_webModuleLocation->lineEdit());
	registerField("generix.version", m_configurationVersionLabel, "version");
	registerField("generix.adresse", m_urlLocationEdit);
	registerField("generix.dataStream", m_dataStreamEdit->lineEdit());
	connect(m_webModuleLocation->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(updateInformations()));
}

NewGenerixInformationPageImpl::~NewGenerixInformationPageImpl()
{
}

QString NewGenerixInformationPageImpl::pagePluginId() const
{
	return "GenerixInformation";
}

bool NewGenerixInformationPageImpl::isComplete() const
{
	if (! QWizardPage::isComplete()) return false;
	if (! QDir(QDir::fromNativeSeparators(m_webModuleLocation->lineEdit()->text())).exists()) return false;
	return true;
}

bool NewGenerixInformationPageImpl::pageIsVisible() const
{
	return true;
}

bool NewGenerixInformationPageImpl::saveSettingsDialog(XinxProject * project)
{
	static_cast<GenerixProject*>(project)->setWebModuleLocation(m_webModuleLocation->lineEdit()->text());
	project->writeProperty("dataStreamLocation", m_dataStreamEdit->lineEdit()->text());
	project->writeProperty("moduleInternetAdresse", m_urlLocationEdit->text());

	return true;
}

void NewGenerixInformationPageImpl::initializePage()
{
	m_messageLabel->setVisible(false);
	m_webModuleLocation->lineEdit()->setText(field("project.path").toString());
}

void NewGenerixInformationPageImpl::updateInformations()
{
	const QString webModuleLocation     = m_webModuleLocation->lineEdit()->text();
	const QString dataStreamLocation    = QDir(QDir(webModuleLocation).absoluteFilePath("../../../log")).canonicalPath();
	QString moduleInternetAdresse = QDir(QDir(webModuleLocation).absoluteFilePath("presentation")).canonicalPath();
	if(!QDir(moduleInternetAdresse).exists())
	{
		moduleInternetAdresse = QDir(QDir(webModuleLocation).absoluteFilePath("langue")).canonicalPath();
	}
	setField("generix.adresse", moduleInternetAdresse);
	setField("generix.dataStream", dataStreamLocation);

	GceInterface * interface = GceInterfaceFactory::createGceInterface(webModuleLocation);
	if (interface)
	{
		setField("generix.version", QVariant::fromValue(interface->version()));
		ConfigurationVersion version = interface->version();
		ConfigurationVersion version150(6, 1, 50);

		const bool presentationDirectoryExist = QDir(QDir(webModuleLocation).absoluteFilePath("presentation")).exists();
		const bool langueFraNavDirectoryExist = QDir(QDir(webModuleLocation).absoluteFilePath("langue/fra/nav")).exists();

		m_messageLabel->setVisible(false);
		if (((version >= version150) && !presentationDirectoryExist) || ((version < version150) && !langueFraNavDirectoryExist))
		{
			m_messageLabel->setVisible(true);
			m_messageLabel->setText(tr("<p style=\"color: red\"><b>!!! WARNING !!!</b> : The expected folder structure doesn't match with the selected version of the web module. "
									   "Please check in the project property the configuration of dervication's path.</p>"));
		}
	}
	else
	{
		setField("generix.version", QVariant::fromValue(ConfigurationVersion()));
	}

	delete interface;
}
