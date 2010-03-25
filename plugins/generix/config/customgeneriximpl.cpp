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
#include "config/specifiquemodelindex.h"
#include <core/xinxconfig.h>

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
	SelfGenerixSettings::self()->load();

	// Extentions
	m_extentions = SelfGenerixSettings::self()->config().files;

	SpecifiqueModelIndex * specifiqueModel = new SpecifiqueModelIndex(&m_extentions , m_specifiqueTableView);
	m_specifiqueTableView->setModel(specifiqueModel);
	m_specifiqueTableView->resizeColumnsToContents();
	QObject::connect(specifiqueModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_specifiqueTableView, SLOT(resizeColumnsToContents()));

	return true;
}

bool CustomGenerixImpl::saveSettingsDialog()
{
	SelfGenerixSettings::self()->config().files = m_extentions;

	SelfGenerixSettings::self()->save();

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




