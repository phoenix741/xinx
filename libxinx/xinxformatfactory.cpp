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
#include "xinxformatfactory.h"
#include "xinxconfig.h"

// QCodeEdit header
#include "qformat.h"

/* XinxFormatFactory */

XinxFormatFactory::XinxFormatFactory( XINXConfig * parent ) : QFormatScheme( parent ), m_config( parent ) {
	updateFormats();
}

XinxFormatFactory::~XinxFormatFactory() {

}

void XinxFormatFactory::updateFormats() {
	QFormat searchFormat, matchFormat;
	searchFormat.background = m_config->config().editor.highlightWord;
	matchFormat.weight = QFont::Bold;

	setFormat( "search", searchFormat );
	setFormat( "match", matchFormat );

	foreach( QString key, m_config->config().formats.keys() ) {
		QFormat ceFormat;
		QTextCharFormat tcFormat = m_config->config().formats[ key ];

		ceFormat.weight = tcFormat.fontWeight();
		ceFormat.italic = tcFormat.fontItalic();
		ceFormat.overline = tcFormat.fontOverline();
		ceFormat.underline = tcFormat.fontUnderline();
		ceFormat.strikeout = tcFormat.fontStrikeOut();
		ceFormat.foreground = tcFormat.foreground().color();

		setFormat( key, ceFormat );
	}
}

