/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "xsltparser.h"
#include <project/xinxprojectmanager.h>
#include <project/xinxprojectproject.h>
#include <project/externalfileresolver.h>

// Qt header
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QFileInfo>
#include <QUuid>

// Libxml header
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

static bool initialized = false;

/* Static Callback */

static int xsltParserInputMatchCallback(char const * filename)
{
	Q_UNUSED(filename);
	return 1;
}

void* xsltParserInputOpenCallback(const char* filename)
{
	QFile f(filename);
	if (! f.open(QIODevice::ReadOnly))
	{
		return 0;
	}

	QString bufferString(QString::fromLatin1(f.readAll()));

	XinxProject::ProjectPtr project = XinxProject::Manager::self()->projectOfFile(filename);
	if (project)
	{
		QRegExp importRegExp("(<xsl:import[^>]+href\\s*=\\s*\")(.*)(\".*>)");
		importRegExp.setMinimal(true);
		int position = 0;
		while ((position = importRegExp.indexIn(bufferString, position)) >= 0)
		{
			QString href = importRegExp.cap(2);
			// FIXME : Must use _ctxt
			ResolverContextInformation ctxt;
			ctxt.setProject(project);
			ctxt.setCurrentPath(QFileInfo(filename).absolutePath());
			QString resolvedRef = project->resolver()->resolveFileName(href, ctxt);

			bufferString.replace(importRegExp.pos(2), href.length(), resolvedRef);

			position += importRegExp.matchedLength() + resolvedRef.length() - href.length();
		}
	}

	QBuffer * buffer = new QBuffer;
	buffer->setData(bufferString.toLatin1());
	if (! buffer->open(QIODevice::ReadOnly))
	{
		delete buffer;
		return 0;
	}

	return buffer;
}

static int xsltParserInputReadCallback(void * context, char * buffer, int len)
{
	QBuffer * b = static_cast<QBuffer*>(context);
	return b->read(buffer, len);
}

static int xsltParserInputCloseCallback(void * context)
{
	QBuffer * b = static_cast<QBuffer*>(context);
	delete b;
	return 0;
}

/* PrivateXsltParser */

class PrivateXsltParser
{
public:
	XinxProject::ProjectPtrWeak _project;
	ResolverContextInformation _resolverCtxt;

	xsltStylesheetPtr m_stylesheet;
	xmlDocPtr m_xmlDoc, m_res;
	QList<XsltParser::ErrorMessage> m_errors;

	friend void xsltParserErrorFunc(void * parser, xmlErrorPtr error);
};

/* Error parsing */

void xsltParserErrorFunc(void * parser, xmlErrorPtr error)
{
	PrivateXsltParser * d = static_cast<PrivateXsltParser*>(parser);
	if (d)
	{
		XsltParser::ErrorMessage m;
		m.isWarning = error->level <= XML_ERR_WARNING;
		m.message   = QLatin1String(error->message);
		m.message   = m.message.simplified();
		m.line      = error->line;

		d->m_errors.append(m);
	}
}

void xsltParserGenericErrorFunc(void * ctx, const char * msg, ...)
{
	va_list ap;
	va_start(ap, msg);

	QString proceedMsg;
	proceedMsg.vsprintf(msg, ap);

	qDebug() << proceedMsg.simplified();

	PrivateXsltParser * d = static_cast<PrivateXsltParser*>(ctx);
	if (d)
	{
		XsltParser::ErrorMessage m;
		m.isWarning = false;
		m.message   = proceedMsg.simplified();
		m.line      = -1;

		d->m_errors.append(m);
	}

	va_end(ap);
}

/* XsltParser */

XsltParser::XsltParser()
{
	d = new PrivateXsltParser;

	d->m_stylesheet  = NULL;
	d->m_xmlDoc      = NULL;
	d->m_res         = NULL;

	if (! initialized)
	{
		initialized = true;
		xmlSubstituteEntitiesDefault(1);
		xmlLineNumbersDefault(1);
		xmlLoadExtDtdDefaultValue = 1;

		xmlRegisterInputCallbacks(xsltParserInputMatchCallback, xsltParserInputOpenCallback, xsltParserInputReadCallback,  xsltParserInputCloseCallback);

	}
}

XsltParser::~XsltParser()
{
	xsltFreeStylesheet(d->m_stylesheet);
	xmlFreeDoc(d->m_res);
	xmlFreeDoc(d->m_xmlDoc);

	delete d;
}

void XsltParser::setProject(XinxProject::ProjectPtr project)
{
	d->_project = project.toWeakRef();
	d->_resolverCtxt.setProject(project);
	d->_resolverCtxt.setCurrentPath(project->projectPath());
}

