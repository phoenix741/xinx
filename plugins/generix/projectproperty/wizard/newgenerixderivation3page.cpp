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
#include "newgenerixderivation3page.h"
#include "projectproperty/generixproject.h"
#include "config/selfgenerixsettings.h"

/* NewGenerixDerivation3Page */

NewGenerixDerivation3Page::NewGenerixDerivation3Page()
{
	setupUi(this);

	registerField("generix.prefix", m_prefixEdit);
	registerField("generix.createMissingDirectory", m_missingDirChk);
	registerField("generix.copyStdFileToDirectory", m_copyToChk);
}

QString NewGenerixDerivation3Page::pagePluginId() const
{
	return "GenerixDerivation3";
}

bool NewGenerixDerivation3Page::pageIsVisible() const
{
	return field("generix.derivation").toBool();
}

bool NewGenerixDerivation3Page::saveSettingsDialog(XinxProject * project)
{
	QStringList prefixes;
	static_cast<GenerixProject*>(project)->setCreateMissingDirectory(m_missingDirChk->isChecked());
	if (m_addPrefixChk->isChecked())
		prefixes << m_prefixEdit->text();
	static_cast<GenerixProject*>(project)->setPrefixes(prefixes);
	static_cast<GenerixProject*>(project)->setCopySourceFileInDerivationPath(m_copyToChk->isChecked());

	return true;
}

void NewGenerixDerivation3Page::initializePage()
{
	IXinxPluginNewProjectConfigurationPage::initializePage();

	ConfigurationVersion version = field("generix.version").value<ConfigurationVersion>();
	ConfigurationVersion version150(6, 1, 50);

	QString projet = SelfGenerixSettings::self()->config().defaultProjectPathName;

	m_addPrefixChk->setChecked(true);
	m_missingDirChk->setChecked(true);
	m_prefixEdit->setText(QString("P%1").arg(field("project.name").toString().left(2)).toUpper());
	if (version <= version150)
	{
		m_copyToChk->setChecked(true);
	}
	else
	{
		m_copyToChk->setChecked(false);
	}
}

bool NewGenerixDerivation3Page::isComplete() const
{
	if (! IXinxPluginNewProjectConfigurationPage::isComplete()) return false;
	return !(m_addPrefixChk->isChecked() && m_prefixEdit->text().isEmpty());
}
