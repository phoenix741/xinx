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
#include "generixderivationoptionspage.h"
#include "projectproperty/generixproject.h"
#include <core/xinxconfig.h>

/* GenerixDerivationOptionsPageImpl */

GenerixDerivationOptionsPageImpl::GenerixDerivationOptionsPageImpl(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f)
{
	setupUi(this);

}

GenerixDerivationOptionsPageImpl::~GenerixDerivationOptionsPageImpl()
{
}

void GenerixDerivationOptionsPageImpl::setProject(XinxProject::Project * project)
{
	m_project = project;
}

QPixmap GenerixDerivationOptionsPageImpl::image()
{
	return QPixmap();
}

QString GenerixDerivationOptionsPageImpl::name()
{
	return this->windowTitle();
}

bool GenerixDerivationOptionsPageImpl::loadSettingsDialog()
{
	GenerixProject * project = static_cast<GenerixProject*>(m_project);
	if (! project) return false;

	m_createMissingDirectoryChk->setChecked(project->createMissingDirectory());
	m_prefixDerivatedFileChk->setChecked(! project->defaultPrefix().isEmpty());
	m_prefixList->setValues(project->prefixes());
	m_prefixList->setDefaultValue(project->defaultPrefix());
	m_copySourceFileChk->setChecked(project->copySourceFileInDerivationPath());

	return true;
}

bool GenerixDerivationOptionsPageImpl::saveSettingsDialog()
{
	GenerixProject * project = static_cast<GenerixProject*>(m_project);
	if (! project) return false;

	project->setCreateMissingDirectory(m_createMissingDirectoryChk->isChecked());
	if (m_prefixDerivatedFileChk->isChecked())
	{
		project->setDefaultPrefix(m_prefixList->defaultValue());
		project->setPrefixes(m_prefixList->values());
	}
	else
	{
		project->setDefaultPrefix(QString());
		project->setPrefixes(QStringList());
	}
	project->setCopySourceFileInDerivationPath(m_copySourceFileChk->isChecked());

	return true;
}

bool GenerixDerivationOptionsPageImpl::cancelSettingsDialog()
{
	return true;
}

QWidget * GenerixDerivationOptionsPageImpl::settingsDialog()
{
	return this;
}

bool GenerixDerivationOptionsPageImpl::isSettingsValid()
{
	return !(m_prefixDerivatedFileChk->isChecked() && m_prefixList->defaultValue().isEmpty());
}

bool GenerixDerivationOptionsPageImpl::isVisible()
{
	return true;
}
