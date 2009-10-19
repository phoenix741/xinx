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

// Qt header
#include <QDir>
#include <QDebug>

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

extern int xmlLoadExtDtdDefaultValue;

/* Static methode */

xmlExternalEntityLoader defaultEntityLoader = NULL;

static xmlParserInputPtr xsltprocExternalEntityLoader( const char *URL, const char *ID, xmlParserCtxtPtr ctxt ) {
	if( ! XINXProjectManager::self()->project() ) return defaultEntityLoader( URL, ID, ctxt );

	xmlParserInputPtr ret;
	warningSAXFunc warning = NULL;

	if ((ctxt != NULL) && (ctxt->sax != NULL)) {
		warning = ctxt->sax->warning;
		ctxt->sax->warning = NULL;
	}

	ret = defaultEntityLoader( URL, ID, ctxt);
	if( ret != NULL ) {
		if( warning != NULL )
			ctxt->sax->warning = warning;
		return ret;
	}

	QStringList searchPaths = XINXProjectManager::self()->project()->processedSearchPathList();
	foreach( QString searchPath, searchPaths ) {
		QString newUrl = QDir( searchPath ).absoluteFilePath( QLatin1String( URL ) );

		ret = defaultEntityLoader( qPrintable(newUrl), ID, ctxt);
		if( ret != NULL ) {
			if( warning != NULL )
				ctxt->sax->warning = warning;
			return ret;
		}
	}

	qWarning() << XsltParser::tr("Failed to load external entity : \"%1\"").arg( QLatin1String( URL ) );
	return NULL;
}

/* PrivateXsltParser */

class PrivateXsltParser {
public:
	xsltStylesheetPtr m_stylesheet;
	xmlDocPtr m_xmlDoc, m_res;
};

/* XsltParser */

XsltParser::XsltParser() {
	d = new PrivateXsltParser;

	d->m_stylesheet = NULL;
	d->m_xmlDoc     = NULL;
	d->m_res        = NULL;

	if( ! defaultEntityLoader ) {
		defaultEntityLoader = xmlGetExternalEntityLoader();
		xmlSetExternalEntityLoader(xsltprocExternalEntityLoader);

		xmlSubstituteEntitiesDefault(1);
		xmlLoadExtDtdDefaultValue = 1;
	}
}

XsltParser::~XsltParser() {
	xsltFreeStylesheet( d->m_stylesheet );
	xmlFreeDoc( d->m_res );
	xmlFreeDoc( d->m_xmlDoc );

	xsltCleanupGlobals();
	xmlCleanupParser();

	delete d;
}

bool XsltParser::loadStylesheet( const QByteArray & data ) {
	xmlDocPtr xslDoc = xmlParseMemory( data, data.size() );
	d->m_stylesheet = xsltParseStylesheetDoc( xslDoc );

	return d->m_stylesheet;
}

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

	d->m_res = xsltApplyStylesheet( d->m_stylesheet, d->m_xmlDoc, 0 );

	return d->m_res;
}
