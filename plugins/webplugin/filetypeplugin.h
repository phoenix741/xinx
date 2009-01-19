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

#ifndef _FILETYPEPLUGIN_H_
#define _FILETYPEPLUGIN_H_

// Xinx header
#include "xsl/xsllistview.h"
#include "xsl/stylesheeteditor.h"
#include "xsl/xmlfileeditor.h"
#include "xsl/htmlfileeditor.h"

#include "js/jsfileeditor.h"
#include "js/javascriptparser.h"

#include "css/cssmodeldata.h"
#include "css/cssfileeditor.h"

#include "xq/xqfileeditor.h"
#include "xq/xquery_keyword.h"

#include "editorcompletion.h"
#include "textfileeditor.h"

#include <xinxpluginsloader.h>
#include <xinxconfig.h>
#include <xinxlanguagefactory.h>
#include <xinxformatfactory.h>

// QCodeedit header
#include "qnfadefinition.h"

// Qt header
#include <QTemporaryFile>
#include <QTextStream>

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

/* XMLFileType */

class XMLFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	XMLFileType() {
		QHash<QString,QTextCharFormat> formats = defaultsFormat();
		foreach( QString key, formats.keys() )
			XINXConfig::self()->config().formats[ key ] = formats[ key ];

		struct_properties property = properties();
		XINXConfig::self()->config().files[ description() ].canBeSpecifique = property.canBeSaveAsSpecifique;
		XINXConfig::self()->config().files[ description() ].customPath = property.specifiqueSubDirectory;
	}

	virtual QString description() {	return tr( "XML File" ); };
	virtual QString match() { return "*.xml"; };
	virtual QIcon icon() { return QIcon( ":/images/typexml.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		XmlFileEditor * editor = new XmlFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}

	virtual FileContentElement * createElement( FileContentElement *, int, const QString & ) {
		return NULL;
	}

	virtual QString highlighterId() const {
		return "XML";
	}

	virtual QHash<QString,QTextCharFormat> defaultsFormat() const {
		QHash<QString,QTextCharFormat> formats;
		formats[ "xml_comment"        ].setForeground( DEFAULT_COMMENT );
		formats[ "xml_error"          ].setForeground( DEFAULT_ERROR );
		formats[ "xml_other"          ].setForeground( DEFAULT_OTHER );
		formats[ "xml_syntaxchar"     ].setForeground( DEFAULT_SYNTAX_CHAR );
		formats[ "xml_elementname"    ].setForeground( DEFAULT_ELEMENT_NAME );
		formats[ "xml_attributename"  ].setForeground( DEFAULT_ATTRIBUTE_NAME );
		formats[ "xml_attributevalue" ].setForeground( DEFAULT_ATTRIBUTE_VALUE );
		formats[ "xml_xpath"          ].setForeground( DEFAULT_XPATH_VALUE );
		return formats;
	}

	virtual QString fileExample() const {
		return
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
	}
};

/* XSLStyleSheetFileType */

class XSLStyleSheetFileType : public XMLFileType {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "XSL Stylesheet" ); };
	virtual QString match() { return "*.xsl"; };
	virtual QIcon icon() { return QIcon( ":/images/typexsl.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = true;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		StyleSheetEditor * editor = new StyleSheetEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}

	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		if( parent )
			return new XSLFileContentParser( parent, filename, line );
		else
			return new XSLFileContentParser();
	}
};

/* HTMLFileType */

