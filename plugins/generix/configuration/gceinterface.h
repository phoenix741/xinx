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

#ifndef GCEINTERFACE_H
#define GCEINTERFACE_H

// Xinx header
#include "configuration/configurationfile.h"
#include <core/exceptions.h>

class GceInterfaceException : public XinxException
{
public:
	GceInterfaceException(const QString & exception) : XinxException(exception) {}
};

class GceInterface
{
public:
	GceInterface();
	virtual ~GceInterface();

	virtual QString rootFilename() = 0;
	virtual QStringList filenames() = 0;
	virtual ConfigurationVersion version() = 0;

	virtual QStringList dictionnaries() = 0;

	virtual QList<BusinessViewInformation> businessView(const QString & filename) = 0;
	virtual QList<BusinessViewInformation> businessViews() = 0;
	virtual QString resolveFileName(const QString & filename) = 0;
};

#endif // GCEINTERFACE_H
