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
#include "jsformatscheme.h"

// QCodeEdit header
#include <qformat.h>

JsFormatScheme::JsFormatScheme( XINXConfig * config ) : XinxFormatScheme( config ) {
	createDefaultScheme();
}

void JsFormatScheme::createDefaultScheme() {
	QFormat comment, string, danger, escapeseq, reservedword, numbers;

	// Comment format
	comment.foreground = Qt::darkGreen;
	setFormat( "comment", comment );

	// String format
	string.foreground = Qt::red;
	setFormat( "string", string );

	// Danger format
	danger.weight = QFont::Bold;
	danger.waveUnderline = true;
	danger.foreground = QColor( 0xd0, 0x60, 0xd0 );
	danger.background = Qt::red;
	setFormat( "danger", danger );

	// Escape Sequence format
	escapeseq.foreground = QColor( 0xff, 0x14, 0x93 );
	setFormat( "escapeseq", escapeseq );

	// Reserved Word format
	reservedword.weight = QFont::Bold;
	setFormat( "reservedword", reservedword );

	// Numbers format
	numbers.foreground = Qt::blue;
	setFormat( "numbers", numbers );
}
