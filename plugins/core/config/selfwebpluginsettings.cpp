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
#include <core/xinxcore.h>
#include <core/xinxconfig.h>
#include "selfwebpluginsettings.h"

// Qt header
#include <QDir>

SelfWebPluginSettings * SelfWebPluginSettings::s_self = 0;

SelfWebPluginSettings::SelfWebPluginSettings() : WebPluginSettings() {
	load();
}

SelfWebPluginSettings::~SelfWebPluginSettings() {
	if( s_self == this ) {
		s_self = 0;
	}
}

SelfWebPluginSettings * SelfWebPluginSettings::self() {
	if( ! s_self ) {
		s_self = new SelfWebPluginSettings();
		XINXStaticDeleter::self()->add( s_self );
	}
	return s_self;
}

void SelfWebPluginSettings::save() {
	WebPluginSettings::save();
	emit changed();
}

WebPluginSettings::struct_oracleParser SelfWebPluginSettings::getDefaultOracleParser() {
	WebPluginSettings::struct_oracleParser value = WebPluginSettings::getDefaultOracleParser();

#ifdef Q_WS_WIN32
	value.classPath = "WEB-INF/lib/xmlparserv2.jar;WEB-INF/lib/gce_xmlparserv2.jar;WEB-INF/lib/technical_divers.jar;WEB-INF/lib/technical_framework.jar;WEB-INF/lib/log4j-1.2.14.jar";
#else
	value.classPath = "WEB-INF/lib/xmlparserv2.jar:WEB-INF/lib/gce_xmlparserv2.jar:WEB-INF/lib/technical_divers.jar:WEB-INF/lib/technical_framework.jar:WEB-INF/lib/log4j-1.2.14.jar";
#endif

	return value;
}
