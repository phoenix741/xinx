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

#ifndef XMLPRETTYPRINTER_H_
#define XMLPRETTYPRINTER_H_

// Xinx header
#include <core/exceptions.h>

// Qt header
#include <QString>
#include <QXmlStreamReader>
#include <QBuffer>
#include <QTextStream>

class XMLPrettyPrinterException : public XinxException {
public:
	XMLPrettyPrinterException( const QString & message, int line, int column ) : XinxException( message ), m_line( line ), m_column( column ) {}
	int m_line, m_column;
};

class XMLPrettyPrinter : public QXmlStreamReader {
public:
	XMLPrettyPrinter();
	virtual ~XMLPrettyPrinter();

	void setText( const QString & text );
	void process();
	QString getResult();
private:
	void writeLevel( int level );
	void constructXML( int level = 0 );

	QString m_text;
	QBuffer m_resultBuffer;
	QTextStream m_result;
};

#endif /*XMLPRETTYPRINTER_H_*/
