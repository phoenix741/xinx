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
#include "newgenerixderivation1page.h"
#include "projectproperty/generixproject.h"
#include "configuration/configurationfile.h"

/* NewGenerixDerivation1Page */

NewGenerixDerivation1Page::NewGenerixDerivation1Page()
{
	setupUi(this);

	registerField("generix.derivation*", m_derivationRadio);
	registerField("generix.projectPath*", m_projectPathEdit);
	registerField("generix.directoryAsPrefix", m_projectDirectoryAsPrefix);
}

QString NewGenerixDerivation1Page::pagePluginId() const
{
	return "GenerixDerivation1";
}

void NewGenerixDerivation1Page::initializePage()
{
	ConfigurationVersion version = field("generix.version").value<ConfigurationVersion>();
	ConfigurationVersion version150(6, 1, 50);

	m_projectDirectoryAsPrefix->setEnabled(version < version150);
}

bool NewGenerixDerivation1Page::pageIsVisible() const
{
	return true;
}

bool NewGenerixDerivation1Page::isComplete() const
{
	if (m_derivationRadio->isChecked() && m_projectPathEdit->text().isEmpty()) return false;
	return true;
}

bool NewGenerixDerivation1Page::saveSettingsDialog(XinxProject::ProjectPtr project)
{
	Q_UNUSED(project);
	return true;
}
