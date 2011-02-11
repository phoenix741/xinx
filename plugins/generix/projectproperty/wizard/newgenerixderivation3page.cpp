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
	ConfigurationVersion version = field("generix.version").value<ConfigurationVersion>();
	ConfigurationVersion version150(6, 1, 50);

	return field("generix.derivation").toBool() || (version >= version150);
}

bool NewGenerixDerivation3Page::saveSettingsDialog(XinxProject::ProjectPtr project)
{
	QSharedPointer<GenerixProject> gnxProject = project.staticCast<GenerixProject>();

	QStringList prefixes;
	gnxProject->setCreateMissingDirectory(m_missingDirChk->isChecked());
	if (m_addPrefixChk->isChecked())
		prefixes << m_prefixEdit->text();
	gnxProject->setPrefixes(prefixes);
	gnxProject->setCopySourceFileInDerivationPath(m_copyToChk->isChecked());

	return true;
}

void NewGenerixDerivation3Page::initializePage()
{
	m_createMissingMessage->setVisible(false);
	m_prefixMessage->setVisible(false);

	IXinxPluginNewProjectConfigurationPage::initializePage();

	ConfigurationVersion version = field("generix.version").value<ConfigurationVersion>();
	ConfigurationVersion version150(6, 1, 50);

	m_prefixEdit->setText(QString("%1").arg(field("project.name").toString().left(3)).toUpper());
	m_missingDirChk->setChecked(true);
	if (version < version150)
	{
		m_addPrefixChk->setChecked(true);
		m_copyToChk->setChecked(true);
	}
	else
	{
		m_addPrefixChk->setChecked(false);
		m_copyToChk->setChecked(false);
	}
}

bool NewGenerixDerivation3Page::isComplete() const
{
	if (! IXinxPluginNewProjectConfigurationPage::isComplete()) return false;
	return !(m_addPrefixChk->isChecked() && m_prefixEdit->text().isEmpty());
}

void NewGenerixDerivation3Page::on_m_missingDirChk_stateChanged(int value)
{
	m_createMissingMessage->setVisible(field("generix.derivation").toBool() && (value == Qt::Unchecked));
}

void NewGenerixDerivation3Page::on_m_addPrefixChk_stateChanged(int value)
{
	ConfigurationVersion version = field("generix.version").value<ConfigurationVersion>();
	ConfigurationVersion version140(6, 1, 40);

	m_prefixMessage->setVisible((version < version140) && (value == Qt::Unchecked));
}
