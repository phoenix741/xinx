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

#ifndef LIBXML2STREAMREADER_H
#define LIBXML2STREAMREADER_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QIODevice>
#include <QXmlStreamReader>

class PrivateLibXml2StreamReader;

class LIBEXPORT LibXml2StreamReader
{
public:
	LibXml2StreamReader();
	LibXml2StreamReader(QIODevice * device);
	~LibXml2StreamReader();

	QString attribute(const QString & name);

	QString documentEncoding () const;
	QString documentVersion () const;

	QString errorString () const;
	bool hasError () const;

	bool isStartElement () const;
	bool isCharacters () const;
	bool isCDATA () const;
	bool isEntityReference () const;
	bool isEntity () const;
	bool isProcessingInstruction () const;
	bool isComment () const;
	bool isStartDocument () const;
	bool isDocumentType () const;
	bool isDocumentFragment () const;
	bool isWhitespace () const;
	bool isEndElement () const;

	qint64	columnNumber () const;
	qint64 lineNumber () const;

	QIODevice *	device () const;
	void setDevice ( QIODevice * device );

	void raiseError ( const QString & message = QString() );

	QString readElementText ();
	void skipCurrentElement ();
	bool atEnd () const;
	void readNext ();

	QString name () const;
	QString prefix () const;
	QString text () const;

	void setNamespaceProcessing (bool value);
	bool namespaceProcessing() const;

	bool validate(const QString & uri);
private:
	PrivateLibXml2StreamReader * d;
};

#endif // LIBXML2STREAMREADER_H
