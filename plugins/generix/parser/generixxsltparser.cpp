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
#include "generixxsltparser.h"

// Qt header
#include <QDebug>

// Libxml2
#include <libxslt/extensions.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xlink.h>

/* Static methode */

static void xsltExtFunctionGnxTrad( xmlXPathParserContextPtr ctxt, int nargs ) {
	if( nargs != 3 ) {
		//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
		return;
	}

	xmlXPathObjectPtr lang = valuePop(ctxt);
	xmlXPathObjectPtr label = valuePop(ctxt);
	xmlXPathObjectPtr context = valuePop(ctxt);

	qDebug() << "Lang: " << lang->stringval << ", label: " << label->stringval << ", context: " << context->stringval;

	valuePush(ctxt, label);

	xmlXPathFreeObject(lang);
	xmlXPathFreeObject(context);
}

static void xsltExtFunctionGnxTradJS( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}


static void xsltExtFunctionGnxMessage( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}

static void xsltExtFunctionGnxTime( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}

static void xsltExtFunctionGnxEncode( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}

static void xsltExtFunctionGnxNormalizeJS( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}

static void xsltExtFunctionGnxTrim( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}

static void xsltExtFunctionGnxLpad( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}

static void xsltExtFunctionGnxFormat2Number( xmlXPathParserContextPtr ctxt, int nargs ) {
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	return;
}

void* xsltExtInitFunc(xsltTransformContextPtr ctxt, const xmlChar *URI) {
	xsltRegisterExtFunction( ctxt, (xmlChar*)"trad", URI, xsltExtFunctionGnxTrad );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"tradJS", URI, xsltExtFunctionGnxTradJS );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"message", URI, xsltExtFunctionGnxMessage );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"time", URI, xsltExtFunctionGnxTime );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"encode", URI, xsltExtFunctionGnxEncode );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"normalizeJS", URI, xsltExtFunctionGnxNormalizeJS );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"trim", URI, xsltExtFunctionGnxTrim );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"lpad", URI, xsltExtFunctionGnxLpad );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"format2Number", URI, xsltExtFunctionGnxFormat2Number );


	return 0;
}

void xsltExtShutdownFunc(xsltTransformContextPtr ctxt, const xmlChar *URI, void *data) {
	Q_UNUSED( ctxt );
	Q_UNUSED( URI );
	Q_UNUSED( data );
}

/* GenerixXsltParser */

GenerixXsltParser::GenerixXsltParser() : XsltParser() {
}


void GenerixXsltParser::registerPlugin( void* ctxt ) {
	xsltExtInitFunc( static_cast<xsltTransformContextPtr>( ctxt ), (xmlChar*)"http://www.oracle.com/XSL/Transform/java/fr.generix.technicalframework.application.translation.ParserTraduc" );
}
