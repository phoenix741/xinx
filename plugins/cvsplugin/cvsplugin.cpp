/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "cvsplugin.h"

// Qt header
#include <QString>
#include <QVariant>

/* CVSPlugin */

CVSPlugin::CVSPlugin() {
}

bool CVSPlugin::initializePlugin( const QString & lang ) {

}

QVariant CVSPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("CVS Plugin");
	case PLG_DESCRIPTION:
		return tr("Enable using CVS versionned file system.");
	case PLG_ICON:
		return QIcon();
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

Q_EXPORT_PLUGIN2(cvsplugin, CVSPlugin)
