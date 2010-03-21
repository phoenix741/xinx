/* *********************************************************************** *
* XINX                                                                    *
* Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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
#include <QUrl>
#include <QRegExp>
#include <QLocale>
#include <QDateTime>

// Libxml2
#include <libxslt/extensions.h>
#include <libxml/xpathInternals.h>
#include <libxslt/xsltutils.h>
#include <libxml/globals.h>

// Std header
#include <ctime>

/* Static methode */

namespace Gnx
{
namespace XsltExtention
{

static void convertToString(xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr & ptr)
{
	if (ptr->type != XPATH_STRING)
	{
		valuePush(ctxt, ptr);
		xmlXPathStringFunction(ctxt, 1);
		ptr = valuePop(ctxt);
	}
}

static QString qtValuePopString(xmlXPathParserContextPtr ctxt)
{
	xmlXPathObjectPtr str = valuePop(ctxt);
	convertToString(ctxt, str);
	QString result = QString::fromUtf8((char*)str->stringval);
	xmlXPathFreeObject(str);
	return result;
}

static void qtValuePush(xmlXPathParserContextPtr ctxt, QString value)
{
	valuePush(ctxt, xmlXPathNewString((xmlChar*)value.toUtf8().data()));
}

static void convertToNumber(xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr & ptr)
{
	if (ptr->type != XPATH_NUMBER)
	{
		valuePush(ctxt, ptr);
		xmlXPathNumberFunction(ctxt, 1);
		ptr = valuePop(ctxt);
	}
}

static double qtValuePopNumber(xmlXPathParserContextPtr ctxt)
{
	xmlXPathObjectPtr str = valuePop(ctxt);
	convertToNumber(ctxt, str);
	double result = str->floatval;
	xmlXPathFreeObject(str);
	return result;
}

static void qtValuePush(xmlXPathParserContextPtr ctxt, double value)
{
	valuePush(ctxt, xmlXPathNewFloat(value));
}

static void convertToBool(xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr & ptr)
{
	if (ptr->type != XPATH_BOOLEAN)
	{
		valuePush(ctxt, ptr);
		xmlXPathNumberFunction(ctxt, 1);
		ptr = valuePop(ctxt);
	}
}

static bool qtValuePopBoolean(xmlXPathParserContextPtr ctxt)
{
	xmlXPathObjectPtr str = valuePop(ctxt);
	convertToBool(ctxt, str);
	bool result = str->boolval;
	xmlXPathFreeObject(str);
	return result;
}

static void qtValuePush(xmlXPathParserContextPtr ctxt, bool value)
{
	valuePush(ctxt, xmlXPathNewBoolean(value));
}

static void trad(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "trad: number of argument incorrect\n");
		return;
	}

	QString lang = qtValuePopString(ctxt);
	QString label = qtValuePopString(ctxt);
	QString context = qtValuePopString(ctxt);

	qtValuePush(ctxt, label);
}

static void tradJS(xmlXPathParserContextPtr ctxt, int nargs)
{
	valuePush(ctxt, xmlXPathNewNodeSet(NULL));
}

static void message(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs == 0)
	{
		xsltGenericError(xsltGenericErrorContext, "message: number of argument incorrect\n");
		return;
	}

	QStack<QString> stack;
	for (int i = 0; i < nargs; i++)
	{
		stack.push(qtValuePopString(ctxt));
	}

	QString message = stack.pop();

	for (int i = 1; i < nargs; i++)
	{
		const QString p = stack.pop();

		int pos = message.indexOf(QString("@%1").arg(i));
		if (pos >= 0)
		{
			message.replace(pos, 2, p);
		}
		else
		{
			pos = message.indexOf("@");
			if (pos >= 0)
			{
				message.replace(pos, 1, p);
			}
		}
	}

	qtValuePush(ctxt, message);
}

static void time(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs > 0)
	{
		xsltGenericError(xsltGenericErrorContext, "time: number of argument incorrect\n");
		return;
	}

#define rdtsc(low, high) __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

	quint32 low, high;
	quint64 result;
	rdtsc(low, high);
	result = ((quint64)high << 32) | low;

	qtValuePush(ctxt, (double)result);
}

