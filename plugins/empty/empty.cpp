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
#include "empty.h"

// Qt header
#include <QString>
#include <QVariant>
#include <QApplication>
#include <QPixmap>

/* EmptyPlugin */

EmptyPlugin::EmptyPlugin()
{
	Q_INIT_RESOURCE(empty);
}

EmptyPlugin::~EmptyPlugin()
{
}

bool EmptyPlugin::initializePlugin(const QString & lang)
{
	return true;
}

QVariant EmptyPlugin::getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr)
{
	switch (attr)
	{
	case PLG_NAME:
		return tr("XINX Demo Empty Plugin");
	case PLG_DESCRIPTION:
		return tr("This small plugin do nothing and is just a demo");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_ICON:
		return QPixmap(":/empty/images/empty.png");
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "1.0";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

Q_EXPORT_PLUGIN2(emptyplugin, EmptyPlugin)
