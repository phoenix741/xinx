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
#include "xinxlanguagefactory.h"
#include "xinxformatscheme.h"
#include "xinxformatfactory.h"
#include "xinxconfig.h"

// QCodeEdit header
#include <qnfadefinition.h>

/* XinxLanguageFactory */

XinxLanguageFactory::XinxLanguageFactory( XinxFormatScheme *fmt, XINXConfig *p ) : QLanguageFactory( fmt, p ) {
/*	QLanguageFactory::LangData data;
	QNFADefinition::load( ":/qcodeedit/lan/xml.qnfa", &data, XINXConfig::self()->formatFactory()->scheme( "XML" ) );
	XINXConfig::self()->languageFactory()->addLanguage(data);
*/
//	addDefinitionPath( ":/qcodeedit/lan" );
}

XinxLanguageFactory::~XinxLanguageFactory() {
}
