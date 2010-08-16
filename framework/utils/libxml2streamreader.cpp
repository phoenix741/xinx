/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2006-2010 by Ulrich Van Den Hekke                         *
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
#include "libxml2streamreader.h"

// Libxml2 header
#include <libxml/xmlreader.h>

/* PrivateLibXml2StreamReader */

class PrivateLibXml2StreamReader
{
public:
	PrivateLibXml2StreamReader();

	QIODevice * m_device;

	xmlTextReaderPtr m_reader;
	int m_ret;

	bool m_hasError, m_namespaceProcesssing;
	bool m_simulateEndElement, m_isSimulatedEndElement;
	QString m_errorString;
};

PrivateLibXml2StreamReader::PrivateLibXml2StreamReader()
{
	m_ret                   = 1;
	m_hasError              = false;
	m_namespaceProcesssing  = true;
	m_simulateEndElement    = false;
	m_isSimulatedEndElement = false;
	m_device                = NULL;
	m_reader                = NULL;
}

/* Static member */

int libxml2read (void * context, char * buffer, int len)
{
	PrivateLibXml2StreamReader * d = (PrivateLibXml2StreamReader*)context;
	return d->m_device->read(buffer, len);
}

int libxml2close (void * context)
{
	PrivateLibXml2StreamReader * d = (PrivateLibXml2StreamReader*)context;
	d->m_device->close();
	return 0;
}

void libxml2error (void * arg, const char * msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator)
{
	PrivateLibXml2StreamReader * d = (PrivateLibXml2StreamReader*)arg;

	switch (severity)
	{
	case XML_PARSER_SEVERITY_VALIDITY_WARNING:
	case XML_PARSER_SEVERITY_WARNING:
	case XML_PARSER_SEVERITY_VALIDITY_ERROR:
		break;
	case XML_PARSER_SEVERITY_ERROR:
		d->m_hasError    = true;
		d->m_ret         = 0;
	}

	d->m_errorString = QString::fromAscii(msg);
}

void libxml2structured_error (void *userData, xmlErrorPtr error)
{
	PrivateLibXml2StreamReader * d = (PrivateLibXml2StreamReader*)userData;

	switch(error->level)
	{
	case XML_ERR_NONE:
	case XML_ERR_WARNING:
		break;
	case XML_ERR_ERROR:
	case XML_ERR_FATAL:
		d->m_hasError    = true;
		d->m_ret         = 0;
	}

	d->m_errorString = QString::fromAscii(error->message);
}

/* LibXml2StreamReader */

LibXml2StreamReader::LibXml2StreamReader()
{
	d = new PrivateLibXml2StreamReader;
}

LibXml2StreamReader::LibXml2StreamReader(QIODevice * device)
{
	d = new PrivateLibXml2StreamReader;
	setDevice(device);
}

LibXml2StreamReader::~LibXml2StreamReader()
{
	delete d;

	xmlCleanupParser();
}

QString LibXml2StreamReader::documentEncoding () const
{
	return QString::fromUtf8((char*)xmlTextReaderConstEncoding(d->m_reader));
}

QString LibXml2StreamReader::documentVersion () const
{
	return QString::fromUtf8((char*)xmlTextReaderConstXmlVersion(d->m_reader));
}

QString LibXml2StreamReader::errorString () const
{
	return d->m_errorString;
}

bool LibXml2StreamReader::hasError () const
{
	return d->m_hasError;
}

QString LibXml2StreamReader::attribute(const QString & name)
{
	xmlChar * value = xmlTextReaderGetAttribute(d->m_reader, (xmlChar*)name.toUtf8().data());
	QString returnedValue = QString::fromUtf8((char*)value);
	xmlFree(value);

	return returnedValue;
}

bool LibXml2StreamReader::isStartElement () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_ELEMENT;
}

bool LibXml2StreamReader::isCharacters () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_TEXT;
}

bool LibXml2StreamReader::isCDATA () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_CDATA;
}

bool LibXml2StreamReader::isEntityReference () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_ENTITY_REFERENCE;
}

bool LibXml2StreamReader::isEntity () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_ENTITY;
}

bool LibXml2StreamReader::isProcessingInstruction () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_PROCESSING_INSTRUCTION;
}

bool LibXml2StreamReader::isComment () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_COMMENT;
}

bool LibXml2StreamReader::isStartDocument () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_DOCUMENT;
}

bool LibXml2StreamReader::isDocumentType () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_DOCUMENT_TYPE;
}

bool LibXml2StreamReader::isDocumentFragment () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_DOCUMENT_FRAGMENT;
}

bool LibXml2StreamReader::isWhitespace () const
{
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_WHITESPACE;
}

