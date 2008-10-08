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

#include "config/webpluginformimpl.h"
#include "config/selfwebpluginsettings.h"

#include "filetypeplugin.h"

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

	qRegisterMetaType<StyleSheetEditor>( "StyleSheetEditor" );
	qRegisterMetaType<XmlFileEditor>( "XmlFileEditor" );
	qRegisterMetaType<HtmlFileEditor>( "HtmlFileEditor" );
	qRegisterMetaType<JSFileEditor>( "JSFileEditor" );
	qRegisterMetaType<CSSFileEditor>( "CSSFileEditor" );
	qRegisterMetaType<XQFileEditor>( "XQFileEditor" );

	m_fileTypes << new XSLStyleSheetFileType;
	m_fileTypes << new XMLFileType;
	m_fileTypes << new HTMLFileType;
	m_fileTypes << new JSFileType;
	m_fileTypes << new CSSFileType;
	m_fileTypes << new XQFileType;
}

WebPlugin::~WebPlugin() {
	qDeleteAll( m_fileTypes );
	delete SelfWebPluginSettings::self();
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

QList<IFileTypePlugin*> WebPlugin::fileTypes() {
	return m_fileTypes;
}

QStringList WebPlugin::highlighters() {
	return QStringList() << "XML" << "JS" << "CSS" << "XQ";
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
	extentions[ "xq" ]    = "XQ";
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
	} else if( highlighter.toUpper() == "XQ" ) {
		formats[ "xq_accessors"       ].setForeground( DEFAULT_COMMENT );
		formats[ "xq_accessors"       ].setFontWeight( QFont::Bold );
		formats[ "xq_other"           ].setForeground( DEFAULT_OTHER );
		formats[ "xq_buildin"         ].setForeground( DEFAULT_OTHER );
		formats[ "xq_buildin"         ].setFontWeight( QFont::Bold );
		formats[ "xq_numerical"       ].setForeground( DEFAULT_NUMBER );
		formats[ "xq_numerical"       ].setFontWeight( QFont::Bold );
		formats[ "xq_string"          ].setForeground( DEFAULT_STRING );
		formats[ "xq_string"          ].setFontWeight( QFont::Bold );
		formats[ "xq_regular"         ].setForeground( DEFAULT_COMMENT );
		formats[ "xq_regular"         ].setFontWeight( QFont::Bold );
		formats[ "xq_boolean"         ].setForeground( DEFAULT_COMMENT );
		formats[ "xq_boolean"         ].setFontWeight( QFont::Bold );
		formats[ "xq_datetime"        ].setForeground( DEFAULT_OTHER );
		formats[ "xq_datetime"        ].setFontWeight( QFont::Bold );
		formats[ "xq_sequence"        ].setForeground( DEFAULT_STRING );
		formats[ "xq_sequence"        ].setFontWeight( QFont::Bold );
		formats[ "xq_aggregate"       ].setForeground( DEFAULT_NUMBER );
		formats[ "xq_aggregate"       ].setFontWeight( QFont::Bold );
		formats[ "xq_context"         ].setForeground( DEFAULT_COMMENT );
		formats[ "xq_context"         ].setFontWeight( QFont::Bold );

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
	} else if( highlighter.toUpper() == "CSS" ) {
		example =
				".test, #td, test2 {\n"
				"\tbackground-color: red; /* Commentaire */\n"
				"\tmargin: 8pt;\n"
				"}\n";
	} else if( highlighter.toUpper() == "XQ" ) {
		example =
				"count(DynamicRow)\n"
				"max(Qtecde)";

	}
	return example;
}

SyntaxHighlighter * WebPlugin::createHighlighter( const QString & highlighter, QTextDocument* parent, XINXConfig * config ) {
	if( highlighter.toUpper() == "XML" )
		return new webplugin_xml( parent, config );
	if( highlighter.toUpper() == "JS" )
		return new webplugin_js( parent, config );
	if( highlighter.toUpper() == "CSS" )
		return new webplugin_css( parent, config );
	if( highlighter.toUpper() == "XQ" )
		return new XQHighlighter( parent, config );
	return 0;
}

QWidget * WebPlugin::createSettingsDialog() {
	return new WebPluginFormImpl();
}

bool WebPlugin::loadSettingsDialog( QWidget * widget ) {
	SelfWebPluginSettings * settings = SelfWebPluginSettings::self();
	WebPluginFormImpl * form = qobject_cast<WebPluginFormImpl*>( widget );
	Q_ASSERT( form );

	form->m_activeXmlGroupBox->setChecked( settings->config().xml.activeCompletion );
	form->m_addClosedBaliseCheckBox->setChecked( settings->config().xml.addClosedBalise );
	form->m_addDefaultAttributeCheckBox->setChecked( settings->config().xml.addDefaultAttribute );
	form->m_addDefaultSubBaliseCheckBox->setChecked( settings->config().xml.addDefaultSubBalise );

	form->m_javaScriptGroupBox->setChecked( settings->config().javascript.activeCompletion );
	return true;
}

bool WebPlugin::saveSettingsDialog( QWidget * widget ) {
	SelfWebPluginSettings * settings = SelfWebPluginSettings::self();
	WebPluginFormImpl * form = qobject_cast<WebPluginFormImpl*>( widget );
	Q_ASSERT( form );

	settings->config().xml.activeCompletion = form->m_activeXmlGroupBox->isChecked();
	settings->config().xml.addClosedBalise = form->m_addClosedBaliseCheckBox->isChecked();
	settings->config().xml.addDefaultAttribute = form->m_addDefaultAttributeCheckBox->isChecked();
	settings->config().xml.addDefaultSubBalise = form->m_addDefaultSubBaliseCheckBox->isChecked();

	settings->config().javascript.activeCompletion = form->m_javaScriptGroupBox->isChecked();

	settings->save();
	return true;
}

Q_EXPORT_PLUGIN2(webplugin, WebPlugin)
