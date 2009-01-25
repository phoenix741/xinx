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
#include "cssformatscheme.h"

// QCodeEdit header
#include <qformat.h>

CssFormatScheme::CssFormatScheme( XINXConfig * config ) : XinxFormatScheme( config ) {
	createDefaultScheme();
}

void CssFormatScheme::createDefaultScheme() {
	QFormat comment, string, escapeseq, numbers, attribute, classf, pseudoclass, directive, id, tag, identifier1, identifier2, value1, value2;

	// Comment format
	comment.foreground = Qt::darkGreen;
	setFormat( "comment", comment );

	// String format
	string.foreground = Qt::red;
	setFormat( "string", string );

	// Escape Sequence format
	escapeseq.foreground = QColor( 0xff, 0x14, 0x93 );
	setFormat( "escapeseq", escapeseq );

	// Numbers format
	numbers.foreground = Qt::blue;
	setFormat( "numbers", numbers );

	// Attribute format
	attribute.foreground = Qt::blue;
	setFormat( "attribute", attribute );

	// Class format
	classf.foreground = Qt::darkMagenta;
	classf.weight = QFont::Bold;
	setFormat( "class", classf );

	// Pseudo Class format
	pseudoclass.foreground = Qt::blue;
	setFormat( "pseudoclass", pseudoclass );

	// Directive format
	directive.foreground = Qt::blue;
	setFormat( "directive", directive );

	// Id format
	id.foreground = Qt::blue;
	setFormat( "id", id );

	// Tag format
	setFormat( "tag", tag );

	// Reserved Word format
	identifier1.weight = QFont::Bold;
	setFormat( "identifier1", identifier1 );

	// Reserved Word format
	identifier2.weight = QFont::Bold;
	setFormat( "identifier2", identifier2 );

	// Reserved Word format
	value1.foreground = Qt::darkRed;
	value1.weight = QFont::Bold;
	setFormat( "value1", value1 );

	// Reserved Word format
	value2.foreground = Qt::darkRed;
	value2.weight = QFont::Bold;
	setFormat( "value2", value2 );
}
