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
#include "xinxformatscheme.h"
#include "xinxconfig.h"

// QCodeEdit header
#include "qformat.h"

/* XinxFormatScheme */

XinxFormatScheme::XinxFormatScheme( XINXConfig * parent ) : QFormatScheme( parent ), m_config( parent ) {
	createStandardFormat();
}

XinxFormatScheme::~XinxFormatScheme() {

}

void XinxFormatScheme::createStandardFormat() {
	QFormat searchFormat, braceMatchFormat, braceMismatchFormat;

	// Init search format
	searchFormat.background = m_config->config().editor.highlightWord;
	setFormat( "search", searchFormat );

	// Init match format
	braceMatchFormat.weight = QFont::Bold;
	braceMatchFormat.foreground = Qt::red;
	braceMatchFormat.background = Qt::yellow;
	setFormat( "match", braceMatchFormat );
	setFormat( "braceMatch", braceMatchFormat );

	// Init mismatch format
	braceMismatchFormat.weight = QFont::Bold;
	braceMismatchFormat.foreground = Qt::yellow;
	braceMismatchFormat.background = Qt::red;
	setFormat( "braceMismatch", braceMismatchFormat );
}

void XinxFormatScheme::updateFormatsFromConfig() {
	// Reload from config file ;)
	createStandardFormat();

	foreach( QString format, m_config->config().formats.keys() ) {
		if( m_nameSpace.isEmpty() || format.startsWith( m_nameSpace + "_" ) ) {
			if( m_config->config().formats.contains( format ) ) {
				XINXConfig::struct_qformat conf = m_config->config().formats.value( format );

				QString formatName = format.remove( 0, m_nameSpace.length() + 1 );
				QFormat format;
				format.italic        = conf.italic;
				format.weight        = conf.bold ? QFont::Bold : QFont::Normal;
				format.overline      = conf.overline;
				format.strikeout     = conf.strikout;
				format.underline     = conf.underline;
				format.waveUnderline = conf.waveunderline;
				format.foreground    = conf.foreground;
				format.background    = conf.background;
				setFormat( formatName, format );
			}
		}
	}
}

void XinxFormatScheme::putFormatsToConfig() {
	foreach( QString f, formats() ) {
		if( ( f == "match" ) || ( f == "search" ) ) continue;
		XINXConfig::struct_qformat conf;
		conf.italic        = format( f ).italic;
		conf.bold          = format( f ).weight == QFont::Bold;
		conf.overline      = format( f ).overline;
		conf.strikout      = format( f ).strikeout;
		conf.underline     = format( f ).underline;
		conf.waveunderline = format( f ).waveUnderline;
		conf.foreground    = format( f ).foreground;
		conf.background    = format( f ).background;

		m_config->config().formats[ m_nameSpace + "_" + f ] = conf;
	}
}

void XinxFormatScheme::setNameSpace( const QString & value ) {
	m_nameSpace = value;
}

const QString & XinxFormatScheme::nameSpace() const {
	return m_nameSpace;
}
