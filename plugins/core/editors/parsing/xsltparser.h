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

#ifndef XSLTPARSER_H
#define XSLTPARSER_H
#pragma once

// Qt header
#include <QCoreApplication>
#include <QByteArray>
#include <QString>

class PrivateXsltParser;

class XsltParser {
	Q_DECLARE_TR_FUNCTIONS(XsltParser)
public:
    XsltParser();
	~XsltParser();

	//bool loadStylesheet( const QByteArray & data );
	bool loadStylesheet( const QString & filename );

	bool loadXmlFile( const QByteArray & data );
	bool loadXmlFile( const QString & filename );

	QString getOutput() const;

	bool process();
private:
	PrivateXsltParser * d;
	friend class PrivateXsltParser;
};

#endif // XSLTPARSER_H