XinxProject::ProjectPtr XsltParser::project()
{
	return d->_project.toStrongRef();
}

const QList<XsltParser::ErrorMessage> & XsltParser::errors() const
{
	return d->m_errors;
}

bool XsltParser::loadStylesheet(const QString & filename)
{
	d->_resolverCtxt = d->_project.toStrongRef()->resolver()->createContextInformation(filename);

	d->m_errors.clear();
	xmlSetStructuredErrorFunc(d, xsltParserErrorFunc);
	xmlSetGenericErrorFunc(d, xsltParserGenericErrorFunc);
	//xsltSetGenericDebugFunc(d, xsltParserGenericErrorFunc);
	d->m_stylesheet = xsltParseStylesheetFile((const xmlChar *) qPrintable(filename));
	//xsltSetGenericDebugFunc(0, 0);
	xmlSetGenericErrorFunc(0, 0);
	xmlSetStructuredErrorFunc(0, 0);

	return d->m_stylesheet;
}

bool XsltParser::loadXmlFile(const QByteArray & data)
{
	d->m_errors.clear();
	xmlSetStructuredErrorFunc(d, xsltParserErrorFunc);
	xmlSetGenericErrorFunc(d, xsltParserGenericErrorFunc);
	//xsltSetGenericDebugFunc(d, xsltParserGenericErrorFunc);
	d->m_xmlDoc = xmlParseMemory(data, data.size());
	//xsltSetGenericDebugFunc(0, 0);
	xmlSetGenericErrorFunc(0, 0);
	xmlSetStructuredErrorFunc(0, 0);

	return d->m_xmlDoc;
}

bool XsltParser::loadXmlFile(const QString & filename)
{
	d->m_errors.clear();
	xmlSetStructuredErrorFunc(d, xsltParserErrorFunc);
	xmlSetGenericErrorFunc(d, xsltParserGenericErrorFunc);
	//xsltSetGenericDebugFunc(d, xsltParserGenericErrorFunc);
	d->m_xmlDoc = xmlReadFile(qPrintable(filename), NULL, XML_PARSE_NOBLANKS);
	//xsltSetGenericDebugFunc(0, 0);
	xmlSetGenericErrorFunc(0, 0);
	xmlSetStructuredErrorFunc(0, 0);

	return d->m_xmlDoc;
}

QString XsltParser::getOutput() const
{
	d->m_errors.clear();
	Q_ASSERT(d->m_res);

	xmlChar* buffer;
	int bufferSize;

	xmlSetStructuredErrorFunc(d, xsltParserErrorFunc);
	xmlSetGenericErrorFunc(d, xsltParserGenericErrorFunc);
	//xsltSetGenericDebugFunc(d, xsltParserGenericErrorFunc);
	xsltSaveResultToString(&buffer, &bufferSize, d->m_res, d->m_stylesheet);
	//xsltSetGenericDebugFunc(0, 0);
	xmlSetGenericErrorFunc(0, 0);
	xmlSetStructuredErrorFunc(0, 0);

	return QString::fromUtf8((char*)buffer);
}

void XsltParser::registerPlugin(void* ctxt)
{
	Q_UNUSED(ctxt);
}

bool XsltParser::process()
{
	Q_ASSERT(d->m_stylesheet);
	Q_ASSERT(d->m_xmlDoc);

	d->m_errors.clear();

	if (d->m_res)
		xmlFreeDoc(d->m_res);

	xmlSetStructuredErrorFunc(d, xsltParserErrorFunc);
	xmlSetGenericErrorFunc(d, xsltParserGenericErrorFunc);
	//xsltSetGenericDebugFunc(d, xsltParserGenericErrorFunc);
	xsltTransformContextPtr ctxt = xsltNewTransformContext(d->m_stylesheet, d->m_xmlDoc);
	xsltSetTransformErrorFunc(ctxt, d, xsltParserGenericErrorFunc);

	if (ctxt == NULL)
	{
		xmlSetGenericErrorFunc(0, 0);
		xmlSetStructuredErrorFunc(0, 0);
		return 0;
	}

	registerPlugin(ctxt);

	d->m_res = xsltApplyStylesheetUser(d->m_stylesheet, d->m_xmlDoc, 0, 0, 0, ctxt);
	//xsltSetGenericDebugFunc(0, 0);
	xsltSetTransformErrorFunc(ctxt, d, 0);
	xmlSetGenericErrorFunc(0, 0);
	xmlSetStructuredErrorFunc(0, 0);

	return d->m_res;
}
