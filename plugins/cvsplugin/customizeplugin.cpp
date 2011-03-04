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
#include "customizeplugin.h"

CustomizePlugin::CustomizePlugin(PluginSettings * settings, QWidget * parent) : QWidget(parent), m_settings(settings)
{
	setupUi(this);
}

CustomizePlugin::~CustomizePlugin()
{

}

QPixmap CustomizePlugin::image()
{
	return QPixmap(":/images/hi64-app-cervisia.png");
}

QString CustomizePlugin::name()
{
	return tr("CVS");
}

QWidget * CustomizePlugin::settingsDialog()
{
	return this;
}

bool CustomizePlugin::loadSettingsDialog()
{
	// CVS: Progress message
	if (m_settings->config().progressMessages.isEmpty())
		m_cvsVerboseComboBox->setCurrentIndex(2);
	else if (m_settings->config().progressMessages == "-q")
		m_cvsVerboseComboBox->setCurrentIndex(1);
	else if (m_settings->config().progressMessages == "-Q")
		m_cvsVerboseComboBox->setCurrentIndex(0);

	// CVS: Compression
	m_cvsCompressionComboBox->setCurrentIndex(m_settings->config().compressionLevel);

	// CVS: Prune empty directories
	m_cvsPruneCheckBox->setChecked(m_settings->config().pruneEmptyDirectories);

	// CVS: Create any directories that exist in the repository
	m_cvsCreateDirCheckBox->setChecked(m_settings->config().createDirectories);

	return true;
}

bool CustomizePlugin::saveSettingsDialog()
{
	// CVS: Progress message
	switch (m_cvsVerboseComboBox->currentIndex())
	{
	case 2:
		m_settings->config().progressMessages = "";
		break;
	case 1:
		m_settings->config().progressMessages = "-q";
		break;
	case 0:
		m_settings->config().progressMessages = "-Q";
		break;
	}

	// CVS: Compression
	m_settings->config().compressionLevel = m_cvsCompressionComboBox->currentIndex();

	// CVS: Prune empty directories
	m_settings->config().pruneEmptyDirectories = m_cvsPruneCheckBox->isChecked();

	// CVS: Create any directories that exist in the repository
	m_settings->config().createDirectories = m_cvsCreateDirCheckBox->isChecked();

	m_settings->save();
	return true;
}

bool CustomizePlugin::cancelSettingsDialog()
{
	return true;
}

bool CustomizePlugin::isSettingsValid(QString& message)
{
	return true;
}

bool CustomizePlugin::isVisible()
{
	return true;
}
