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

/* PrivateXINXConfig */

class PrivateXINXConfig {
public:
	PrivateXINXConfig( XINXConfig * parent );
	~PrivateXINXConfig();
	
	QHash<QString,QString> m_extentions;
private:
	XINXConfig * m_parent;
};

PrivateXINXConfig::PrivateXINXConfig( XINXConfig * parent ) : m_parent( parent ) {
	m_extentions[ "xsl" ] = QApplication::translate( "XINXConfig", "All XSL Stylesheet" );
	m_extentions[ "js" ]  = QApplication::translate( "XINXConfig", "All JavaScript" );
	m_extentions[ "fws" ] = QApplication::translate( "XINXConfig", "All Webservices input stream" );
	m_extentions[ "xml" ] = QApplication::translate( "XINXConfig", "All XML File" );
	m_extentions[ "css" ] = QApplication::translate( "XINXConfig", "All HTML Stylesheet" );
	m_extentions[ "sql" ] = QApplication::translate( "XINXConfig", "All SQL file" );
}

PrivateXINXConfig::~PrivateXINXConfig() {
	
}


/* XINXConfig */

XINXConfig::XINXConfig( const XINXConfig & origine ) : AppSettings( origine ) {
	d = new PrivateXINXConfig( this );
}

XINXConfig::XINXConfig() : AppSettings() {
	d = new PrivateXINXConfig( this );
}

XINXConfig::~XINXConfig() {
	delete d;
}
	
struct_globals XINXConfig::getDefaultGlobals() {
	struct_globals value = AppSettings::getDefaultGlobals();

	value.formats[ "xml_comment"        ].setForeground( DEFAULT_COMMENT );
	value.formats[ "xml_error"          ].setForeground( DEFAULT_ERROR );
	value.formats[ "xml_other"          ].setForeground( DEFAULT_OTHER );
	value.formats[ "xml_syntaxchar"     ].setForeground( DEFAULT_SYNTAX_CHAR );
	value.formats[ "xml_elementname"    ].setForeground( DEFAULT_ELEMENT_NAME );
	value.formats[ "xml_attributename"  ].setForeground( DEFAULT_ATTRIBUTE_NAME );	
	value.formats[ "xml_attributevalue" ].setForeground( DEFAULT_ATTRIBUTE_VALUE );	
	value.formats[ "xml_xpathvalue"     ].setForeground( DEFAULT_XPATH_VALUE );	

	value.formats[ "js_comment"         ].setForeground( DEFAULT_COMMENT );
	value.formats[ "js_error"           ].setForeground( DEFAULT_ERROR );
	value.formats[ "js_other"           ].setForeground( DEFAULT_OTHER );
	value.formats[ "js_reservedword"    ].setForeground( DEFAULT_RESERVEDWORD );
	value.formats[ "js_reservedword"    ].setFontWeight( QFont::Bold );
	value.formats[ "js_number"          ].setForeground( DEFAULT_NUMBER );
	value.formats[ "js_string"          ].setForeground( DEFAULT_STRING );	
	
	return value;
}

struct_editor XINXConfig::getDefaultEditor() {
	struct_editor value = AppSettings::getDefaultEditor();

	QFont font;
	font.setFamily( "Monospace" );
	font.setFixedPitch( true );
	font.setPointSize( 8 );

	value.defaultFormat.setFont( font );

	return value;
}

QString XINXConfig::filter( const QString & suffix ) {
	return d->m_extentions[ suffix ] + " (*." + suffix + ")";
}

QStringList XINXConfig::filters() {
	QStringList result;
	QString allExtentions;
	
	foreach( QString suffix, d->m_extentions.keys() ) {
		result << filter( suffix );
		if( suffix != d->m_extentions.keys().last() )
			allExtentions += "*." + suffix + ";";
		else
			allExtentions += "*." + suffix;
	}
	result.insert( 0, QApplication::translate( "XINXConfig", "All managed file") + " (" + allExtentions + ")" );
	
	return result;
}

