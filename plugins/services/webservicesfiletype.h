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

// Xinx header
#include "webserviceseditor.h"

/* WebServicesFileType */

class WebServicesFileType : public QObject, public IFileTypePlugin
{
	Q_OBJECT
public:
	virtual QString description()
	{
		return tr("Web Services Stream");
	};
	virtual QString match()
	{
		return "*.fws";
	};
	virtual QString icon()
	{
		return ":/services/images/typefws.png";
	};

	virtual AbstractEditor * createEditor(const QString & filename)
	{
		WebServicesEditor * editor = new WebServicesEditor();

		if (! filename.isEmpty())
			editor->loadFromFile(filename);

		return editor;
	}

	virtual QString parserType()
	{
		return QString();
	}
};

#endif //  _WEBSERVICESFILETYPE_H_