static void timeMs(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs == 0)
	{
		xsltGenericError(xsltGenericErrorContext, "timeMs: number of argument incorrect\n");
		return;
	}

	clock_t c = clock();
	qtValuePush(ctxt, (double)c);
}

static void encode(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (!(nargs == 1 || nargs == 2))
	{
		xsltGenericError(xsltGenericErrorContext, "encode: number of argument incorrect\n");
		return;
	}

	QString encoding = "UTF-8";
	if (nargs == 2)
	{
		encoding = qtValuePopString(ctxt);
	}
	if (encoding != "UTF-8")
	{
		xsltGenericError(xsltGenericErrorContext, "encode: can't encode another encoding that UTF-8\n");
		return;
	}

	QString text = qtValuePopString(ctxt);
	QByteArray ba = QUrl::toPercentEncoding(text);

	qtValuePush(ctxt, QString(ba));
}

static void decode(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (!(nargs == 1 || nargs == 2))
	{
		xsltGenericError(xsltGenericErrorContext, "decode: number of argument incorrect\n");
		return;
	}

	QString encoding = "UTF-8";
	if (nargs == 2)
	{
		encoding = qtValuePopString(ctxt);
	}
	if (encoding != "UTF-8")
	{
		xsltGenericError(xsltGenericErrorContext, "decode: can't encode another encoding that UTF-8\n");
		return;
	}

	QString text   = qtValuePopString(ctxt);
	QString result = QUrl::fromPercentEncoding(text.toAscii());

	qtValuePush(ctxt, result);
}

static void replace(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "replace: number of argument incorrect\n");
		return;
	}

	QString replacement = qtValuePopString(ctxt);
	QString regexpStr   = qtValuePopString(ctxt);
	QString text        = qtValuePopString(ctxt);

	text.replace(QRegExp(regexpStr), replacement);

	qtValuePush(ctxt, text);
}

static void match(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 2)
	{
		xsltGenericError(xsltGenericErrorContext, "match: number of argument incorrect\n");
		return;
	}

	QString regexpStr   = qtValuePopString(ctxt);
	QString text        = qtValuePopString(ctxt);

	if (text.contains(QRegExp(regexpStr)))
	{
		qtValuePush(ctxt, true);
	}
	else
	{
		qtValuePush(ctxt, false);
	}
}

static void replaceQuote(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 1)
	{
		xsltGenericError(xsltGenericErrorContext, "replaceQuote: number of argument incorrect\n");
		return;
	}

	QString text = qtValuePopString(ctxt);
	text.replace('\'', '¤');
	qtValuePush(ctxt, text);
}


static void normalizeJS(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 1)
	{
		xsltGenericError(xsltGenericErrorContext, "normalizeJS: number of argument incorrect\n");
		return;
	}

	QString text = qtValuePopString(ctxt);
	text.replace(QRegExp("[^\\]\""), "\\\"");
	text.replace(QRegExp("[^\\]\'"), "\\\'");
	qtValuePush(ctxt, text);
}

static void trim(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 1)
	{
		xsltGenericError(xsltGenericErrorContext, "trim: number of argument incorrect\n");
		return;
	}

	QString text = qtValuePopString(ctxt);
	text = text.trimmed();
	qtValuePush(ctxt, text);
}

static void rtrim(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 1)
	{
		xsltGenericError(xsltGenericErrorContext, "rtrim: number of argument incorrect\n");
		return;
	}

	QString text = qtValuePopString(ctxt);

	while (text.count() && (text.endsWith(' ') || text.endsWith('\t')))
	{
		text.remove(text.size() - 1, 1);
	}

	qtValuePush(ctxt, text);
}

static void ltrim(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 1)
	{
		xsltGenericError(xsltGenericErrorContext, "ltrim: number of argument incorrect\n");
		return;
	}

	QString text = qtValuePopString(ctxt);

	while (text.count() && (text.startsWith(' ') || text.startsWith('\t')))
	{
		text.remove(0, 1);
	}

	qtValuePush(ctxt, text);
}

