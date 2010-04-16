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

#ifndef SELFGENERIXSETTINGS_H_
#define SELFGENERIXSETTINGS_H_
#pragma once

// Xinx header
#include "gcesettings.h"

class SelfGceSettings : public QObject, public GceSettings
{
	Q_OBJECT
public:
	virtual ~SelfGceSettings();

	static SelfGceSettings * self();

	virtual void save();
signals:
	void changed();
private:
	SelfGceSettings();

	static SelfGceSettings * s_self;
};

#endif /* SELFGENERIXSETTINGS_H_ */
