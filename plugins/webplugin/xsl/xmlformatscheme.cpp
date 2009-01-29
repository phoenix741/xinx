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
#include "xmlformatscheme.h"

// QCodeEdit header
#include <qformat.h>

XmlFormatScheme::XmlFormatScheme( XINXConfig * config ) : XinxFormatScheme( config ) {
	setNameSpace( "xml" );
	createDefaultScheme();
}

void XmlFormatScheme::createDefaultScheme() {
	QFormat comment, cdata, preprocessor, doctype, entity, elementname, attributename, attributevalue, xpath;

	// Comment format
	comment.foreground = Qt::darkGreen;
	setFormat( "comment", comment );

	// CDATA format
	cdata.foreground = Qt::gray;
	setFormat( "cdata", cdata );

	// Preprocessor format
	preprocessor.foreground = Qt::gray;
	setFormat( "preprocessor", preprocessor );

	// doctype format
	doctype.foreground = Qt::gray;
	setFormat( "doctype", doctype );

	// entity format
	entity.foreground = Qt::gray;
	setFormat( "entity", entity );

	// Element Name format
	elementname.foreground = Qt::darkRed;
	setFormat( "elementname", elementname );

	// Attribute Name format
	attributename.foreground = Qt::red;
	setFormat( "attributename", attributename );

	// Attribute Value format
	attributevalue.foreground = Qt::blue;
	setFormat( "attributevalue", attributevalue );

	// X Path format
	xpath.foreground = Qt::darkMagenta;
	setFormat( "xpath", xpath );
}
