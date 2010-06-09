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
#include "maqformatscheme.h"

// QCodeEdit header
#include <qformat.h>

MaquetteFormatScheme::MaquetteFormatScheme(XINXConfig * config) : XinxFormatScheme(config)
{
	setNameSpace("maquette");
	createDefaultScheme();
}

void MaquetteFormatScheme::createDefaultScheme()
{
	QFormat comment, string, escapeseq, reservedword, numbers, blocP, bloc05, bloc10, bloc11, bloc12;

	// Comment format
	comment.foreground = Qt::darkGreen;
	setFormat("comment", comment);

	// String format
	string.foreground = Qt::red;
	setFormat("string", string);

	// Escape Sequence format
	escapeseq.foreground = QColor(0xff, 0x14, 0x93);
	setFormat("escapeseq", escapeseq);

	// Reserved Word format
	reservedword.weight = QFont::Bold;
	setFormat("reservedword", reservedword);

	// Numbers format
	numbers.foreground = Qt::blue;
	setFormat("numbers", numbers);

	// BlocP format
	blocP.background = QColor(0xf5, 0xff, 0xbf);
	setFormat("blocP", blocP);

	// Bloc05 format
	bloc05.background = QColor(0xff, 0xd4, 0xd4);
	setFormat("bloc05", bloc05);

	// Bloc10 format
	bloc10.background = QColor(0xc0, 0xe0, 0xff);
	setFormat("bloc10", bloc10);

	// Bloc11 format
	bloc11.background = QColor(0xa6, 0xd9, 0xff);
	setFormat("bloc11", bloc11);

	// Bloc12 format
	bloc12.background = QColor(0x93, 0xd6, 0xff);
	setFormat("bloc12", bloc12);
}
