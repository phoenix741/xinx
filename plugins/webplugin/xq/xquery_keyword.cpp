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
#include "xquery_keyword.h"
#include "xinxcore.h"

/* Static */
XQueryKeyword * XQueryKeyword::s_self = 0;


/* XQueryKeyword */

XQueryKeyword::XQueryKeyword() : QObject() {
	/* Accessors */
	m_keywords.insert("node-name",    "accessors");
	m_keywords.insert("nilled",       "accessors");
	m_keywords.insert("string",       "accessors");
	m_keywords.insert("data",         "accessors");
	m_keywords.insert("base-uri",     "accessors");
	m_keywords.insert("document-uri", "accessors");

	/* Other */
	m_keywords.insert("error",       "other");
	m_keywords.insert("trace",       "other");
	m_keywords.insert("resolve-uri", "other");

	/* Build-in Type */
	m_keywords.insert("xs:unsignedInt",  "buildin");
	m_keywords.insert("xs:string",       "buildin");
	m_keywords.insert("xs:boolean",      "buildin");
	m_keywords.insert("xs:decimal",      "buildin");
	m_keywords.insert("xs:float", 	     "buildin");
	m_keywords.insert("xs:double",       "buildin");
	m_keywords.insert("xs:duration",     "buildin");
	m_keywords.insert("xs:dateTime",     "buildin");
	m_keywords.insert("xs:time",         "buildin");
	m_keywords.insert("xs:date",         "buildin");
	m_keywords.insert("xs:gYearMonth",   "buildin");
	m_keywords.insert("xs:gYear",        "buildin");
	m_keywords.insert("xs:gMonthDay",    "buildin");
	m_keywords.insert("xs:gDay",         "buildin");
	m_keywords.insert("xs:gMonth",       "buildin");
	m_keywords.insert("xs:hexBinary",    "buildin");
	m_keywords.insert("xs:base64Binary", "buildin");
	m_keywords.insert("xs:anyURI",       "buildin");
	m_keywords.insert("xs:QName",        "buildin");

	/* Numerical */
	m_keywords.insert("number",             "numerical");
	m_keywords.insert("abs",                "numerical");
	m_keywords.insert("ceiling",            "numerical");
	m_keywords.insert("floor",              "numerical");
	m_keywords.insert("round",              "numerical");
	m_keywords.insert("round-half-to-even", "numerical");

	/* String */
	m_keywords.insert("codepoints-to-string", "string");
	m_keywords.insert("string-to-codepoints", "string");
	m_keywords.insert("compare"             , "string");
	m_keywords.insert("codepoint-equal"     , "string");
	m_keywords.insert("compare"             , "string");
	m_keywords.insert("concat"              , "string");
	m_keywords.insert("string-join"         , "string");
	m_keywords.insert("substring"           , "string");
	m_keywords.insert("string-length"       , "string");
	m_keywords.insert("normalize-space"     , "string");
	m_keywords.insert("normalize-unicode"   , "string");
	m_keywords.insert("upper-case"          , "string");
	m_keywords.insert("lower-case"          , "string");
	m_keywords.insert("translate"           , "string");
	m_keywords.insert("encode-for-uri"      , "string");
	m_keywords.insert("iri-to-uri"          , "string");
	m_keywords.insert("escape-html-uri"     , "string");

	/* Sub-string matching */
	m_keywords.insert("contains",         "string");
	m_keywords.insert("starts-with",      "string");
	m_keywords.insert("ends-with",        "string");
	m_keywords.insert("substring-before", "string");
	m_keywords.insert("substring-after",  "string");

	/* Regular */
	m_keywords.insert("matches",  "regular");
	m_keywords.insert("replace",  "regular");
	m_keywords.insert("tokenize", "regular");

	/* Boolean */
	m_keywords.insert("true" , "boolean");
	m_keywords.insert("false", "boolean");
	m_keywords.insert("not",   "boolean");

	/* Date and Time */
	m_keywords.insert("yearMonthDuration", "datetime");
	m_keywords.insert("dayTimeDuration",   "datetime");

	/* Sequence */
	m_keywords.insert("boolean",         "sequence");
	m_keywords.insert("index-of",        "sequence");
	m_keywords.insert("empty",           "sequence");
	m_keywords.insert("exists",          "sequence");
	m_keywords.insert("distinct-values", "sequence");
	m_keywords.insert("insert-before",   "sequence");
	m_keywords.insert("remove",          "sequence");
	m_keywords.insert("reverse",         "sequence");
	m_keywords.insert("subsequence",     "sequence");
	m_keywords.insert("unordered",       "sequence");

	/* aggregate */
	m_keywords.insert("count", "aggregate");
	m_keywords.insert("avg",   "aggregate");
	m_keywords.insert("max",   "aggregate");
	m_keywords.insert("min",   "aggregate");
	m_keywords.insert("sum",   "aggregate");

	/* Context */
	m_keywords.insert("position", "context");
	m_keywords.insert("last",     "context");

}

XQueryKeyword::~XQueryKeyword() {
	if( this == s_self )
		s_self = 0;
}

XQueryKeyword * XQueryKeyword::self() {
	if( s_self == 0 ) {
		s_self = new XQueryKeyword();
		XINXStaticDeleter::self()->addObject( s_self );
	}
	return s_self;
}

const QHash<QString,QString> & XQueryKeyword::keywords() const {
	return m_keywords;
}

