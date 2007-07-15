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

#include <QBuffer>
#include <QDebug>

/* 	JavaScriptParserException */

JavaScriptParserException::JavaScriptParserException( int line ) {
	qDebug() << QObject::tr("Error at line %1").arg( line );
}

/* PrivateJavaScriptFunction */

class PrivateJavaScriptFunction {
public:
	PrivateJavaScriptFunction( JavaScriptFunction * parent );
	virtual ~PrivateJavaScriptFunction();
	
	QStringList m_params, m_variables;
	QString m_name;
	
	int m_line;
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

const QString & JavaScriptFunction::name() {
	return d->m_name;
}

int JavaScriptFunction::line() {
	return d->m_line;
}


/* PrivateJavaScriptParser */

class PrivateJavaScriptParser {
public:
	enum JAVASCRIPT_TOKEN { TOKEN_UNKNOWN, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_PONCTUATION, TOKEN_EOF };

	PrivateJavaScriptParser( JavaScriptParser * parent );
	virtual ~PrivateJavaScriptParser();
	
	QStringList m_variables;
	QList<JavaScriptFunction*> m_functions;
	
	int m_line;
	QString m_filename;
		
	void nextIdentifier( QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName );
	QStringList loadVariables( QIODevice * device );
	JavaScriptFunction * loadFunction( QIODevice * device );
private:
	JavaScriptParser * m_parent;
};

PrivateJavaScriptParser::PrivateJavaScriptParser( JavaScriptParser * parent ) {
	m_parent = parent;
}

PrivateJavaScriptParser::~PrivateJavaScriptParser() {
	qDeleteAll( m_functions );
}

void PrivateJavaScriptParser::nextIdentifier( QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName ) {
	char ch, c;
	QString st;
	enum { STATE_START, STATE_IDENTIFIER, STATE_STRING1, STATE_STRING2, STATE_NUMBER, STATE_STARTCOMMENT, STATE_COMMENT1, STATE_COMMENT2, STATE_EOCOMMENT1, STATE_END } state;
	
	state = STATE_START;
	st    = "";
	
	do {
		if( ! device->getChar( &ch ) )  {
			symbName = "";
			symbType = TOKEN_EOF;
			state = STATE_END;	
		}
		c = QChar( ch ).toUpper().toAscii();
		switch( state ) {
		case STATE_START:
			if( ( c == ' ' ) || ( c == 13 ) || ( c == 9 ) ) 
				; // Skip space
			else if( c == 10 ) 
				m_line++; 
			else if( ( c == '{' ) || ( c == '}' ) || ( c == '&' ) || ( c == '|' ) || ( c == '*' ) || 
			         ( c == ';' ) || ( c == '=' ) || ( c == '(' ) || ( c == ')' ) ) {
				symbType = TOKEN_PONCTUATION;
				st = c;
				symbName = st;
				state = STATE_END;
		    } else if( c == '/' ) {
		    	state = STATE_STARTCOMMENT;
	    	} else if( ( c >= 'A' ) && ( c <= 'Z' ) ) {
	    		state = STATE_IDENTIFIER;
	    		st = ch;
    		} else if( ( c == '+' ) || ( c == '-' ) || ( ( c >= '0' ) && ( c <= '9' ) ) ) {
    			st = c;
    			state = STATE_NUMBER;
   			} else if( c == '\'' ) {
   				state = STATE_STRING1;
   			} else if( c == '\'' ) {
   				state = STATE_STRING2;
  			} else {
  				st = c;	
  				symbName = st;
  				symbType = TOKEN_UNKNOWN;
  				state = STATE_END;
 			}			
			break;
		case STATE_STARTCOMMENT:
			if( c == '*' ) 
				state = STATE_COMMENT1;
			else if( c == '/' )
				state = STATE_COMMENT2;
			else {
				device->ungetChar( ch );
				state = STATE_START;
			}
			break;
		case STATE_COMMENT1:
			if( c == '*' ) 
				state = STATE_EOCOMMENT1;
			else if( c == 13 )
				m_line++;
			break;
		case STATE_EOCOMMENT1:
			if( c == '/' ) 
				state = STATE_START;
			else {
				state = STATE_COMMENT1;
				if( c == 13 )
					m_line++;				
			}
			break;
		case STATE_COMMENT2:
			if( c == 13 ) {
				state = STATE_START;
				m_line++;				
			}
			break;
		case STATE_IDENTIFIER:
			if( ( ( c >= 'A' ) && ( c <= 'Z' ) ) || ( ( c >= '0' ) && ( c <= '9' ) ) || ( c == '_' ) )
				st += ch;
			else {
				symbName = st;
				symbType = TOKEN_IDENTIFIER;
				state = STATE_END;
				device->ungetChar( ch );
			}
			break;
		case STATE_NUMBER:
			if( ( ( c >= '0' ) && ( c <= '9' ) ) || ( c == '+' ) || ( c == '-' ) )
				st += c;
			else {
				symbName = st;
				symbType = TOKEN_NUMBER;
				state = STATE_END;
				device->ungetChar( ch );
			}
			break;
		case STATE_STRING1:
			if( c == '\'' ) {
				symbName = st;
				symbType = TOKEN_STRING;
				state = STATE_END;
			} else if ( c == '\\' ) {
				if( ! device->getChar( &ch ) ) state = STATE_END; else {
					st += ch;
				}
			} else
				st += ch;
			break;
		case STATE_STRING2:
			if( c == '"' ) {
				symbName = st;
				symbType = TOKEN_STRING;
				state = STATE_END;
			} else if ( c == '\\' ) {
				if( ! device->getChar( &ch ) ) state = STATE_END; else {
					st += ch;
				}
			} else
				st += ch;
			break;
		case STATE_END:
			;
		}
	} while( state != STATE_END );
}

QStringList PrivateJavaScriptParser::loadVariables( QIODevice * buffer ) {
	QStringList variables;
	enum PrivateJavaScriptParser::JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw JavaScriptParserException( m_line );

	variables << name;
	bool cont = true;
	do {
		nextIdentifier( buffer, type, name );
		if( ( type == TOKEN_PONCTUATION ) && ( name == ";" ) ) 
			cont = false;
		else if( ( type == TOKEN_PONCTUATION ) && ( name == "," ) ) {
			nextIdentifier( buffer, type, name );
			if( type != TOKEN_IDENTIFIER )
				throw JavaScriptParserException( m_line );
			variables << name;
		}
	} while( cont );
	return variables;
}

JavaScriptFunction * PrivateJavaScriptParser::loadFunction( QIODevice * buffer ) {
	JavaScriptFunction * function = new JavaScriptFunction();
	
	enum PrivateJavaScriptParser::JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw JavaScriptParserException( m_line );
	function->d->m_name = name;
	function->d->m_line = m_line;
	
	nextIdentifier( buffer, type, name );
	if( ! ( ( type == TOKEN_PONCTUATION ) && ( name == "(" ) ) ) 
		throw JavaScriptParserException( m_line );
	
	do {		
		nextIdentifier( buffer, type, name );
		if( type == TOKEN_IDENTIFIER ) 
			function->d->m_params << name;
		
		do {
			nextIdentifier( buffer, type, name );
		} while( ( type != TOKEN_PONCTUATION ) || ( ( name != ")" ) && ( name != "," ) ) );
	} while( ( type != TOKEN_PONCTUATION ) || ( name != ")" ) );

	return function;	
}


/* JavaScriptParser */

JavaScriptParser::JavaScriptParser() {
	d = new PrivateJavaScriptParser( this );
}

JavaScriptParser::~JavaScriptParser() {
	delete d;
}

void JavaScriptParser::load( const QString & content, const QString & filename ) {
	d->m_variables.clear();
	qDeleteAll( d->m_functions );
	d->m_functions.clear();
	
	d->m_filename = filename;
	
	enum PrivateJavaScriptParser::JAVASCRIPT_TOKEN type;
	QString name;

	QByteArray array = content.toAscii();
	QBuffer buffer ( &array );
	buffer.open( QIODevice::ReadWrite | QIODevice::Text );	
	buffer.seek( 0 );
	
	JavaScriptFunction * function;
	int bloc = 0;
	
	do {
		d->nextIdentifier( &buffer, type, name );
		switch( type ) {
		case PrivateJavaScriptParser::TOKEN_IDENTIFIER:
			if( name == "var" ) { // variable
				if( bloc == 0 )
					d->m_variables << d->loadVariables( &buffer );
				else 
					function->d->m_variables << d->loadVariables( &buffer );
			} else
			if( name == "function" ) {
				function = d->loadFunction( &buffer );
				d->m_functions << function;
			} else
			do {
				d->nextIdentifier( &buffer, type, name );
			} while( ( type != PrivateJavaScriptParser::TOKEN_PONCTUATION ) || ( ( name != ";" ) && ( name != "{" ) ) );
			if( ( type == PrivateJavaScriptParser::TOKEN_PONCTUATION ) && ( name == "{" ) ) bloc ++;
			break;
		case PrivateJavaScriptParser::TOKEN_PONCTUATION:
			if( name == "{" )
				bloc++;
			if( name == "}" )
				bloc--;
			if( bloc < 0 ) 
				throw JavaScriptParserException( d->m_line );
		case PrivateJavaScriptParser::TOKEN_EOF:
			break;
		default:
			throw JavaScriptParserException( d->m_line );
		}
	} while( ! buffer.atEnd() );
}
	
const QStringList & JavaScriptParser::variables() {
	return d->m_variables;
}

const QList<JavaScriptFunction*> & JavaScriptParser::functions() {
	return d->m_functions;
}

const QString & JavaScriptParser::filename() {
	return d->m_filename;
}
