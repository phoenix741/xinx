/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "webplugin.h"
#include "xmlhighlighter.h"
#include "jshighlighter.h"
#include "csshighlighter.h"

#include "xmlprettyprinter.h"

#include "xsllistview.h"
#include "javascriptparser.h"
#include "cssmodeldata.h"

#include "editorcompletion.h"
#include "xmlcompleter.h"
#include "jscompleter.h"
#include "csscompleter.h"

// Qt header
#include <QStringList>
#include <QHash>
#include <QDir>
#include <QApplication>
#include <QTextEdit>
#include <QTranslator>
#include <QApplication>

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

WebPlugin::WebPlugin() {
	Q_INIT_RESOURCE(webplugin);
}

bool WebPlugin::initializePlugin( const QString & lang ) {
	QTranslator * tranlator = new QTranslator( this );
	tranlator->load( QString(":/translations/webplugin_%1").arg( lang ) );
	qApp->installTranslator(tranlator);

	webplugin_js::init();
	webplugin_css::init();
	
	xmlCompletionContents = new CompletionXML();
	try {
		xmlCompletionContents->setPath( "datas:baseplugin_xml.xml" );
	} catch( NotCompletionFileException ) {
		qWarning( qPrintable( tr("Can't read baseplugin_xml file") ) );
	}
	return true;
}

QVariant WebPlugin::getPluginAttribute( const enum IXinxPlugin::PluginAttribute & attr ) {
	switch( attr ) {
	case PLG_NAME:
		return tr("Web Plugin");
	case PLG_DESCRIPTION:
		return tr("Editor helper for HTML, XSL, XML, JS, CSS files.");
	case PLG_ICON:
		return QPixmap( ":/images/planet.png" );
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "0.1";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QStringList WebPlugin::extentions() {
	return QStringList() << "xml" << "xsl" << "htm html xhtml" << "js" << "css" << "fws"; 
}

QHash<QString,QString> WebPlugin::extentionsDescription() {
	QHash<QString,QString> extentions;
	extentions[ "xsl" ]   = tr( "XSL Stylesheet" );
	extentions[ "fws" ]   = tr( "Webservices input stream" );
	extentions[ "xml" ]   = tr( "XML File" );
	extentions[ "html" ]  = tr( "HTML File" );
	extentions[ "htm" ]   = tr( "HTML File" );
	extentions[ "xhtml" ] = tr( "HTML File" );
	extentions[ "js" ]    = tr( "JavaScript" );
	extentions[ "css" ]   = tr( "Cascading Style Sheet" );
	return extentions;
}

QIcon WebPlugin::icon( const QString & extention ) {
	if( extention == "xml" ) {
		return QIcon( ":/images/typexml.png" );
	} else if( extention == "xsl" ) {
		return QIcon( ":/images/typexsl.png" );
	} else if( extention == "js" ) {
		return QIcon( ":/images/typejs.png" );
	} else if( ( extention == "html" ) || ( extention == "htm" ) || ( extention == "xhtml" ) ) {
		return QIcon( ":/images/typehtml.png" );
	} else if( extention == "css" ) {
		return QIcon( ":/images/typecss.png" );
	} else if( extention == "fws" ) {
		return QIcon( ":/images/typefws.png" );
	} else
		return QIcon( ":/images/typeunknown.png" );
}

QStringList WebPlugin::highlighters() {
	return QStringList() << "XML" << "JS" << "CSS";
}

QString WebPlugin::highlighterOfExtention( const QString & extention ) {
	QHash<QString,QString> extentions;
	extentions[ "fws" ]   = "XML";
	extentions[ "xsl" ]   = "XML";
	extentions[ "xml" ]   = "XML";
	extentions[ "html" ]  = "XML";
	extentions[ "htm" ]   = "XML";
	extentions[ "xhtml" ] = "XML";
	extentions[ "js" ]    = "JS";
	extentions[ "css" ]   = "CSS";
	return extentions[ extention ];
}

QHash<QString,QTextCharFormat> WebPlugin::formatOfHighlighter( const QString & highlighter ) {
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
		formats[ "css_string"         ].setForeground( DEFAULT_STRING );	
		formats[ "css_operator"       ].setForeground( DEFAULT_SYNTAX_CHAR );
		formats[ "css_directive"      ].setForeground( DEFAULT_NUMBER );
		formats[ "css_number"         ].setForeground( DEFAULT_NUMBER );
		formats[ "css_pseudoclass"	  ].setForeground( DEFAULT_NUMBER );
		formats[ "css_class"		  ].setForeground( DEFAULT_XPATH_VALUE );
		formats[ "css_class"          ].setFontWeight( QFont::Bold );
		formats[ "css_id"		      ].setForeground( DEFAULT_XPATH_VALUE );
		formats[ "css_tag"		      ].setForeground( DEFAULT_RESERVEDWORD );
		formats[ "css_attribute"      ].setForeground( DEFAULT_NUMBER );
		formats[ "css_identifier"     ].setForeground( DEFAULT_RESERVEDWORD );
		formats[ "css_identifier"     ].setFontWeight( QFont::Bold );
		formats[ "css_identifier1"    ].setForeground( DEFAULT_RESERVEDWORD );
		formats[ "css_identifier1"    ].setFontWeight( QFont::Bold );
		formats[ "css_identifier2"    ].setForeground( DEFAULT_RESERVEDWORD );
		formats[ "css_identifier2"    ].setFontWeight( QFont::Bold );
		formats[ "css_value"		  ].setForeground( DEFAULT_ELEMENT_NAME );
		formats[ "css_value"		  ].setFontItalic( true );
		formats[ "css_value1"		  ].setForeground( DEFAULT_ELEMENT_NAME );
		formats[ "css_value1"		  ].setFontItalic( true );
		formats[ "css_value2"		  ].setForeground( DEFAULT_ELEMENT_NAME );
		formats[ "css_value2"		  ].setFontItalic( true );
	}
	
	return formats;
}

QString WebPlugin::exampleOfHighlighter( const QString & highlighter ) {
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
				".test, #td, test2 {\n"
				"\tbackground-color: red; /* Commentaire */\n"
				"\tmargin: 8pt;\n"
				"}\n";
	} 
	return example;
}

