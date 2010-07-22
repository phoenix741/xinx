/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include "customcontentviewimpl.h"
#include <contentview2/contentview2manager.h>

CustomContentViewImpl::CustomContentViewImpl(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
}

CustomContentViewImpl::~CustomContentViewImpl()
{

}

QPixmap CustomContentViewImpl::image()
{
	return QPixmap(":/images/preferences-contentview.png");
}

QString CustomContentViewImpl::name()
{
	return windowTitle();
}

QWidget * CustomContentViewImpl::settingsDialog()
{
	return this;
}

bool CustomContentViewImpl::loadSettingsDialog()
{

}

bool CustomContentViewImpl::saveSettingsDialog()
{

}

bool CustomContentViewImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomContentViewImpl::isSettingsValid()
{
	return true;
}

bool CustomContentViewImpl::isVisible()
{
	return true;
}

void CustomContentViewImpl::on_m_compactBaseBtn_clicked()
{
	QSqlDatabase db = ContentView2::Manager::self()->database();
	{
		db.exec("VACUUM");
	}
}

void CustomContentViewImpl::on_m_reindexBaseBtn_clicked()
{

}

void CustomContentViewImpl::on_m_cleanBaseBtn_clicked()
{

}
