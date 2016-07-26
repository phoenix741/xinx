/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2016 by Ulrich Van Den Hekke                              *
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

// Qt header

// Xinx header
#include "webserviceseditor.h"
#include "webservicesfiletype.h"

/* WebServicesFileType */

WebServicesFileType::WebServicesFileType() {
}

WebServicesFileType::~WebServicesFileType() {
}

QString WebServicesFileType::name() {
	return "WebServicesEditor";
}

QString WebServicesFileType::description() {
	return tr("Web Services Stream");
}

QString WebServicesFileType::match() {
	return "*.fws";
}

QString WebServicesFileType::icon() {
	return ":/services/images/typefws.png";
}

AbstractEditor * WebServicesFileType::createEditor() {
	return new WebServicesEditor();
}

QString WebServicesFileType::parserType() {
	return QString();
}

QString WebServicesFileType::defaultFileName() {
	return tr("noname") + ".fws";
}
