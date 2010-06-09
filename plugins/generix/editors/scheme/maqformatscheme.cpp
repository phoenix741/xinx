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
	QFormat comment, string, reservedword, numbers,
			blocP, blocV, blocT, blocPercent;

	// Comment format
	comment.foreground = Qt::darkGreen;
	setFormat("comment", comment);

	// String format
	string.foreground = Qt::red;
	setFormat("string", string);

	// Reserved Word format
	reservedword.weight = QFont::Bold;
	setFormat("reservedword", reservedword);

	// Numbers format
	numbers.foreground = Qt::blue;
	setFormat("numbers", numbers);
	setFormat("variable", numbers);
	setFormat("variable_fictive", numbers);

	// BlocP format
	blocP.background = QColor(0xf5, 0xff, 0xbf);
	setFormat("blocP", blocP);

	// Reserved Word format
	reservedword.background = QColor(0xf5, 0xff, 0xbf);
	reservedword.weight = QFont::Bold;
	setFormat("maquette_word", reservedword);

	// BlocV format
	blocV.background = QColor(0xff, 0xb9, 0xae);
	setFormat("blocV", blocV);
	setFormat("blocM", blocV);

	// blocPercent format
	blocPercent.foreground = QColor(0x05, 0xb8, 0xff);
	setFormat("blocPercent", blocPercent);

	// blocT format
	blocT.background = QColor(0xfe, 0x8e, 0xff);
	setFormat("blocT", blocT);
}
