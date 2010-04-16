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
#include "selfgcesettings.h"

// Qt header
#include <QDir>

SelfGceSettings * SelfGceSettings::s_self = 0;

SelfGceSettings::SelfGceSettings() : GceSettings()
{
	load();
}

SelfGceSettings::~SelfGceSettings()
{
	if (s_self == this)
	{
		s_self = 0;
	}
}

SelfGceSettings * SelfGceSettings::self()
{
	if (! s_self)
	{
		s_self = new SelfGceSettings();
		XINXStaticDeleter::self()->add(s_self);
	}
	return s_self;
}

void SelfGceSettings::save()
{
	GceSettings::save();
	emit changed();
}