static void lpad(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3 && nargs != 2)
	{
		xsltGenericError(xsltGenericErrorContext, "lpad: number of argument incorrect\n");
		return;
	}

	const QString completion = nargs == 3 ? qtValuePopString(ctxt) : " ";
	const int     len        = qtValuePopNumber(ctxt);
	const QString s          = qtValuePopString(ctxt);
	QString result     = s;

	while (result.size() < len)
		result = completion + result;

	qtValuePush(ctxt, result);
}

static void rpad(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3 && nargs != 2)
	{
		xsltGenericError(xsltGenericErrorContext, "rpad: number of argument incorrect\n");
		return;
	}

	const QString completion = nargs == 3 ? qtValuePopString(ctxt) : " ";
	const int     len        = qtValuePopNumber(ctxt);
	const QString s          = qtValuePopString(ctxt);
	QString result     = s;

	while (result.size() < len)
		result = result + completion;

	qtValuePush(ctxt, result);
}

static void formatNumber(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "formatNumber: number of argument incorrect\n");
		return;
	}

	const QString locale  = qtValuePopString(ctxt);
	const QString format  = qtValuePopString(ctxt);
	const QString value   = qtValuePopString(ctxt);
	QString result        = value;

	if (value == "NaN")
	{
		qtValuePush(ctxt, value);
		return;
	}

	int intValue = 0;
	if (!value.isEmpty())
	{
		result.replace(",", ".");
		result.replace(" ", "");
		intValue = result.toInt();
	}

	result = QLocale::c().toString(intValue);

	qtValuePush(ctxt, result);
}

static void format2Number(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 5)
	{
		xsltGenericError(xsltGenericErrorContext, "format2Number: number of argument incorrect\n");
		return;
	}

	const QString locale  = qtValuePopString(ctxt);
	const QString gs      = qtValuePopString(ctxt);
	const QString dc      = qtValuePopString(ctxt);
	const QString format  = qtValuePopString(ctxt);
	const QString value   = qtValuePopString(ctxt);

	if (value == "NaN")
	{
		qtValuePush(ctxt, value);
		return;
	}

	int intValue = value.toInt();

	QString result = QLocale::system().toString(intValue);

	qtValuePush(ctxt, result);
}

static void formatNumericToGCE(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "formatNumericToGCE: number of argument incorrect\n");
		return;
	}

	const QString decimalSymbol = qtValuePopString(ctxt);
	const bool    dec           = qtValuePopBoolean(ctxt);
	const QString value         = qtValuePopString(ctxt);
	QString result;

	if (! value.isEmpty())
	{
		bool dot = false;

		for (int i = value.size() - 1; i >= 0; i--)
		{
			QChar c = value.at(i);

			if (c == '.' || c == ',')
			{
				if (!dot && c == decimalSymbol.at(0))
				{
					dot = true;

					if (dec)
					{
						result.insert(0, '.');
					}
					else
					{
						result.clear();
					}
				}
			}
			else if (c == ' ')
			{
			}
			else if (c >= '0' && c <= '9')
			{
				result.insert(0, c);
			}
			else if ((c == '+' || c == '-') && i == 0)
			{
				// Pas la peine d'ajouter le plus
				if (c == '-')
				{
					result.insert(0, c);
				}
			}
			else
			{
				xsltGenericError(xsltGenericErrorContext, "formatNumericToGCE: character incorrect\n");
				return;
			}
		}
	}
	else
	{
		result = "0";
	}


	qtValuePush(ctxt, result);
}

static void formatDate(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "formatDate: number of argument incorrect\n");
		return;
	}

	const QString internalFormat = qtValuePopString(ctxt);
	const QString format         = qtValuePopString(ctxt).trimmed();
	const QString date           = qtValuePopString(ctxt).trimmed();
	QString result;

	if (!date.isEmpty() && !format.isEmpty())
	{
		QDateTime dt = QDateTime::fromString(date, internalFormat);
		result       = dt.toString(format);
	}

	qtValuePush(ctxt, result);
}

static void formatDateToGce(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "formatDateToGce: number of argument incorrect\n");
		return;
	}

	const QString format         = qtValuePopString(ctxt).trimmed();
	const QString date           = qtValuePopString(ctxt).trimmed();
	QString result;

	if (!date.isEmpty() && !format.isEmpty())
	{
		QDateTime dt = QDateTime::fromString(date, format);
		result       = dt.toString("yyyyMMdd");
	}

	qtValuePush(ctxt, result);
}

