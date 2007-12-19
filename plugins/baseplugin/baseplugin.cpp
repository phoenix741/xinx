/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

// Xinx header
#include "baseplugin.h"
#include "xmlhighlighter.h"
#include "jshighlighter.h"

// Qt header
#include <QStringList>
#include <QHash>

/* Constantes */

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

/* BasePlugin */

QStringList BasePlugin::highlighters() {
	baseplugin_js::init();
	return QStringList() << "XML" << "JS" << "CSS";
}

QHash<QString,QString> BasePlugin::highlighterFilters() {
	QHash<QString,QString> extentions;
	extentions[ "xsl" ]   = tr( "All XSL Stylesheet" );
	extentions[ "fws" ]   = tr( "All Webservices input stream" );
	extentions[ "xml" ]   = tr( "All XML File" );
	extentions[ "html" ]  = tr( "All HTML File" );
	extentions[ "htm" ]   = tr( "All HTML File" );
	extentions[ "js" ]    = tr( "All JavaScript" );
	extentions[ "css" ]   = tr( "All HTML Stylesheet" );
	return extentions;
}

QString BasePlugin::highlighterOfExtention( const QString & extention ) {
	QHash<QString,QString> extentions;
	extentions[ "xsl" ]   = "XML";
	extentions[ "fws" ]   = "XML";
	extentions[ "xml" ]   = "XML";
	extentions[ "html" ]  = "XML";
	extentions[ "htm" ]   = "XML";
	extentions[ "xhtml" ] = "XML";
	extentions[ "js" ]    = "JS";
	extentions[ "css" ]   = "CSS";
	return extentions[ extention ];
}

QHash<QString,QTextCharFormat> BasePlugin::formatOfHighlighter( const QString & highlighter ) {
	QHash<QString,QTextCharFormat> formats;
	if( highlighter.toUpper() == "XML" )  {
		formats[ "xml_comment"        ].setForeground( DEFAULT_COMMENT );
		formats[ "xml_error"          ].setForeground( DEFAULT_ERROR );
		formats[ "xml_other"          ].setForeground( DEFAULT_OTHER );
		formats[ "xml_syntaxchar"     ].setForeground( DEFAULT_SYNTAX_CHAR );
		formats[ "xml_elementname"    ].setForeground( DEFAULT_ELEMENT_NAME );
		formats[ "xml_attributename"  ].setForeground( DEFAULT_ATTRIBUTE_NAME );	
		formats[ "xml_attributevalue" ].setForeground( DEFAULT_ATTRIBUTE_VALUE );	
		formats[ "xml_xpathvalue"     ].setForeground( DEFAULT_XPATH_VALUE );	
	} else if( highlighter.toUpper() == "JS" ) {
		formats[ "js_comment"         ].setForeground( DEFAULT_COMMENT );
		formats[ "js_error"           ].setForeground( DEFAULT_ERROR );
		formats[ "js_other"           ].setForeground( DEFAULT_OTHER );
		formats[ "js_reservedword"    ].setForeground( DEFAULT_RESERVEDWORD );
		formats[ "js_reservedword"    ].setFontWeight( QFont::Bold );
		formats[ "js_number"          ].setForeground( DEFAULT_NUMBER );
		formats[ "js_string"          ].setForeground( DEFAULT_STRING );	
	} else if( highlighter.toUpper() == "CSS" ) {
		formats[ "css_comment"        ].setForeground( DEFAULT_COMMENT );
		formats[ "css_error"          ].setForeground( DEFAULT_ERROR );
		formats[ "css_other"          ].setForeground( DEFAULT_OTHER );
	}
	
	return formats;
}

QString BasePlugin::exampleOfHighlighter( const QString & highlighter ) {
	QString example;
	if( highlighter.toUpper() == "XML" ) {
		example =
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
				"<!-- This is a comment -->\n"
				"<xsl:stylesheet version=\"1.0\">\n"
				"\t<xsl:import href=\"myimport.xsl\"/>\n"
				"\t<xsl:variable name=\"MYVARIABLE\"/>\n"
				"\t<xsl:template match=\"/\">\n"
				"\t\t<xsl:comment> This is a text </xsl:comment>>>>>>>\n"
				"\n"
				"\t\t<xsl:choose>\n"
				"\t\t\t<xsl:when test=\"test\">\n"
				"\t\t\t\t<input type=\"hidden\" value=\"{$MYVARIABLE}\"/>\n"
				"\t\t\t</xsl:when>\n"
				"\t\t\t<xsl:otherwise>Otherwise not</xsl:otherwise>\n"
				"\t\t</xsl:choose>\n"
				"\t</xsl:template>\n"
				"</xsl:stylesheet>\n";		
	} else if( highlighter.toUpper() == "JS" ) {
		example = 
				"/**\n"
				" * This is a comment\n"
				"**/\n"
				"\n"
				"function myfunction( param1, param2, param3 ) {\n"
				"	var variable = window.open('number' + 56 + \"othertext\",'frame','options');\n"
				"	alert( param& );\n"
				"	variable.close();\n"
				"}\n";
	} else if( highlighter.toUpper() == "CSS" )  {
		example =
				".test, #td {\n"
				"\tbackground-color: red;\n"
				"}\n";
	} 
	return example;
}

void BasePlugin::highlightBlock( const QString & highlighter, const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * i, const QString& text ) {
	if( highlighter.toUpper() == "XML" ) {
		baseplugin_xml::highlightBlock( formats, i, text );
	} else if( highlighter.toUpper() == "JS" ) {
		baseplugin_js::highlightBlock( formats, i, text );
	}
}


Q_EXPORT_PLUGIN2(xinx_baseplugin, BasePlugin)
