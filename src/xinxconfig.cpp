/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include <QApplication>
#include <QDir>
#include <QLocale>

#include "xinxconfig.h"
#include "jshighlighter.h"
#include "xmlhighlighter.h"

/* Define */

static const QColor DEFAULT_COMMENT			= Qt::darkGreen;
static const QColor DEFAULT_ERROR			= Qt::darkMagenta;
static const QColor DEFAULT_OTHER			= Qt::black;

static const QColor DEFAULT_SYNTAX_CHAR		= Qt::blue;
static const QColor DEFAULT_ELEMENT_NAME	= Qt::darkRed;
static const QColor DEFAULT_ATTRIBUTE_NAME	= Qt::red;
static const QColor DEFAULT_ATTRIBUTE_VALUE	= Qt::blue;
static const QColor DEFAULT_XPATH_VALUE		= Qt::darkMagenta;

static const QColor DEFAULT_RESERVEDWORD	= Qt::black;
static const QColor DEFAULT_NUMBER			= Qt::blue;
static const QColor DEFAULT_STRING			= Qt::red;

/* XINXConfig */

XINXConfig::XINXConfig( const XINXConfig & origine ) : AppSettings( origine ) {
}

XINXConfig::XINXConfig() : AppSettings() {
}

XINXConfig::~XINXConfig() {
}
	
struct_globals XINXConfig::getDefaultGlobals() {
	struct_globals value = AppSettings::getDefaultGlobals();

	QFont font;
	font.setFamily( "Monospace" );
	font.setFixedPitch( true );
	font.setPointSize( 8 );

	value.defaultFormat.setFont( font );
	
	return value;
}

struct_editor XINXConfig::getDefaultEditor() {
	struct_editor value = AppSettings::getDefaultEditor();

	formats[ "xml_comment"        ].setForeground( DEFAULT_COMMENT );
	formats[ "xml_error"          ].setForeground( DEFAULT_ERROR );
	formats[ "xml_other"          ].setForeground( DEFAULT_OTHER );
	formats[ "xml_syntaxchar"     ].setForeground( DEFAULT_SYNTAX_CHAR );
	formats[ "xml_elementname"    ].setForeground( DEFAULT_ELEMENT_NAME );
	formats[ "xml_attributename"  ].setForeground( DEFAULT_ATTRIBUTE_NAME );	
	formats[ "xml_attributevalue" ].setForeground( DEFAULT_ATTRIBUTE_VALUE );	
	formats[ "xml_xpathvalue"     ].setForeground( DEFAULT_XPATH_VALUE );	

	formats[ "js_comment"         ].setForeground( DEFAULT_COMMENT );
	formats[ "js_error"           ].setForeground( DEFAULT_ERROR );
	formats[ "js_other"           ].setForeground( DEFAULT_OTHER );
	formats[ "js_reservedword"    ].setForeground( DEFAULT_RESERVEDWORD );
	formats[ "js_reservedword"    ].setFontWeight( QFont::Bold );
	formats[ "js_number"          ].setForeground( DEFAULT_NUMBER );
	formats[ "js_string"          ].setForeground( DEFAULT_STRING );	
	
	return value;
}
