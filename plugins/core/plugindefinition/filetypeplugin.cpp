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
#include "editors/widgeteditor/xml/xmltexteditor.h"
#include "editors/widgeteditor/xsl/xsltexteditor.h"

/* TextFileType */

QString TextFileType::name()
{
	return "TextFileEditor";
}

QString TextFileType::description()
{
	return tr("Text File");
}

QString TextFileType::match()
{
	return "*.txt *.log *.spl";
}

QString TextFileType::icon()
{
	return ":/images/typeunknown.png";
}

AbstractEditor * TextFileType::createEditor()
{
	return new TextFileEditor(0);
}

QString TextFileType::highlighterId() const
{
	return "TEXT";
}

XinxFormatScheme * TextFileType::createFormatScheme(XINXConfig * config) const
{
	return new XinxFormatScheme(config);
}

QString TextFileType::createLanguageDescription() const
{
	QString result;
	QFile description(":/qcodeedit/lan/none.qnfa");
	if (description.open(QIODevice::ReadOnly))
	{
		QTextStream textDescription(&description);
		result = textDescription.readAll();
	}
	return result;
}

QString TextFileType::fileExample() const
{
	return "This is a small text.\nOn multiple line";
}

QString TextFileType::parserType()
{
	return QString();
}

QString TextFileType::defaultFileName()
{
	return tr("noname") + ".txt";
}

/* XMLFileType */

QString XMLFileType::name()
{
	return "XmlFileEditor";
}

QString XMLFileType::description()
{
	return tr("XML File");
}

QString XMLFileType::match()
{
	return "*.xml";
}

QString XMLFileType::icon()
{
	return ":/images/typexml.png";
}

AbstractEditor * XMLFileType::createEditor()
{
	return new XmlFileEditor();
}

QString XMLFileType::highlighterId() const
{
	return "XML";
}

XinxFormatScheme * XMLFileType::createFormatScheme(XINXConfig * config) const
{
	return new XmlFormatScheme(config);
}

QString XMLFileType::createLanguageDescription() const
{
	QString result;
	QFile description(":/qcodeedit/lan/xml.qnfa");
	if (description.open(QIODevice::ReadOnly))
	{
		QTextStream textDescription(&description);
		result = textDescription.readAll();
	}
	return result;
}

