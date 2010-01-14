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
#include "xsltparser.h"
#include <project/xinxproject.h>
#include <project/externalfileresolver.h>

// Qt header
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QFileInfo>

// Libxml header
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <libxslt/extensions.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xlink.h>

static bool initialized = false;

/* PrivateXsltParser */

class PrivateXsltParser {
public:
	xsltStylesheetPtr m_stylesheet;
	xmlDocPtr m_xmlDoc, m_res;
};

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


void* xsltExtInitFunc(xsltTransformContextPtr ctxt, const xmlChar *URI) {
	xsltRegisterExtFunction( ctxt, (xmlChar*)"trad", URI, xsltExtFunctionGnxTrad );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"tradJS", URI, xsltExtFunctionGnxTradJS );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"message", URI, xsltExtFunctionGnxMessage );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"time", URI, xsltExtFunctionGnxTime );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"encode", URI, xsltExtFunctionGnxEncode );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"normalizeJS", URI, xsltExtFunctionGnxNormalizeJS );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"trim", URI, xsltExtFunctionGnxTrim );
	xsltRegisterExtFunction( ctxt, (xmlChar*)"lpad", URI, xsltExtFunctionGnxLpad );

	return 0;
}

void xsltExtShutdownFunc(xsltTransformContextPtr ctxt, const xmlChar *URI, void *data) {
	Q_UNUSED( ctxt );
	Q_UNUSED( URI );
	Q_UNUSED( data );
}

/* xmlRegisterCallback */

static int xsltParserInputMatchCallback(char const * filename) {
	Q_UNUSED( filename );
	return 1;
}

static void* xsltParserInputOpenCallback(char const * filename) {
	QFile f( filename );
	if( ! f.open( QIODevice::ReadOnly ) ) {
		return 0;
	}

	QString bufferString( QString::fromLatin1( f.readAll() ) );

	if( XINXProjectManager::self()->project() ) {
		QRegExp importRegExp( "(<xsl:import[^>]+href\\s*=\\s*\")(.*)(\".*>)" );
		importRegExp.setMinimal( true );
		int position = 0;
		while( ( position = importRegExp.indexIn( bufferString, position ) ) >= 0 ) {
			QString href = importRegExp.cap( 2 );
			QString resolvedRef = ExternalFileResolver::self()->resolveFileName( href, QFileInfo( filename ).absolutePath() );

			bufferString.replace( importRegExp.pos( 2 ), href.length(), resolvedRef );

			position += importRegExp.matchedLength() + resolvedRef.length() - href.length();
		}
	}

	QBuffer * b = new QBuffer;
	b->setData( bufferString.toLatin1() );
	if( ! b->open( QIODevice::ReadOnly ) ) {
		delete b;
		return 0;
	}

	return b;
}

static int xsltParserInputReadCallback(void * context, char * buffer, int len) {
	QBuffer * f = static_cast<QBuffer*>( context );
	return f->read( buffer, len );
}

static int xsltParserInputCloseCallback(void * context) {
	delete static_cast<QBuffer*>( context );
	return 0;
}

/* XsltParser */

XsltParser::XsltParser() {
	d = new PrivateXsltParser;

	d->m_stylesheet = NULL;
	d->m_xmlDoc     = NULL;
	d->m_res        = NULL;

	if( ! initialized ) {
		initialized = true;
		xmlSubstituteEntitiesDefault(1);
		xmlLoadExtDtdDefaultValue = 1;

		xmlRegisterInputCallbacks( xsltParserInputMatchCallback, xsltParserInputOpenCallback,
								   xsltParserInputReadCallback,  xsltParserInputCloseCallback);
	}
}

XsltParser::~XsltParser() {
	xsltFreeStylesheet( d->m_stylesheet );
	xmlFreeDoc( d->m_res );
	xmlFreeDoc( d->m_xmlDoc );

	delete d;
}

/*bool XsltParser::loadStylesheet( const QByteArray & data ) {
	xmlDocPtr xslDoc = xmlParseMemory( data, data.size() );
	d->m_stylesheet = xsltParseStylesheetDoc( xslDoc );

	return d->m_stylesheet;
}*/

bool XsltParser::loadStylesheet( const QString & filename ) {
	d->m_stylesheet = xsltParseStylesheetFile( (const xmlChar *) qPrintable( filename ) );

	return d->m_stylesheet;
}

bool XsltParser::loadXmlFile( const QByteArray & data ) {
	d->m_xmlDoc = xmlParseMemory( data, data.size() );

	return d->m_xmlDoc;
}

bool XsltParser::loadXmlFile( const QString & filename ) {
	d->m_xmlDoc = xmlParseFile( qPrintable(filename) );

	return d->m_xmlDoc;
}

QString XsltParser::getOutput() const {
	Q_ASSERT( d->m_res );

	xmlChar* buffer;
	int bufferSize;

	xsltSaveResultToString( &buffer, &bufferSize, d->m_res, d->m_stylesheet );

	return QLatin1String( (char*)buffer );
}

bool XsltParser::process() {
	Q_ASSERT( d->m_stylesheet );
	Q_ASSERT( d->m_xmlDoc );

	if( d->m_res )
		xmlFreeDoc( d->m_res );

	xsltTransformContextPtr ctxt = xsltNewTransformContext( d->m_stylesheet, d->m_xmlDoc );
	if( ctxt == NULL )
		return 0;

	xsltExtInitFunc( ctxt, (xmlChar*)"http://www.oracle.com/XSL/Transform/java/fr.generix.technicalframework.application.translation.ParserTraduc" );

	d->m_res = xsltApplyStylesheetUser( d->m_stylesheet, d->m_xmlDoc, 0, 0, 0, ctxt );

	return d->m_res;
}
