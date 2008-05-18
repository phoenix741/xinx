/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef XMLPRETTYPRINTER_H_
#define XMLPRETTYPRINTER_H_

// Xinx header
#include <exceptions.h>

// Qt header
#include <QString>
#include <QXmlStreamReader>
#include <QBuffer>
#include <QTextStream>

class XMLPrettyPrinterException {
public:
	XMLPrettyPrinterException( const QString & message, int line, int column ) : m_message( message ), m_line( line ), m_column( column ) {}
	QString m_message;
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