class HTMLFileType : public XMLFileType {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "HTML File" ); };
	virtual QString match() { return "*.htm *.html *.xhtml"; };
	virtual QIcon icon() { return QIcon( ":/images/typehtml.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = false;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		HtmlFileEditor * editor = new HtmlFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
};

/* JSFileType */

class JSFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	JSFileType() {
		QHash<QString,QTextCharFormat> formats = defaultsFormat();
		foreach( QString key, formats.keys() )
			XINXConfig::self()->config().formats[ key ] = formats[ key ];

		struct_properties property = properties();
		XINXConfig::self()->config().files[ description() ].canBeSpecifique = property.canBeSaveAsSpecifique;
		XINXConfig::self()->config().files[ description() ].customPath = property.specifiqueSubDirectory;
	}

	virtual QString description() {	return tr( "JavaScript" ); };
	virtual QString match() { return "*.js"; };
	virtual QIcon icon() { return QIcon( ":/images/typejs.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = "js/";
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		JSFileEditor * editor = new JSFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		if( parent )
			return new JavaScriptParser( parent, filename, line );
		else
			return new JavaScriptParser();
	}

	virtual QString highlighterId() const {
		return "ECMAScript";
	}

	virtual QHash<QString,QTextCharFormat> defaultsFormat() const {
		QHash<QString,QTextCharFormat> formats;
		formats[ "ecmascript_comment"         ].setForeground( DEFAULT_COMMENT );
		formats[ "ecmascript_error"           ].setForeground( DEFAULT_ERROR );
		formats[ "ecmascript_other"           ].setForeground( DEFAULT_OTHER );
		formats[ "ecmascript_reservedword"    ].setForeground( DEFAULT_RESERVEDWORD );
		formats[ "ecmascript_reservedword"    ].setFontWeight( QFont::Bold );
		formats[ "ecmascript_number"          ].setForeground( DEFAULT_NUMBER );
		formats[ "ecmascript_string"          ].setForeground( DEFAULT_STRING );
		return formats;
	}

	virtual QString fileExample() const {
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
};

/* CSSFileType */

class CSSFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	CSSFileType() {
		QHash<QString,QTextCharFormat> formats = defaultsFormat();
		foreach( QString key, formats.keys() )
			XINXConfig::self()->config().formats[ key ] = formats[ key ];

		struct_properties property = properties();
		XINXConfig::self()->config().files[ description() ].canBeSpecifique = property.canBeSaveAsSpecifique;
		XINXConfig::self()->config().files[ description() ].customPath = property.specifiqueSubDirectory;
	}

	virtual QString description() {	return tr( "Cascading Style Sheet" ); };
	virtual QString match() { return "*.css"; };
	virtual QIcon icon() { return QIcon( ":/images/typecss.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = "css/";
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		CSSFileEditor * editor = new CSSFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		return new CSSFileContentParser( parent, filename, line );
	}

	virtual QString highlighterId() const {
		return "CSS";
	}

	virtual QHash<QString,QTextCharFormat> defaultsFormat() const {
		QHash<QString,QTextCharFormat> formats;
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
		return formats;
	}

	virtual QString fileExample() const {
		return
			".test, #td, test2 {\n"
			"\tbackground-color: red; /* Commentaire */\n"
			"\tmargin: 8pt;\n"
			"}\n";
	}
};

/* XQFileType */

class XQFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	XQFileType() {
		QHash<QString,QTextCharFormat> formats = defaultsFormat();
		foreach( QString key, formats.keys() )
			XINXConfig::self()->config().formats[ key ] = formats[ key ];

		struct_properties property = properties();
		XINXConfig::self()->config().files[ description() ].canBeSpecifique = property.canBeSaveAsSpecifique;
		XINXConfig::self()->config().files[ description() ].customPath = property.specifiqueSubDirectory;

		QTemporaryFile temporaryXqFile;
		if( temporaryXqFile.open() ) {
			QTextStream textStream( &temporaryXqFile );
			textStream 	<< "<!DOCTYPE QNFA>"
						<< "<QNFA language=\"XQuery\" extensions=\"xq\" defaultLineMark=\"bookmark\">"
						<< "<word id=\"data/float\" format=\"xq_numbers\">-?[0-9]*.[0-9]+</word>"
						<< "<word id=\"data/decimal\" format=\"xq_numbers\">-?[0-9]+</word>"
						<< "<context id=\"data/string\" format=\"xq_string\" >"
						<< "<start>&quot;</start>"
						<< "<stop>&quot;</stop>"
						<< "<stop exclusive=\"false\">\n</stop>"
						<< "<sequence id=\"escape\" format=\"xq_escapeseq\" >\\[nrtvf\\\"'\n]</sequence>"
						<< "</context>"
						<< "<context id=\"data/chars\" format=\"ecmascript_string\" >"
						<< "<start>'</start>"
						<< "<stop>'</stop>"
						<< "<stop exclusive=\"false\">\n</stop>"
						<< "<sequence id=\"escape\" format=\"escapeseq\" >\\[nrtvf\\\"'\n]</sequence>"
						<< "</context>";

			foreach( QString key, XQueryKeyword::self()->keywords().values() ) {
				textStream << "<list id=\"keyword_/" << key << "\" format=\"xquery_" << key << "\">";

				foreach( QString value, XQueryKeyword::self()->keywords().keys( key ) ) {
					textStream << "<word>" << value << "</word>";
				}

				textStream << "</list>";
			}

			textStream << "</QNFA>";

			temporaryXqFile.reset();

			QLanguageFactory::LangData data;
			QNFADefinition::load( temporaryXqFile.fileName(), &data, XINXConfig::self()->formatFactory() );
			XINXConfig::self()->languageFactory()->addLanguage(data);
		}
	}
	virtual ~XQFileType() {
	};

	virtual QString description() {	return tr( "XQuery" ); };
	virtual QString match() { return "*.xq"; };
	virtual QIcon icon() { return QIcon( ":/images/typexq.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = false;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = false;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		XQFileEditor * editor = new XQFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		Q_UNUSED( parent );
		Q_UNUSED( line );
		Q_UNUSED( filename );
		return 0;
	}

	virtual QString highlighterId() const {
		return "XQuery";
	}

	virtual QHash<QString,QTextCharFormat> defaultsFormat() const {
		QHash<QString,QTextCharFormat> formats;
		formats[ "xquery_accessors"       ].setForeground( DEFAULT_COMMENT );
		formats[ "xquery_accessors"       ].setFontWeight( QFont::Bold );
		formats[ "xquery_other"           ].setForeground( DEFAULT_OTHER );
		formats[ "xquery_buildin"         ].setForeground( DEFAULT_OTHER );
		formats[ "xquery_buildin"         ].setFontWeight( QFont::Bold );
		formats[ "xquery_numerical"       ].setForeground( DEFAULT_NUMBER );
		formats[ "xquery_numerical"       ].setFontWeight( QFont::Bold );
		formats[ "xquery_string"          ].setForeground( DEFAULT_STRING );
		formats[ "xquery_string"          ].setFontWeight( QFont::Bold );
		formats[ "xquery_regular"         ].setForeground( DEFAULT_COMMENT );
		formats[ "xquery_regular"         ].setFontWeight( QFont::Bold );
		formats[ "xquery_boolean"         ].setForeground( DEFAULT_COMMENT );
		formats[ "xquery_boolean"         ].setFontWeight( QFont::Bold );
		formats[ "xquery_datetime"        ].setForeground( DEFAULT_OTHER );
		formats[ "xquery_datetime"        ].setFontWeight( QFont::Bold );
		formats[ "xquery_sequence"        ].setForeground( DEFAULT_STRING );
		formats[ "xquery_sequence"        ].setFontWeight( QFont::Bold );
		formats[ "xquery_aggregate"       ].setForeground( DEFAULT_NUMBER );
		formats[ "xquery_aggregate"       ].setFontWeight( QFont::Bold );
		formats[ "xquery_context"         ].setForeground( DEFAULT_COMMENT );
		formats[ "xquery_context"         ].setFontWeight( QFont::Bold );
		return formats;
	}

	virtual QString fileExample() const {
		return
			"count(DynamicRow)\n"
			"max(Qtecde)";
	}
};

#endif // _FILETYPEPLUGIN_H_
