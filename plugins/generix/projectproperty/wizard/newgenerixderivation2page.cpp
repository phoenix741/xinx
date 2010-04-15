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
#include "newgenerixderivation2page.h"
#include "projectproperty/generixproject.h"
#include "configuration/configurationfile.h"

// Qt header
#include <QDir>

/* NewGenerixDerivation2Page */

NewGenerixDerivation2Page::NewGenerixDerivation2Page()
{
	setupUi(this);
}

void NewGenerixDerivation2Page::initializePage()
{
	ConfigurationVersion version = field("generix.version").value<ConfigurationVersion>();
	ConfigurationVersion version150(6, 1, 50);
	const bool isDirectoryAsPrefix = field("generix.directoryAsPrefix").toBool();
	const bool isDerivation = field("generix.derivation").toBool();
	const QString projectPath = field("generix.projectPath").toString();

	m_directoryList->clear();
	if (isDerivation && (version < version150))
	{
		QListWidgetItem * navPathItem = new QListWidgetItem(m_directoryList);
		navPathItem->setText("langue/fra/nav");
		navPathItem->setCheckState(Qt::Checked);

		QListWidgetItem * projectPathItem = new QListWidgetItem(m_directoryList);
		if (isDirectoryAsPrefix)
		{
			projectPathItem->setText(projectPath + "/langue/fra/nav");
		}
		else
		{
			projectPathItem->setText("langue/fra/nav/" + projectPath);
		}
		projectPathItem->setCheckState(Qt::Unchecked);
	}
	if (version >= version150)
	{
		const QString webModuleLocation = field("generix.webmodule").toString();
		bool projectPathAdded = false;
		foreach(const QString & p, QDir(QDir(webModuleLocation).absoluteFilePath("presentation")).entryList(QDir::Dirs | QDir::NoDotAndDotDot))
		{
			QListWidgetItem * pitem = new QListWidgetItem(m_directoryList);
			pitem->setText("presentation/" + p);
			if (isDerivation)
			{
				if (projectPath == p)
				{
					projectPathAdded = true;
					pitem->setCheckState(Qt::Unchecked);
				}
				else
				{
					pitem->setCheckState(Qt::Checked);
				}
			}
			else
			{
				pitem->setCheckState(Qt::Unchecked);
			}
		}
		if (isDerivation && !projectPathAdded)
		{
			QListWidgetItem * pitem = new QListWidgetItem(m_directoryList);
			pitem->setText("presentation/" + projectPath);
			pitem->setCheckState(Qt::Unchecked);
		}
	}
}

QString NewGenerixDerivation2Page::pagePluginId() const
{
	return "GenerixDerivation2";
}

bool NewGenerixDerivation2Page::pageIsVisible() const
{
	ConfigurationVersion version = field("generix.version").value<ConfigurationVersion>();
	ConfigurationVersion version150(6, 1, 50);

	return field("generix.derivation").toBool() || (version >= version150);
}

bool NewGenerixDerivation2Page::saveSettingsDialog(XinxProject * project)
{
	const bool isDerivation = field("generix.derivation").toBool();

	QList<GenerixProject::DerivationPath> paths;
	for (int i = 0; i < m_directoryList->count(); i++)
	{
		GenerixProject::DerivationPath dp;
		dp.name = m_directoryList->item(i)->text().section(QDir::separator(), -1);
		dp.path = m_directoryList->item(i)->text();
		dp.derivation = m_directoryList->item(i)->checkState() == Qt::Checked;
		if (isDerivation)
		{
			dp.visible = m_directoryList->item(i)->checkState() == Qt::Unchecked;
		}
		else
		{
			dp.visible = true;
		}
		paths << dp;
	}
	static_cast<GenerixProject*>(project)->setDerivationsPath(paths);
	return true;
}
