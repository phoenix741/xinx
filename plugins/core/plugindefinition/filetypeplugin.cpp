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
#include "filetypeplugin.h"
#include "editors/models/xsl/xslcontentviewparser.h"
#include "editors/models/xsl/xslcv2parser.h"
#include "editors/widgeteditor/xml/xmltexteditor.h"
#include "editors/widgeteditor/xsl/xsltexteditor.h"
#include "editors/models/js/jscontentviewparser.h"

/* TextFileType */

QString TextFileType::description() {
	return tr( "Text File" );
}

QString TextFileType::match() {
	return "*.txt *.log *.spl";
}

QString TextFileType::icon() {
	return ":/images/typeunknown.png";
}

AbstractEditor * TextFileType::createEditor( const QString & filename ) {
	TextFileEditor * editor = new TextFileEditor();

	if( ! filename.isEmpty() )
		editor->loadFromFile( filename );

	return editor;
}

QString TextFileType::highlighterId() const {
	return "TEXT";
}

XinxFormatScheme * TextFileType::createFormatScheme( XINXConfig * config ) const {
	return new XinxFormatScheme( config );
}

QString TextFileType::createLanguageDescription() const {
	QString result;
	QFile description( ":/qcodeedit/lan/none.qnfa" );
	if( description.open( QIODevice::ReadOnly ) ) {
		QTextStream textDescription( &description );
		result = textDescription.readAll();
	}
	return result;
}

QString TextFileType::fileExample() const {
	return "This is a small text.\nOn multiple line";
}

ContentViewParser * TextFileType::createParser() {
	return 0;
}

/* XMLFileType */

QString XMLFileType::description() {
	return tr( "XML File" );
}

QString XMLFileType::match() {
	return "*.xml";
}

QString XMLFileType::icon() {
	return ":/images/typexml.png";
}

AbstractEditor * XMLFileType::createEditor( const QString & filename ) {
	XmlFileEditor * editor = new XmlFileEditor();

	if( ! filename.isEmpty() )
		editor->loadFromFile( filename );

	return editor;
}

QString XMLFileType::highlighterId() const {
	return "XML";
}

XinxFormatScheme * XMLFileType::createFormatScheme( XINXConfig * config ) const {
	return new XmlFormatScheme( config );
}

QString XMLFileType::createLanguageDescription() const {
	QString result;
	QFile description( ":/qcodeedit/lan/xml.qnfa" );
	if( description.open( QIODevice::ReadOnly ) ) {
		QTextStream textDescription( &description );
		result = textDescription.readAll();
	}
	return result;
}

QString XMLFileType::fileExample() const {
	return
		"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
		"<!-- This is a comment -->\n"
		"<xsl:stylesheet version=\"1.0\">\n"
		"\t<xsl:import href=\"myimport.xsl\"/>\n"
		"\t<xsl:variable name=\"MYVARIABLE\"/>\n"
		"\t<xsl:template match=\"/\">\n"
		"\t\t<xsl:comment> This is a text </xsl:comment>\n"
		"\n"
		"\t\t<xsl:choose>\n"
		"\t\t\t<xsl:when test=\"test\">\n"
		"\t\t\t\t<input type=\"hidden\" value=\"{$MYVARIABLE}\"/>\n"
		"\t\t\t</xsl:when>\n"
		"\t\t\t<xsl:otherwise>Otherwise not</xsl:otherwise>\n"
		"\t\t</xsl:choose>\n"
		"\t</xsl:template>\n"
		"</xsl:stylesheet>\n";
}

ContentViewParser * XMLFileType::createParser() {
	return 0;
}

/* XSLStyleSheetFileType */

QString XSLStyleSheetFileType::description() {
	return tr( "XSL Stylesheet" );
}

QString XSLStyleSheetFileType::match() {
	return "*.xsl";
}

QString XSLStyleSheetFileType::icon() {
	return ":/images/typexsl.png";
}

AbstractEditor * XSLStyleSheetFileType::createEditor( const QString & filename ) {
  StyleSheetEditor * editor = new StyleSheetEditor();

	if( ! filename.isEmpty() )
		editor->loadFromFile( filename );

	return editor;
}

ContentViewParser * XSLStyleSheetFileType::createParser() {
	return new XslContentViewParser();
}

ContentView2::Parser * XSLStyleSheetFileType::createParser2() {
	return new XslContentView2Parser();
}

/* HTMLFileType */

QString HTMLFileType::description() {
	return tr( "HTML File" );
}

QString HTMLFileType::match() {
	return "*.htm *.html *.xhtml";
}

QString HTMLFileType::icon() {
	return ":/images/typehtml.png";
}

AbstractEditor * HTMLFileType::createEditor( const QString & filename ) {
	HtmlFileEditor * editor = new HtmlFileEditor();

	if( ! filename.isEmpty() )
		editor->loadFromFile( filename );

	return editor;
}

/* JSFileType */

QString JSFileType::description() {
	return tr( "JavaScript" );
}

QString JSFileType::match() {
	return "*.js";
}

QString JSFileType::icon() {
	return ":/images/typejs.png";
}

