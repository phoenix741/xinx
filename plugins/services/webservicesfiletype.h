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

#ifndef _WEBSERVICESFILETYPE_H_
#define _WEBSERVICESFILETYPE_H_
#pragma once

// Qt header
#include <QObject>

// Xinx header
#include <plugins/interfaces/files.h>

/* WebServicesFileType */

class WebServicesFileType : public QObject, public IFileTypePlugin
{
	Q_OBJECT
public:
	WebServicesFileType();
	virtual ~WebServicesFileType();

	virtual QString name();
	virtual QString description();
	virtual QString match();
	virtual QString icon();
	virtual AbstractEditor * createEditor();
	virtual QString parserType();
	virtual QString defaultFileName();
};

#endif //  _WEBSERVICESFILETYPE_H_