static void formatTime(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 3)
	{
		xsltGenericError(xsltGenericErrorContext, "formatTime: number of argument incorrect\n");
		return;
	}

	const QString internalFormat = qtValuePopString(ctxt);
	const QString format         = qtValuePopString(ctxt).trimmed();
	const QString date           = qtValuePopString(ctxt).trimmed();
	QString result;

	if (!date.isEmpty() && !format.isEmpty())
	{
		QDateTime dt = QDateTime::fromString(date, internalFormat);
		result       = dt.toString(format);
	}

	qtValuePush(ctxt, result);
}

static void createMask(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 5)
	{
		xsltGenericError(xsltGenericErrorContext, "createMask: number of argument incorrect\n");
		return;
	}

	const QString groupingSymbol = qtValuePopString(ctxt);
	const QString decimalSymbol  = qtValuePopString(ctxt);
	const int nbZero             = qtValuePopNumber(ctxt);
	const int lgDec              = qtValuePopNumber(ctxt);
	const int lgInt              = qtValuePopNumber(ctxt);
	QString result;

	// Construction de la partie décimale
	for (int i = 0; i < lgDec; i++)
	{
		result.insert(0, '0');
	}

	if (lgDec > 0)
	{
		result.insert(0, decimalSymbol);
	}

	// Construction de la partie entière
	int lgMax = lgInt;
	int zero = 0;

	for (int i = 0; i < lgMax; i++)
	{
		if (((i + 1) % 4 == 0) && !groupingSymbol.isEmpty() && groupingSymbol != "s")
		{
			result.insert(0, groupingSymbol);
			lgMax++;
		}
		else if (zero < nbZero)
		{
			result.insert(0, '0');
			zero++;
		}
		else
		{
			result.insert(0, '#');
		}
	}

	qtValuePush(ctxt, result);
}

static void toUpperCase(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 1)
	{
		xsltGenericError(xsltGenericErrorContext, "toUpperCase: number of argument incorrect\n");
		return;
	}

	QString text = qtValuePopString(ctxt);
	text = text.toUpper();
	qtValuePush(ctxt, text);
}

static void toLowerCase(xmlXPathParserContextPtr ctxt, int nargs)
{
	if (nargs != 1)
	{
		xsltGenericError(xsltGenericErrorContext, "toLowerCase: number of argument incorrect\n");
		return;
	}

	QString text = qtValuePopString(ctxt);
	text = text.toLower();
	qtValuePush(ctxt, text);
}

static void getScreenValue(xmlXPathParserContextPtr ctxt, int nargs)
{
	QString screen_data, business_data, result;

	xmlXPathObjectPtr nodeList = valuePop(ctxt);
	if (nodeList->type == XPATH_NODESET)
	{
		xmlNodeSetPtr nodes = nodeList->nodesetval;
		int size = nodes ? nodes->nodeNr : 0;
		for (int i = 0; i < size; i++)
		{
			if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE)
			{
				xmlElementPtr node = (xmlElementPtr)nodes->nodeTab[i];
				xmlNodePtr child   = node->children;

				while (child != NULL)
				{
					if (child->type == XML_ELEMENT_NODE)
					{
						const QString name = QString::fromUtf8((char*)child->name);
						xmlChar* content = xmlNodeGetContent(child);
						if (name == "screen_data")
						{
							screen_data += QString::fromUtf8((char*)content);
						}
						if (name == "business_data")
						{
							business_data += QString::fromUtf8((char*)content);
						}
						xmlFree(content);
					}

					child = child->next;
				}
				if (!(screen_data.isEmpty() && business_data.isEmpty())) break;
			}
		}
	}
	if (!screen_data.isEmpty())
	{
		result = screen_data;
	}
	else if (!business_data.isEmpty())
	{
		result = business_data;
	}

	qtValuePush(ctxt, result);
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
	xsltRegisterExtFunction(ctxt, (xmlChar*)"formatTime", URI, Gnx::XsltExtention::formatTime);
	xsltRegisterExtFunction(ctxt, (xmlChar*)"formatNumericToGce", URI, Gnx::XsltExtention::formatNumericToGCE);

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
