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

	foreach(QString resolver, ExternalFileResolver::self()->externalFileResoverNames())
	{
		m_resolverList->addItem(resolver);
	}

	qRegisterMetaType<ConfigurationVersion>("ConfigurationVersion");
	registerField("generix.webmodule*", m_webModuleLocation->lineEdit());
	registerField("generix.version", m_configurationVersionLabel, "version");
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

	return true;
}

void NewGenerixInformationPageImpl::initializePage()
{
	m_webModuleLocation->lineEdit()->setText(field("project.path").toString());
}

void NewGenerixInformationPageImpl::updateInformations()
{
	GceInterface * interface = GceInterfaceFactory::createGceInterface(m_webModuleLocation->lineEdit()->text());
	if (interface)
	{
		setField("generix.version", QVariant::fromValue(interface->version()));
		ConfigurationVersion version = interface->version();
		ConfigurationVersion version150(6, 1, 50);

		if (version <= version150)
		{
			m_resolverList->setCurrentIndex(0);
		}
		else
		{
			int index = m_resolverList->findText(ExternalFileResolver::self()->externalFileResover("gce150resolver")->name());
			m_resolverList->setCurrentIndex(index);
		}
	}
	else
	{
		setField("generix.version", QVariant::fromValue(ConfigurationVersion()));
	}

	delete interface;
}
