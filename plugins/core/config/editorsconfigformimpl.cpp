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

#include "editorsconfigformimpl.h"
#include "config/selfwebpluginsettings.h"

/* EditorsConfigFormImpl */

EditorsConfigFormImpl::EditorsConfigFormImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);

}

EditorsConfigFormImpl::~EditorsConfigFormImpl()
{

}

QPixmap EditorsConfigFormImpl::image()
{
	return QPixmap(":/images/preferences-editors.png");
}

QString EditorsConfigFormImpl::name()
{
	return tr("Editors Completion");
}

QWidget * EditorsConfigFormImpl::settingsDialog()
{
	return this;
}

bool EditorsConfigFormImpl::loadSettingsDialog()
{
	SelfWebPluginSettings * settings = SelfWebPluginSettings::self();

	m_activeXmlGroupBox->setChecked(settings->config().xml.activeCompletion);
	m_addClosedBaliseCheckBox->setChecked(settings->config().xml.addClosedBalise);
	m_addDefaultAttributeCheckBox->setChecked(settings->config().xml.addDefaultAttribute);
	m_addDefaultSubBaliseCheckBox->setChecked(settings->config().xml.addDefaultSubBalise);

	m_javaScriptGroupBox->setChecked(settings->config().javascript.activeCompletion);
	return true;
}

bool EditorsConfigFormImpl::saveSettingsDialog()
{
	SelfWebPluginSettings * settings = SelfWebPluginSettings::self();

	settings->config().xml.activeCompletion = m_activeXmlGroupBox->isChecked();
	settings->config().xml.addClosedBalise = m_addClosedBaliseCheckBox->isChecked();
	settings->config().xml.addDefaultAttribute = m_addDefaultAttributeCheckBox->isChecked();
	settings->config().xml.addDefaultSubBalise = m_addDefaultSubBaliseCheckBox->isChecked();

	settings->config().javascript.activeCompletion = m_javaScriptGroupBox->isChecked();

	settings->save();
	return true;
}

bool EditorsConfigFormImpl::isSettingsValid(QString& message)
{
	return true;
}

bool EditorsConfigFormImpl::cancelSettingsDialog()
{
	return true;
}

bool EditorsConfigFormImpl::isVisible()
{
	return true;
}

