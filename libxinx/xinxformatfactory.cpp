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
#include "xinxformatscheme.h"
#include "xinxpluginsloader.h"
#include "xinxconfig.h"

// QCodeEdit header
#include "qformat.h"

/* XinxFormatFactory */

XinxFormatFactory::XinxFormatFactory( XINXConfig * parent ) : QObject( parent ), m_config( parent ) {
	updateFormats();
}

XinxFormatFactory::~XinxFormatFactory() {

}

XinxFormatScheme * XinxFormatFactory::scheme( const QString & highlighter ) {
	if( ! m_formatScheme.contains( highlighter ) ) {
		XinxFormatScheme * scheme = XinxPluginsLoader::self()->scheme( highlighter, m_config );
		if( scheme ) {
			m_formatScheme[ highlighter ] = scheme;
		}
	}
	return m_formatScheme.value( highlighter, 0 );
}

void XinxFormatFactory::updateFormats() {
	foreach( XinxFormatScheme * scheme, m_formatScheme.values() ) {
		scheme->updateFormatsFromConfig();
	}
}

void XinxFormatFactory::putFormats() {
	foreach( XinxFormatScheme * scheme, m_formatScheme.values() ) {
		scheme->putFormatsToConfig();
	}
}


