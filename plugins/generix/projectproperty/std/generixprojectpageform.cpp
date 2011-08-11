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
}

GenerixProjectPageFormImpl::~GenerixProjectPageFormImpl()
{
}

void GenerixProjectPageFormImpl::setProject(XinxProject::ProjectPtr project)
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
	updateInformations();

	return true;
}

bool GenerixProjectPageFormImpl::saveSettingsDialog()
{
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

bool GenerixProjectPageFormImpl::isSettingsValid(QString& message)
{
	return true;
}

bool GenerixProjectPageFormImpl::isVisible()
{
	return true;
}


void GenerixProjectPageFormImpl::updateInformations()
{
	if (! ConfigurationManager::manager(m_project)) return;
	QSharedPointer<GceConfiguration> interface = ConfigurationManager::manager(m_project)->getInterface();
	if (interface)
	{
		m_propertyFileLabel->setText(QDir(m_project->projectPath()).relativeFilePath(interface->filename()));
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
