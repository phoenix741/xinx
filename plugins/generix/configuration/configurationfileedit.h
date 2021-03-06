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

#ifndef CONFIGURATIONFILEEDIT_H
#define CONFIGURATIONFILEEDIT_H

// Qt header
#include <QLabel>

// Xinx header
#include "configurationfile.h"

class ConfigurationVersionLabel : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(ConfigurationVersion version READ version WRITE setVersion)
public:
	ConfigurationVersionLabel(QWidget * parent);

	const ConfigurationVersion & version() const;
	void setVersion(const ConfigurationVersion & value);

private:
	ConfigurationVersion m_version;
};

#endif // CONFIGURATIONFILEEDIT_H
