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
#include "xqformatscheme.h"

// QCodeEdit header
#include <qformat.h>

XqFormatScheme::XqFormatScheme( XINXConfig * config ) : XinxFormatScheme( config ) {
	createDefaultScheme();
}

void XqFormatScheme::createDefaultScheme() {
	QFormat string, escapeseq, numbers, faccessors, fbuildin, fnumerical, fstring, fregular, fboolean, fdatetime, fsequence, faggregate, fcontext;

	// String format
	string.foreground = Qt::red;
	setFormat( "string", string );

	// Escape Sequence format
	escapeseq.foreground = QColor( 0xff, 0x14, 0x93 );
	setFormat( "escapeseq", escapeseq );

	// Numbers format
	numbers.foreground = Qt::blue;
	setFormat( "numbers", numbers );

	// Accessors format
	faccessors.foreground = Qt::darkGreen;
	faccessors.weight = QFont::Bold;
	setFormat( "xquery_accessors", faccessors );

	// Buid-In format
	fbuildin.foreground = Qt::darkGreen;
	fbuildin.weight = QFont::Bold;
	setFormat( "xquery_buildin", fbuildin );

	// Numerical format
	fnumerical.foreground = Qt::darkGreen;
	fnumerical.weight = QFont::Bold;
	setFormat( "xquery_numerical", fnumerical );

	// String format
	fstring.foreground = Qt::red;
	fstring.weight = QFont::Bold;
	setFormat( "xquery_string", fstring );

	// Regular format
	fregular.foreground = Qt::darkGreen;
	fregular.weight = QFont::Bold;
	setFormat( "xquery_regular", fregular );

	// Boolean format
	fboolean.foreground = Qt::darkGreen;
	fboolean.weight = QFont::Bold;
	setFormat( "xquery_boolean", fboolean );

	// Date Time format
	fdatetime.weight = QFont::Bold;
	setFormat( "xquery_datetime", fdatetime );

	// Sequence format
	fsequence.foreground = Qt::red;
	fsequence.weight = QFont::Bold;
	setFormat( "xquery_sequence", fsequence );

	// Aggrate format
	faggregate.foreground = Qt::blue;
	faggregate.weight = QFont::Bold;
	setFormat( "xquery_aggregate", faggregate );

	// Context format
	fcontext.foreground = Qt::darkGreen;
	fcontext.weight = QFont::Bold;
	setFormat( "xquery_context", fcontext );
}
