/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "customprojectimpl.h"
#include <core/xinxconfig.h>
#include <directoryedit.h>
#include <QDir>

/* CustomProjectImpl */

CustomProjectImpl::CustomProjectImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);
}

CustomProjectImpl::~CustomProjectImpl()
{
}

QPixmap CustomProjectImpl::image()
{
	return QPixmap(":/images/preferences-project.png");
}

QString CustomProjectImpl::name()
{
	return windowTitle();
}

QWidget * CustomProjectImpl::settingsDialog()
{
	return this;
}

bool CustomProjectImpl::loadSettingsDialog()
{
	// Default project directory
	m_projectPathLineEdit->lineEdit()->setText(QDir::toNativeSeparators(XINXConfig::self()->config().project.defaultPath));

	// Refresh Directory Timeout
	m_refreshProjectTimoutSpinBox->setValue(XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout);

	// Open the last project at start
	m_openLastSessionCheckBox->setChecked(XINXConfig::self()->config().project.openTheLastSessionAtStart);

	// Save project with session
	_single_project_mode_checkbox->setChecked(XINXConfig::self()->config().project.singleProjectMode);

	// Close the version management log automaticaly.
	m_closeLogCheckBox->setChecked(XINXConfig::self()->config().project.closeVersionManagementLog);

	// CVS : Create Change Log automatically
	m_changeLogCheckBox->setChecked(XINXConfig::self()->config().rcs.createChangelog);

	// CVS : Auto add file
	m_autoAddFileChk->setChecked(XINXConfig::self()->config().rcs.autoAddFileToVersionningSystem);

	return true;
}

bool CustomProjectImpl::saveSettingsDialog()
{
	// Default project directory
	XINXConfig::self()->config().project.defaultPath = QDir::fromNativeSeparators(m_projectPathLineEdit->lineEdit()->text());

	// Refresh Directory Timeout
	XINXConfig::self()->config().project.automaticProjectDirectoryRefreshTimeout = m_refreshProjectTimoutSpinBox->value();

	// Open the last project at start
	XINXConfig::self()->config().project.openTheLastSessionAtStart = m_openLastSessionCheckBox->isChecked();

	// Save project with session
	XINXConfig::self()->config().project.singleProjectMode = _single_project_mode_checkbox->isChecked();

	// Close the version management log automaticaly.
	XINXConfig::self()->config().project.closeVersionManagementLog = m_closeLogCheckBox->isChecked();

	// CVS : Create Change Log automatically
	XINXConfig::self()->config().rcs.createChangelog = m_changeLogCheckBox->isChecked();

	// CVS : Auto add file
	XINXConfig::self()->config().rcs.autoAddFileToVersionningSystem = m_autoAddFileChk->isChecked();

	return true;
}

bool CustomProjectImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomProjectImpl::isSettingsValid(QString& message)
{
	QString directory = QDir::fromNativeSeparators(m_projectPathLineEdit->lineEdit()->text());
	if (! QDir(directory).exists())
	{
		message = tr("Directory %1 doesn't exist.").arg(QDir::toNativeSeparators(directory));
		return false;
	}
	return true;
}

bool CustomProjectImpl::isVisible()
{
	return true;
}