SyntaxHighlighter * WebPlugin::createHighlighter( const QString & highlighter, QObject* parent, XINXConfig * config ) {
	if( highlighter.toUpper() == "XML" ) 
		return new webplugin_xml( parent, config );
	if( highlighter.toUpper() == "JS" ) 
		return new webplugin_js( parent, config );
	if( highlighter.toUpper() == "CSS" ) 
		return new webplugin_css( parent, config );
	return 0;
}

SyntaxHighlighter * WebPlugin::createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config ) {
	if( highlighter.toUpper() == "XML" ) 
		return new webplugin_xml( parent, config );
	if( highlighter.toUpper() == "JS" ) 
		return new webplugin_js( parent, config );
	if( highlighter.toUpper() == "CSS" ) 
		return new webplugin_css( parent, config );
	return 0;
}

SyntaxHighlighter * WebPlugin::createHighlighter( const QString & highlighter, QTextEdit* parent, XINXConfig * config ) {
	if( highlighter.toUpper() == "XML" ) 
		return new webplugin_xml( parent, config );
	if( highlighter.toUpper() == "JS" ) 
		return new webplugin_js( parent, config );
	if( highlighter.toUpper() == "CSS" ) 
		return new webplugin_css( parent, config );
	return 0;
}

QStringList WebPlugin::prettyPrinters() {
	return QStringList() << "XML";
}

QString WebPlugin::prettyPrinterOfExtention( const QString & extention ) {
	QHash<QString,QString> extentions;
	extentions[ "xsl" ]   = "XML";
	extentions[ "xml" ]   = "XML";
	extentions[ "xhtml" ] = "XML";
	return extentions[ extention ];
}

QString WebPlugin::prettyPrint( const QString & plugin, const QString & text, QString * errorStr, int * line, int * column  ) {
	if( plugin == "XML" ) {
		try {
			if( errorStr ) *errorStr = QString();
			XMLPrettyPrinter prettyPrinter;
			prettyPrinter.setText( text );
			prettyPrinter.process();
			return prettyPrinter.getResult();
		} catch( XMLPrettyPrinterException e ) {
			if( errorStr ) *errorStr = e.m_message;
			if( line )     *line     = e.m_line;
			if( column )   *column   = e.m_column;
			return text;
		}
	} else
		return text;
}

