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

#ifndef GCECONFIGURATIONDEF_H
#define GCECONFIGURATIONDEF_H

// Xinx header
#include "gceparser.h"

// Qt header
#include <QString>
#include <QMultiHash>
#include <QCoreApplication>

class GceConfigurationXmlParser2;

class GceConfigurationDefParser : public GceParser
{
	Q_DECLARE_TR_FUNCTIONS(GceConfigurationDefParser)
public:
	GceConfigurationDefParser();
	GceConfigurationDefParser(const QString & filename);
	virtual ~GceConfigurationDefParser();

	virtual void startJob();
protected:
	virtual void readConfigurationDef(const QString & configurationdefFileName);
        virtual void readConfigurationFile(int configurationIndex, const QString & module, const QString & configurationFileName);

	friend class GceConfigurationXmlParser2;
protected:
	QString _directory_path;
};

#endif // GCECONFIGURATIONDEF_H