bool LibXml2StreamReader::isEndElement () const
{
	return (xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_END_ELEMENT) || d->m_isSimulatedEndElement;
}

qint64 LibXml2StreamReader::columnNumber () const
{
	return xmlTextReaderGetParserColumnNumber(d->m_reader);
}

qint64 LibXml2StreamReader::lineNumber () const
{
	xmlNodePtr node = xmlTextReaderCurrentNode(d->m_reader);
	if (node)
	{
		return xmlGetLineNo(node);
	}
	return 0;
}

QIODevice *	LibXml2StreamReader::device () const
{
	return d->m_device;
}

void LibXml2StreamReader::setDevice ( QIODevice * device )
{
	if (d->m_device != device)
	{
		if (d->m_device)
		{
			xmlFreeTextReader(d->m_reader);
		}

		int options = 0;

		d->m_device = device;
		d->m_reader = xmlReaderForIO(libxml2read, libxml2close, d, NULL, NULL, options);
		xmlTextReaderSetErrorHandler(d->m_reader, libxml2error, d);
		xmlTextReaderSetStructuredErrorHandler(d->m_reader, libxml2structured_error, d);
	}
}

void LibXml2StreamReader::raiseError ( const QString & message )
{
	d->m_errorString = message;
	d->m_hasError    = true;
	d->m_ret         = 0;
}

QString LibXml2StreamReader::readElementText ()
{
	Q_ASSERT(xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_ELEMENT);

	QString returnedValue;
	if (!xmlTextReaderIsEmptyElement(d->m_reader))
	{
		xmlChar * value;
		int startLevel, level;
		bool endLoop = false;
		startLevel = level = xmlTextReaderDepth (d->m_reader);

		d->m_ret = xmlTextReaderRead(d->m_reader);
		while (d->m_ret == 1)
		{
			switch (xmlTextReaderNodeType(d->m_reader))
			{
			case XML_READER_TYPE_CDATA:
			case XML_READER_TYPE_ENTITY_REFERENCE:
			case XML_READER_TYPE_TEXT:
				value = xmlTextReaderReadString(d->m_reader);
				returnedValue += QString::fromUtf8((char*)value);
				xmlFree(value);
				break;
			case XML_READER_TYPE_END_ELEMENT:
				if(startLevel == level)
				{
					endLoop = true;
				}
				else
				{
					level--;
				}
				break;
			case XML_READER_TYPE_ELEMENT:
				if (!xmlTextReaderIsEmptyElement(d->m_reader))
					level++;
				break;
			}

			if (endLoop)
				break;

			d->m_ret = xmlTextReaderRead(d->m_reader);
		}
	}

	d->m_isSimulatedEndElement = false;
	d->m_simulateEndElement    = false;

	return returnedValue;
}

void LibXml2StreamReader::skipCurrentElement ()
{
	readElementText();
}

bool LibXml2StreamReader::atEnd () const
{
	return ! (d->m_ret == 1);
}

void LibXml2StreamReader::readNext ()
{
	if (!d->m_hasError)
	{
		if (!d->m_simulateEndElement)
		{
			d->m_ret = xmlTextReaderRead(d->m_reader);
			if (isStartElement() && xmlTextReaderIsEmptyElement(d->m_reader))
			{
				d->m_simulateEndElement = true;
			}
			d->m_isSimulatedEndElement = false;
		}
		else
		{
			d->m_simulateEndElement = false;
			d->m_isSimulatedEndElement = true;
		}
	}
}

QString LibXml2StreamReader::name () const
{
	if (d->m_namespaceProcesssing)
		return QString::fromUtf8((char*)xmlTextReaderConstLocalName(d->m_reader));
	else
		return QString::fromUtf8((char*)xmlTextReaderConstName(d->m_reader));
}

QString LibXml2StreamReader::prefix () const
{
	if (d->m_namespaceProcesssing)
		return QString::fromUtf8((char*)xmlTextReaderConstPrefix(d->m_reader));
	else
		return QString();
}

QString LibXml2StreamReader::text () const
{
	xmlChar * value = xmlTextReaderReadString(d->m_reader);
	QString returnedValue = QString::fromUtf8((char*)value);
	xmlFree(value);

	return returnedValue;
}


void LibXml2StreamReader::setNamespaceProcessing (bool value)
{
	d->m_namespaceProcesssing = value;
}

bool LibXml2StreamReader::namespaceProcessing() const
{
	return d->m_namespaceProcesssing;
}

bool LibXml2StreamReader::validate(const QString & uri)
{
	if (d->m_reader)
	{
		int ret = xmlTextReaderSchemaValidate(d->m_reader, uri.toLatin1().data());

		if (ret == -1)
			return false;
		else
			return true;
	}

	return false;
}
