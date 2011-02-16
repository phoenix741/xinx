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
#include "customgeneriximpl.h"
#include <core/xinxconfig.h>
#include "config/selfgcesettings.h"

/* CustomGenerixImpl */

CustomGenerixImpl::CustomGenerixImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);
}

CustomGenerixImpl::~CustomGenerixImpl()
{
}

QPixmap CustomGenerixImpl::image()
{
	return QPixmap(":/generix/images/logo_gce64.png");
}

QString CustomGenerixImpl::name()
{
	return windowTitle();
}

QWidget * CustomGenerixImpl::settingsDialog()
{
	return this;
}

bool CustomGenerixImpl::loadSettingsDialog()
{
	SelfGceSettings::self()->load();

	m_readConfigurationChk->setChecked(SelfGceSettings::self()->config().readConfigurations);
	m_readDictionariesChk->setChecked(SelfGceSettings::self()->config().readDictionaries);

	return true;
}

bool CustomGenerixImpl::saveSettingsDialog()
{
	SelfGceSettings::self()->config().readConfigurations = m_readConfigurationChk->isChecked();
	SelfGceSettings::self()->config().readDictionaries   = m_readDictionariesChk->isChecked();

	SelfGceSettings::self()->save();
	return true;
}

bool CustomGenerixImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomGenerixImpl::isSettingsValid()
{
	return true;
}

bool CustomGenerixImpl::isVisible()
{
	return true;
}