AbstractEditor * JSFileType::createEditor( const QString & filename ) {
	JSFileEditor * editor = new JSFileEditor();

	if( ! filename.isEmpty() )
		editor->loadFromFile( filename );

	return editor;
}

QString JSFileType::highlighterId() const {
	return "ECMAScript";
}

QString JSFileType::createLanguageDescription() const {
	QString result;
	QFile description( ":/qcodeedit/lan/js.qnfa" );
	if( description.open( QIODevice::ReadOnly ) ) {
		QTextStream textDescription( &description );
		result = textDescription.readAll();
	}
	return result;
}

XinxFormatScheme * JSFileType::createFormatScheme( XINXConfig * config ) const {
	return new JsFormatScheme( config );
}

QString JSFileType::fileExample() const {
	return
		"/**\n"
		" * This is a comment\n"
		"**/\n"
		"\n"
		"function myfunction( param1, param2, param3 ) {\n"
		"	var variable = window.open('number' + 56 + \"othertext\",'frame','options');\n"
		"	alert( param& );\n"
		"	variable.close();\n"
		"}\n";
}

ContentViewParser * JSFileType::createParser() {
	return new JsContentViewParser();
}

/* CSSFileType */


QString CSSFileType::description() {
	return tr( "Cascading Style Sheet" );
}

QString CSSFileType::match() {
	return "*.css";
}

QString CSSFileType::icon() {
	return ":/images/typecss.png";
}

AbstractEditor * CSSFileType::createEditor( const QString & filename ) {
	CSSFileEditor * editor = new CSSFileEditor();

	if( ! filename.isEmpty() )
		editor->loadFromFile( filename );

	return editor;
}

QString CSSFileType::highlighterId() const {
	return "CSS";
}

QString CSSFileType::createLanguageDescription() const {
	QString result;
	QFile description( ":/qcodeedit/lan/css.qnfa" );
	if( description.open( QIODevice::ReadOnly ) ) {
		QTextStream textDescription( &description );
		result = textDescription.readAll();
	}
	return result;
}

XinxFormatScheme * CSSFileType::createFormatScheme( XINXConfig * config ) const {
	return new CssFormatScheme( config );
}

QString CSSFileType::fileExample() const {
	return
		".test, #td, test2 {\n"
		"\tbackground-color: red; /* Commentaire */\n"
		"\tmargin: 8pt;\n"
		"}\n";
}

ContentViewParser * CSSFileType::createParser() {
	return 0;
}

/* XQFileType */

XQFileType::XQFileType() {
	/*
		QLanguageFactory::LangData data;
		QNFADefinition::load( temporaryXqFile.fileName(), &data, XINXConfig::self()->formatFactory() );
		XINXConfig::self()->languageFactory()->addLanguage(data);
	}*/
}

XQFileType::~XQFileType() {
};

QString XQFileType::description() {
	return tr( "XQuery" );
}

QString XQFileType::match() {
	return "*.xq";
}

QString XQFileType::icon() {
	return ":/images/typexq.png";
}

AbstractEditor * XQFileType::createEditor( const QString & filename ) {
	XQFileEditor * editor = new XQFileEditor();

	if( ! filename.isEmpty() )
		editor->loadFromFile( filename );

	return editor;
}

QString XQFileType::highlighterId() const {
	return "XQuery";
}

QString XQFileType::createLanguageDescription() const {
	QString result;
	result
		= "<!DOCTYPE QNFA>"
		  "<QNFA language=\"XQuery\" extensions=\"xq\" defaultLineMark=\"bookmark\">"
		  "	<word id=\"data/float\" format=\"numbers\">-?[0-9]*.[0-9]+</word>"
		  "	<word id=\"data/decimal\" format=\"numbers\">-?[0-9]+</word>"
		  "	<context id=\"data/string\" format=\"string\" >"
		  "		<start>&quot;</start>"
		  "		<stop>&quot;</stop>"
		  "		<stop exclusive=\"false\">\n</stop>"
		  "		<sequence id=\"escape\" format=\"escapeseq\" >\\\\[nrtvf\\\"'\n]</sequence>"
		  "	</context>"
		  "	<context id=\"data/chars\" format=\"string\" >"
		  "		<start>'</start>"
		  "		<stop>'</stop>"
		  "		<stop exclusive=\"false\">\n</stop>"
		  "		<sequence id=\"escape\" format=\"escapeseq\" >\\\\[nrtvf\\\"'\n]</sequence>"
		  "	</context>";

	foreach( const QString & key, XQueryKeyword::self()->keywords().values() ) {
		result += QString("<list id=\"keyword_/%1\" format=\"xquery_%1\">").arg( key );

		foreach( QString value, XQueryKeyword::self()->keywords().keys( key ) ) {
			result += QString("<word>%1</word>").arg( value.replace( "-", "\\-" ) );
		}

		result += "</list>";
	}

	result += "</QNFA>";
	return result;
}

XinxFormatScheme * XQFileType::createFormatScheme( XINXConfig * config ) const {
	return new XqFormatScheme( config );
}

QString XQFileType::fileExample() const {
	return
		"count(DynamicRow)\n"
		"max(Qtecde)";
}

ContentViewParser * XQFileType::createParser() {
	return 0;
}
