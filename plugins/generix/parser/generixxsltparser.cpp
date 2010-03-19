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
#include <QStack>

// Libxml2
#include <libxslt/extensions.h>
#include <libxml/xpathInternals.h>
#include <libxslt/xsltutils.h>

/* Static methode */

namespace Gnx
{
namespace XsltExtention
{

static void convertToString(xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr ptr)
{
	if (ptr->type != XPATH_STRING) {
		valuePush(ctxt, ptr);
		xmlXPathStringFunction(ctxt, 1);
		ptr = valuePop(ctxt);
	}
}

static void trad(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
		return;
	}

	xmlXPathObjectPtr lang = valuePop(ctxt);
	xmlXPathObjectPtr label = valuePop(ctxt);
	xmlXPathObjectPtr context = valuePop(ctxt);
	convertToString(ctxt, lang);
	convertToString(ctxt, label);
	convertToString(ctxt, context);

	valuePush(ctxt, label);

	xmlXPathFreeObject(lang);
	xmlXPathFreeObject(context);
}

static void tradJS(xmlXPathParserContextPtr ctxt, int nargs)
{
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}


static void message(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs == 0)
	{
		xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
		return;
	}

	QStack<QString> stack;
	for(int i = 0; i < nargs; i++)
	{
		xmlXPathObjectPtr str = valuePop(ctxt);
		convertToString(ctxt, str);
		stack.push(QLatin1String((char*)str->stringval));
		xmlXPathFreeObject(str);
	}

	QString message = stack.pop();

	for(int i = 1; i < nargs; i++)
	{
		const QString p = stack.pop();

		int pos = message.indexOf(QString("@%1").arg(i));
		if(pos >= 0)
		{
			message.replace(pos, 2, p);
		}
		else
		{
			pos = message.indexOf("@");
			if(pos >= 0)
			{
				message.replace(pos, 1, p);
			}
		}
	}

	xmlXPathObjectPtr result = xmlXPathNewCString(qPrintable(message));
	valuePush(ctxt, result);

	return;
}

static void time(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
//			  Aucun paramètre:
//			  Retourne System.nanoTime()
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void timeMs(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
//			  Aucun paramètre:
//			  Retourne System.nanoTime() convertit en ms
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void encode(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
//				1 ou 2 paramètre.
//				2nd paramètre = UTF-8 par défaut
//
//				Appel java.net.URLEncoder.encode(s, cs);
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void decode(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
//				1 ou 2 paramètre.
//				2nd paramètre = UTF-8 par défaut
//
//				Appel java.net.URLEncoder.decode(s, cs);
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void replace(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
//			  3 paramètre :
//				s, regexp, replacement
//				dans s remplace toutes les expressions de regexp par replacement
//
//				retourne le résultat
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void match(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
//			  2 paramètre :
//				vrai si on trouve le second paramétre (regexp) dans le premier
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void replaceQuote(xmlXPathParserContextPtr ctxt, int nargs)
{
//			 retourn en chaine le premier parametre avec ' remplacé par ¤
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}


static void normalizeJS(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void trim(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void rtrim(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void ltrim(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void lpad(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void rpad(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void formatNumber(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void formatDate(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void formatDateToGce(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void format2Number(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void createMask(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void toUpperCase(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void toLowerCase(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

static void getScreenValue(xmlXPathParserContextPtr ctxt, int nargs)
{
	//xsltGenericError(xsltGenericErrorContext, "gnxTrad: number of argument incorrect\n");
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
	return;
}

}
}

void* xsltExtInitFunc(xsltTransformContextPtr ctxt, const xmlChar *URI)
{
	xsltRegisterExtFunction(ctxt, (xmlChar*)"encode", URI, Gnx::XsltExtention::encode);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"decode", URI, Gnx::XsltExtention::decode);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"time", URI, Gnx::XsltExtention::time);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"timeMs", URI, Gnx::XsltExtention::timeMs);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"replace", URI, Gnx::XsltExtention::replace);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"match", URI, Gnx::XsltExtention::match);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"replaceQuote", URI, Gnx::XsltExtention::replaceQuote);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"toUpperCase", URI, Gnx::XsltExtention::toUpperCase);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"toLowerCase", URI, Gnx::XsltExtention::toLowerCase);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"lpad", URI, Gnx::XsltExtention::lpad);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"rpad", URI, Gnx::XsltExtention::rpad);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"trim", URI, Gnx::XsltExtention::trim);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"ltrim", URI, Gnx::XsltExtention::ltrim);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"rtrim", URI, Gnx::XsltExtention::rtrim);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"message", URI, Gnx::XsltExtention::message);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"normalizeJS", URI, Gnx::XsltExtention::normalizeJS);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"format2Number", URI, Gnx::XsltExtention::format2Number);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"formatNumber", URI, Gnx::XsltExtention::formatNumber);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"formatDate", URI, Gnx::XsltExtention::formatDate);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"formatDateToGce", URI, Gnx::XsltExtention::formatDateToGce);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"getScreenValue", URI, Gnx::XsltExtention::getScreenValue);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"createMask", URI, Gnx::XsltExtention::createMask);

	xsltRegisterExtFunction(ctxt, (xmlChar*)"trad", URI, Gnx::XsltExtention::trad);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"tradJS", URI, Gnx::XsltExtention::tradJS);


	return 0;
}

void xsltExtShutdownFunc(xsltTransformContextPtr ctxt, const xmlChar *URI, void *data)
{
	Q_UNUSED(ctxt);
	Q_UNUSED(URI);
	Q_UNUSED(data);
}

/* GenerixXsltParser */

GenerixXsltParser::GenerixXsltParser() : XsltParser()
{
}


void GenerixXsltParser::registerPlugin(void* ctxt)
{
	xsltExtInitFunc(static_cast<xsltTransformContextPtr>(ctxt), (xmlChar*)"http://www.oracle.com/XSL/Transform/java/fr.generix.technicalframework.application.translation.ParserTraduc");
}
