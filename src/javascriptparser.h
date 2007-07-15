/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef __JAVASCRIPTPARSER_H__
#define __JAVASCRIPTPARSER_H__

#include <QStringList>
#include <QList>

class PrivateJavaScriptFunction;
class PrivateJavaScriptParser;

class JavaScriptParserException {
public:
	JavaScriptParserException( int line );
};

class JavaScriptFunction {
public:
	JavaScriptFunction();
	virtual ~JavaScriptFunction();

	const QString & name();
	const QStringList & params();
	const QStringList & variables();
	int line();
private:
	PrivateJavaScriptFunction * d;
	friend class PrivateJavaScriptFunction;
	friend class PrivateJavaScriptParser;
	friend class JavaScriptParser;
};

class JavaScriptParser {
public:
	JavaScriptParser();
	virtual ~JavaScriptParser();
	
	void load( const QString & content, const QString & filename );

	const QStringList & variables();
	const QList<JavaScriptFunction*> & functions();
	const QString & filename();
private:
	PrivateJavaScriptParser * d;
	friend class PrivateJavaScriptParser;
	friend class PrivateJavaScriptFunction;
	friend class JavaScriptFunction;
};

#endif // __JAVASCRIPTPARSER_H__
