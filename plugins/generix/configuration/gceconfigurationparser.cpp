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
#include "gceconfigurationparser.h"
#include "gceconfigurationxmlparser2.h"
#include "config/selfgcesettings.h"

/* GceConfigurationParser */

GceConfigurationParser::GceConfigurationParser(const QString & filename)
{
	_filename = filename;
}

GceConfigurationParser::~GceConfigurationParser()
{
}

void GceConfigurationParser::startJob()
{
	if (interface())
	{
		GceConfigurationXmlParser2 parser;
		parser.m_quick = ! SelfGceSettings::self()->config().readConfigurations;
		parser.m_configurationNumber = -1;
		parser._gce_configuration = interface();
		parser.loadFromFile(_filename);

		interface()->addBusinessView(parser.m_fileRefToInformation);
		interface()->setVersion(ConfigurationVersion(parser.m_version, parser.m_edition));
	}
}

