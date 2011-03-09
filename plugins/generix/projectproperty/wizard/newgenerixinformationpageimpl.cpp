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
#include "configuration/gceconfigurationparserfactory.h"
#include <project/xinxprojectproject.h>
#include <project/externalfileresolver.h>
#include "projectproperty/generixproject.h"
#include "pluginresolver/gce150fileresolver.h"
#include <directoryedit.h>

// Qt header
#include <QLineEdit>

NewGenerixInformationPageImpl::NewGenerixInformationPageImpl()
{
	setupUi(this);
	labelDataStream->setBuddy(m_dataStreamEdit->lineEdit());

	qRegisterMetaType<ConfigurationVersion>("ConfigurationVersion");
	registerField("generix.version", m_configurationVersionLabel, "version");
	registerField("generix.adresse", m_urlLocationEdit->lineEdit());
	registerField("generix.dataStream", m_dataStreamEdit->lineEdit());
}

NewGenerixInformationPageImpl::~NewGenerixInformationPageImpl()
{
}

QString NewGenerixInformationPageImpl::pagePluginId() const
{
	return "GenerixInformation";
}

bool NewGenerixInformationPageImpl::pageIsVisible() const
{
	return true;
}

bool NewGenerixInformationPageImpl::saveSettingsDialog(XinxProject::ProjectPtr project)
{
	project->writeProperty("dataStreamLocation", m_dataStreamEdit->lineEdit()->text());
	project->writeProperty("moduleInternetAdresse", m_urlLocationEdit->lineEdit()->text());

	return true;
}

void NewGenerixInformationPageImpl::initializePage()
{
	m_messageLabel->setVisible(false);
	updateInformations(field("project.path").toString());
}

void NewGenerixInformationPageImpl::updateInformations(const QString & path)
{
	const QString webModuleLocation     = path;
	const QString dataStreamLocation    = QDir(QDir(webModuleLocation).absoluteFilePath("../../../log")).canonicalPath();
	QString moduleInternetAdresse = QDir(QDir(webModuleLocation).absoluteFilePath("presentation/common")).canonicalPath();
	if (moduleInternetAdresse.isEmpty() || !QDir(moduleInternetAdresse).exists())
	{
		moduleInternetAdresse = QDir(QDir(webModuleLocation).absoluteFilePath("langue/fra/")).canonicalPath();
	}
	setField("generix.adresse", moduleInternetAdresse);
	setField("generix.dataStream", dataStreamLocation);

	QSharedPointer<GceConfiguration> configurationFile(new GceConfiguration);
	QScopedPointer<GceParser> parser(GceConfigurationParserFactory::createGceParser(webModuleLocation));
	if (parser)
	{
		parser->setInterface(configurationFile);
		parser->startJob();

		setField("generix.version", QVariant::fromValue(configurationFile->version()));
		ConfigurationVersion version = configurationFile->version();
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
}
