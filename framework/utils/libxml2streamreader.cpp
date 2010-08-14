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
};

PrivateLibXml2StreamReader::PrivateLibXml2StreamReader()
{
	m_ret    = 1;
	m_device = NULL;
	m_reader = NULL;
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

QXmlStreamReader::Error LibXml2StreamReader::error () const
{

}

QString LibXml2StreamReader::errorString () const
{

}

bool LibXml2StreamReader::hasError () const
{

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
	return xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_END_ELEMENT;
}

qint64 LibXml2StreamReader::columnNumber () const
{
	return xmlTextReaderGetParserColumnNumber(d->m_reader);
}

qint64 LibXml2StreamReader::lineNumber () const
{
	return xmlTextReaderGetParserLineNumber(d->m_reader);
}

QIODevice *	LibXml2StreamReader::device () const
{
	return d->m_device;
}

void LibXml2StreamReader::setDevice ( QIODevice * device )
{
	if (d->m_device == device)
	{
		if (d->m_device)
		{
			xmlFreeTextReader(d->m_reader);
		}

		d->m_device = device;
		d->m_reader = xmlReaderForIO(libxml2read, libxml2close, d, NULL, NULL, 0);
	}
}

void LibXml2StreamReader::raiseError ( const QString & message )
{

}

QString LibXml2StreamReader::readElementText ()
{
	xmlChar * value = xmlTextReaderReadString(d->m_reader);
	QString returnedValue = QString::fromUtf8((char*)value);
	xmlFree(value);

	return returnedValue;
}

void LibXml2StreamReader::skipCurrentElement ()
{
	Q_ASSERT(xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_ELEMENT);

	if (!xmlTextReaderIsEmptyElement(d->m_reader))
	{
		int startLevel, level;
		startLevel = level = xmlTextReaderDepth (d->m_reader);

		int ret = xmlTextReaderRead(d->m_reader);
		while (ret == 1)
		{
			if (xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_END_ELEMENT)
			{
				if(startLevel == level)
				{
					break;
				}
				else
				{
					level--;
				}
			}

			if (xmlTextReaderNodeType(d->m_reader) == XML_READER_TYPE_ELEMENT)
			{
				if (!xmlTextReaderIsEmptyElement(d->m_reader))
					level++;
			}

			ret = xmlTextReaderRead(d->m_reader);
		}
	}
}

bool LibXml2StreamReader::atEnd () const
{
	return ! (d->m_ret == 1);
}

void LibXml2StreamReader::readNext ()
{
	d->m_ret = xmlTextReaderRead(d->m_reader);
}

QString LibXml2StreamReader::name () const
{
	return QString::fromUtf8((char*)xmlTextReaderConstName(d->m_reader));
}

QString LibXml2StreamReader::prefix () const
{
	return QString::fromUtf8((char*)xmlTextReaderConstPrefix(d->m_reader));
}

QString LibXml2StreamReader::text () const
{
	xmlChar * value = xmlTextReaderReadString(d->m_reader);
	QString returnedValue = QString::fromUtf8((char*)value);
	xmlFree(value);

	return returnedValue;
}