QStringList WebPlugin::extendedEditors() {
	return QStringList() << "HTML" << "XML" << "XSL" << "JS" << "CSS";
}

QString WebPlugin::extendedEditorOfExtention( const QString & extention ) {
	QHash<QString,QString> extentions;
	extentions[ "fws" ]   = "FWS";
	extentions[ "xml" ]   = "XML";
	extentions[ "xsl" ]   = "XSL";
	extentions[ "html" ]  = "HTML";
	extentions[ "htm" ]   = "HTML";
	extentions[ "xhtml" ] = "HTML";
	extentions[ "js" ]    = "JS";
	extentions[ "css" ]   = "CSS";
	return extentions[ extention ];
}

void WebPlugin::commentSelectedText( const QString & plugin, IXinxExtendedEditor * editor, bool uncomment ) {
	if( ( plugin == "XML" ) || ( plugin == "XSL" ) || ( plugin == "HTML" ) || ( plugin == "FWS" ) )
		XmlCompleter::commentSelectedText( editor, uncomment );
	else if( plugin == "JS" )
		JsCompleter::commentSelectedText( editor, uncomment );
	else if( plugin == "CSS" )
		CssCompleter::commentSelectedText( editor, uncomment );
}

FileContentElement * WebPlugin::createModelData( const QString & plugin, IXinxExtendedEditor * editor, FileContentElement * parent, const QString & filename, int line ) {
	if( plugin == "XSL" ) {
		if( ! parent )
			return new XSLFileContentParser( editor );
		else
			return new XSLFileContentParser( editor, parent, filename, line );
	} else if( plugin == "JS" ) {
		if( ! parent )
			return new JavaScriptParser();
		else
			return new JavaScriptParser( parent, filename, line );
	} else if( plugin == "CSS" ) {
		return new CSSFileContentParser( editor, parent, filename, line );
	} else
		return NULL;
}

void WebPlugin::createCompleter( const QString & plugin, IXinxExtendedEditor * editor ) {
	if( plugin == "XSL" ) {
		XmlCompleter * c = new XmlCompleter( editor );
		editor->setObject( c );
	} else if( plugin == "HTML" ) {
		XmlCompleter * c = new XmlCompleter( editor, plugin );
		editor->setObject( c );
	} else if( plugin == "FWS" ) {
		XmlCompleter * c = new XmlCompleter( editor, plugin );
		editor->setObject( c );
	} else if( plugin == "JS" ) {
		JsCompleter * c = new JsCompleter( editor );
		editor->setObject( c );
	} else 
		editor->setObject( NULL );
}

QCompleter * WebPlugin::completer( const QString & plugin, IXinxExtendedEditor * editor ) {
	if( ( plugin == "XSL" ) || ( plugin == "HTML" ) ) {
		XmlCompleter * c = dynamic_cast<XmlCompleter*>( editor->object() );
		return c->currentCompleter( editor->qTextEdit()->textCursor() );
	} else if( plugin == "JS" ) {
		JsCompleter * c = dynamic_cast<JsCompleter*>( editor->object() );
		return c->currentCompleter( editor->qTextEdit()->textCursor() );
	} else
		return NULL;
}

bool WebPlugin::keyPress( const QString & plugin, IXinxExtendedEditor * editor, QKeyEvent * event ) {
	if( ( plugin == "XSL" ) || ( plugin == "HTML" ) || ( plugin == "FWS" ) ) {
		XmlCompleter * c = dynamic_cast<XmlCompleter*>( editor->object() );
		return c->keyPressEvent( event );
	} else if( plugin == "JS" ){
		JsCompleter * c = dynamic_cast<JsCompleter*>( editor->object() );
		return c->keyPressEvent( event );
	}
	return false;
}

QPair<QString,int> WebPlugin::searchWord( const QString & plugin, IXinxExtendedEditor * editor, const QString & word ) {
	if( ( plugin == "XSL" ) || ( plugin == "HTML" ) ) {
		XmlCompleter * c = dynamic_cast<XmlCompleter*>( editor->object() );
		return c->searchWord( word );
	}
	return qMakePair( QString(), -1 );
}

Q_EXPORT_PLUGIN2(webplugin, WebPlugin)
