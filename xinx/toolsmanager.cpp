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
#include "toolsmanager.h"
#include "xinxconfig.h"

// Qt header
#include <QFileDialog>
#include <QFile>

/* ToolsManager */

ToolsManager * ToolsManager::s_self = 0;

ToolsManager::ToolsManager() {
	
}

QString ToolsManager::toolNameOf( const QString & tool, QWidget * parentWindow ) {
	QString toolsPath = XINXConfig::self()->config().tools[ tool ];
	if( toolsPath.isEmpty() && QFile::exists( toolsPath ) ) {
		toolsPath = QFileDialog::getOpenFileName( parentWindow, tr("Select the %1 tool").arg( tool ), toolsPath );
		if( ! toolsPath.isEmpty() && QFile::exists( toolsPath ) )
			XINXConfig::self()->config().tools[ tool ] = toolsPath;
	}
	return toolsPath;
}

ToolsManager * ToolsManager::self() {
	if( ! s_self ) {
		s_self = new ToolsManager();
	}
	return s_self;
}


