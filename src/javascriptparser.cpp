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

#include "javascriptparser.h"

#define JAVASCRIPT_IDENTIFIER "\\w"

class PrivateJavaScriptFunction {
public:
	PrivateJavaScriptFunction( JavaScriptFunction * parent );
	virtual ~PrivateJavaScriptFunction();
	
	QStringList m_params, m_variables;
private:
	JavaScriptFunction * m_parent;
};

PrivateJavaScriptFunction::PrivateJavaScriptFunction( JavaScriptFunction * parent ) {
	m_parent =parent;
}

PrivateJavaScriptFunction::~PrivateJavaScriptFunction() {
	
}

/* JavaScriptFunction */

JavaScriptFunction::JavaScriptFunction() {
	d = new PrivateJavaScriptFunction( this );
}

JavaScriptFunction::~JavaScriptFunction() {
	delete d;
}

const QStringList & JavaScriptFunction::params() {
	return d->m_params;
}

const QStringList & JavaScriptFunction::variables() {
	return d->m_variables;
}

/* PrivateJavaScriptParser */

class PrivateJavaScriptParser {
public:
	enum JAVASCRIPT_TOKEN { TOKEN_UNKOWN, TOKEN_IDENTIFIER, TOKEN_CHARACTER, TOKEN_STRING, TOKEN_NUMBER };

	PrivateJavaScriptParser( JavaScriptParser * parent );
	virtual ~PrivateJavaScriptParser();
	
	QStringList m_variables;
	QList<JavaScriptFunction> m_functions;
	
	QString m_content;
	int m_position;
	
	enum JAVASCRIPT_TOKEN nextIdentifier( QString & identifier );
private:
	JavaScriptParser * m_parent;
};

PrivateJavaScriptParser::PrivateJavaScriptParser( JavaScriptParser * parent ) {
	m_parent = parent;
}

PrivateJavaScriptParser::~PrivateJavaScriptParser() {
	
}

enum JAVASCRIPT_TOKEN PrivateJavaScriptParser::nextIdentifier( QString & identifier ) {
	enum JAVASCRIPT_TOKEN token = TOKEN_UNKOWN;
	identifier = "";
	int index = m_position;
	
	char c = m_content.at( index );
	if( ( c >= 'A' ) || ( c <= 'Z' ) ) {
		
	}
	
	return token;
}

/* JavaScriptParser */

JavaScriptParser::JavaScriptParser() {
	d = new PrivateJavaScriptParser( this );
}

JavaScriptParser::~JavaScriptParser() {
	delete d;
}

void JavaScriptParser::load( const QString & content ) {
	int pos = 0, index;
	do {
		QRegExp expression( JAVASCRIPT_IDENTIFIER );
		index = expression.indexIn( content, pos );
		if( index != -1 ) {
			QString identifier = expression.cap( 1 );
			pos += expression.matchedLength();
			if( identifier == "var" ) {
				
			} else if( identifier == "function" ) {
				
			}
		}
	} while( index < content.length() );
}

const QStringList & JavaScriptParser::variables() {
	return m_variables;
}

const QList<JavaScriptFunction> & JavaScriptParser::functions() {
	return m_functions;
}