QString XMLFileType::fileExample() const
{
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

QString XMLFileType::parserType()
{
	return 0;
}

QString XMLFileType::defaultFileName()
{
	return tr("noname") + ".xml";
}


/* XSLStyleSheetFileType */

QString XSLStyleSheetFileType::name()
{
	return "StyleSheetEditor";
}

QString XSLStyleSheetFileType::description()
{
	return tr("XSL Stylesheet");
}

QString XSLStyleSheetFileType::match()
{
	return "*.xsl";
}

QString XSLStyleSheetFileType::icon()
{
	return ":/images/typexsl.png";
}

AbstractEditor * XSLStyleSheetFileType::createEditor()
{
	return new StyleSheetEditor();
}

QString XSLStyleSheetFileType::parserType()
{
	return "XSL";
}

QString XSLStyleSheetFileType::defaultFileName()
{
	return tr("noname") + ".xsl";
}

/* HTMLFileType */

QString HTMLFileType::name()
{
	return "HtmlFileEditor";
}

QString HTMLFileType::description()
{
	return tr("HTML File");
}

QString HTMLFileType::match()
{
	return "*.htm *.html *.xhtml";
}

QString HTMLFileType::icon()
{
	return ":/images/typehtml.png";
}

AbstractEditor * HTMLFileType::createEditor()
{
	return new HtmlFileEditor();
}

QString HTMLFileType::defaultFileName()
{
	return tr("noname") + ".html";
}

/* JSFileType */

QString JSFileType::name()
{
	return "JSFileEditor";
}

QString JSFileType::description()
{
	return tr("JavaScript");
}

QString JSFileType::match()
{
	return "*.js";
}

QString JSFileType::icon()
{
	return ":/images/typejs.png";
}

AbstractEditor * JSFileType::createEditor()
{
	return new JSFileEditor();
}

QString JSFileType::highlighterId() const
{
	return "ECMAScript";
}

QString JSFileType::createLanguageDescription() const
{
	QString result;
	QFile description(":/qcodeedit/lan/js.qnfa");
	if (description.open(QIODevice::ReadOnly))
	{
		QTextStream textDescription(&description);
		result = textDescription.readAll();
	}
	return result;
}

XinxFormatScheme * JSFileType::createFormatScheme(XINXConfig * config) const
{
	return new JsFormatScheme(config);
}

QString JSFileType::fileExample() const
{
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

QString JSFileType::parserType()
{
	return "JS";
}

QString JSFileType::defaultFileName()
{
	return tr("noname") + ".js";
}

/* CSSFileType */


QString CSSFileType::name()
{
	return "CSSFileEditor";
}

QString CSSFileType::description()
{
	return tr("Cascading Style Sheet");
}

QString CSSFileType::match()
{
	return "*.css";
}

QString CSSFileType::icon()
{
	return ":/images/typecss.png";
}

AbstractEditor * CSSFileType::createEditor()
{
	return new CSSFileEditor();
}

QString CSSFileType::highlighterId() const
{
	return "CSS";
}

QString CSSFileType::createLanguageDescription() const
{
	QString result;
	QFile description(":/qcodeedit/lan/css.qnfa");
	if (description.open(QIODevice::ReadOnly))
	{
		QTextStream textDescription(&description);
		result = textDescription.readAll();
	}
	return result;
}

XinxFormatScheme * CSSFileType::createFormatScheme(XINXConfig * config) const
{
	return new CssFormatScheme(config);
}

QString CSSFileType::fileExample() const
{
	return
		".test, #td, test2 {\n"
		"\tbackground-color: red; /* Commentaire */\n"
		"\tmargin: 8pt;\n"
		"}\n";
}

QString CSSFileType::parserType()
{
	return "CSS";
}

QString CSSFileType::defaultFileName()
{
	return tr("noname") + ".css";
}

/* XQFileType */


QString XQFileType::name()
{
	return "XQFileEditor";
}

XQFileType::XQFileType()
{
	/*
	    QLanguageFactory::LangData data;
	    QNFADefinition::load( temporaryXqFile.fileName(), &data, XINXConfig::self()->formatFactory() );
	    XINXConfig::self()->languageFactory()->addLanguage(data);
	}*/
}

XQFileType::~XQFileType()
{
};

QString XQFileType::description()
{
	return tr("XQuery");
}

QString XQFileType::match()
{
	return "*.xq";
}

QString XQFileType::icon()
{
	return ":/images/typexq.png";
}

AbstractEditor * XQFileType::createEditor()
{
	return new XQFileEditor();
}

QString XQFileType::highlighterId() const
{
	return "XQuery";
}

QString XQFileType::createLanguageDescription() const
{
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

	foreach(const QString & key, XQueryKeyword::self()->keywords().values())
	{
		result += QString("<list id=\"keyword_/%1\" format=\"xquery_%1\">").arg(key);

		foreach(QString value, XQueryKeyword::self()->keywords().keys(key))
		{
			result += QString("<word>%1</word>").arg(value.replace("-", "\\-"));
		}

		result += "</list>";
	}

	result += "</QNFA>";
	return result;
}

XinxFormatScheme * XQFileType::createFormatScheme(XINXConfig * config) const
{
	return new XqFormatScheme(config);
}

QString XQFileType::fileExample() const
{
	return
		"count(DynamicRow)\n"
		"max(Qtecde)";
}

QString XQFileType::parserType()
{
	return QString();
}

QString XQFileType::defaultFileName()
{
	return tr("noname") + ".xq";
}

