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
#include "generixprojectpageform.h"
#include <project/xinxprojectproject.h>
#include <core/xinxconfig.h>
#include "configuration/configurationmanager.h"
#include "projectproperty/generixproject.h"

/* GenerixProjectPageFormImpl */

GenerixProjectPageFormImpl::GenerixProjectPageFormImpl(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), m_project(0)
{
	setupUi(this);

	qRegisterMetaType<ConfigurationVersion>("ConfigurationVersion");
	connect(m_webModuleLocation->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(updateInformations()));
}

GenerixProjectPageFormImpl::~GenerixProjectPageFormImpl()
{
}

void GenerixProjectPageFormImpl::setProject(XinxProject::Project * project)
{
	m_project = project;
}

QPixmap GenerixProjectPageFormImpl::image()
{
	return QPixmap();
}

QString GenerixProjectPageFormImpl::name()
{
	return this->windowTitle();
}

bool GenerixProjectPageFormImpl::loadSettingsDialog()
{
	m_webModuleLocation->lineEdit()->setText(static_cast<GenerixProject*>(m_project)->webModuleLocation());

	return true;
}

bool GenerixProjectPageFormImpl::saveSettingsDialog()
{
	static_cast<GenerixProject*>(m_project)->setWebModuleLocation(m_webModuleLocation->lineEdit()->text());

	return true;
}

bool GenerixProjectPageFormImpl::cancelSettingsDialog()
{
	return true;
}

QWidget * GenerixProjectPageFormImpl::settingsDialog()
{
	return this;
}

bool GenerixProjectPageFormImpl::isSettingsValid()
{
	if (! QDir(QDir::fromNativeSeparators(m_webModuleLocation->lineEdit()->text())).exists()) return false;
	return true;
}

bool GenerixProjectPageFormImpl::isVisible()
{
	return true;
}


void GenerixProjectPageFormImpl::updateInformations()
{
	GceInterface * interface = ConfigurationManager::self()->getInterfaceOfDirectory(m_webModuleLocation->lineEdit()->text());
	if (interface)
	{
		m_propertyFileLabel->setText(QDir(m_webModuleLocation->lineEdit()->text()).relativeFilePath(interface->rootFilename()));
		m_configurationVersionLabel->setVersion(interface->version());
		m_dictionaryCount->setText(QString("%1 dictionaries").arg(interface->dictionnaries().count()));
		m_businessViewLabel->setText(QString("%1 businessviews").arg(interface->businessViews().count()));
	}
	else
	{
		m_propertyFileLabel->setText(QString());
		m_configurationVersionLabel->setVersion(ConfigurationVersion());
		m_dictionaryCount->setText(QString());
		m_businessViewLabel->setText(QString());
	}
}
