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

JavaScriptParserException::JavaScriptParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* JavaScriptElement */

JavaScriptElement::JavaScriptElement( JavaScriptElement * parent, const QString & name, int line ) : FileContentElement( parent, name, line ) {
	
}

/* JavaScriptParams */

JavaScriptParams::JavaScriptParams( JavaScriptElement * parent, const QString & name, int line ) : JavaScriptElement( parent, name, line ) {
	
}

/* JavaScriptVariables */

JavaScriptVariables::JavaScriptVariables( JavaScriptElement * parent, const QString & name, int line ) : JavaScriptParams( parent, name, line ) {
	
}

/* PrivateJavaScriptFunction */

class PrivateJavaScriptFunction {
public:
	PrivateJavaScriptFunction( JavaScriptFunction * parent );
	virtual ~PrivateJavaScriptFunction();
	
	QList<JavaScriptParams*> m_params;
	QList<JavaScriptVariables*> m_variables;
private:
	JavaScriptFunction * m_parent;
};

PrivateJavaScriptFunction::PrivateJavaScriptFunction( JavaScriptFunction * parent ) {
	m_parent =parent;
}

PrivateJavaScriptFunction::~PrivateJavaScriptFunction() {
	
}

/* JavaScriptFunction */

JavaScriptFunction::JavaScriptFunction( JavaScriptElement * parent, const QString & name, int line ) : JavaScriptElement( parent, name, line ) {
	d = new PrivateJavaScriptFunction( this );
}

JavaScriptFunction::~JavaScriptFunction() {
	qDeleteAll( d->m_params );
	qDeleteAll( d->m_variables );
	delete d;
}

const QList<JavaScriptParams*> & JavaScriptFunction::params() {
	return d->m_params;
}

const QList<JavaScriptVariables*> & JavaScriptFunction::variables() {
	return d->m_variables;
}

int JavaScriptFunction::rowCount() {
	return d->m_params.size() + d->m_variables.size();
}

FileContentElement * JavaScriptFunction::element( int index ) {
	if( index < d->m_params.size() ) 
		return d->m_params.at( index );
	else
	if( ( index - d->m_params.size() ) < d->m_variables.size() ) 
		return d->m_variables.at( index - d->m_params.size() );
	else
		return NULL;
}

/* PrivateJavaScriptParser */

class PrivateJavaScriptParser {
public:
	enum JAVASCRIPT_TOKEN { TOKEN_UNKNOWN, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, TOKEN_PONCTUATION, TOKEN_EOF };

	PrivateJavaScriptParser( JavaScriptParser * parent );
	virtual ~PrivateJavaScriptParser();
	
	QList<JavaScriptVariables*> m_variables;
	QList<JavaScriptFunction*> m_functions;
	
	int m_line;
		
	void nextIdentifier( QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName );
	QList<JavaScriptVariables*> loadVariables( QIODevice * device );
	JavaScriptFunction * loadFunction( QIODevice * device );
	void loadInstruction( QIODevice * buffer, JavaScriptFunction * function, QString & name, PrivateJavaScriptParser::JAVASCRIPT_TOKEN & type );
private:
	JavaScriptParser * m_parent;
};

PrivateJavaScriptParser::PrivateJavaScriptParser( JavaScriptParser * parent ) {
	m_parent = parent;
	m_line = 1;
}

PrivateJavaScriptParser::~PrivateJavaScriptParser() {
	qDeleteAll( m_functions );
	qDeleteAll( m_variables );
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
			         ( c == ';' ) || ( c == '=' ) || ( c == '(' ) || ( c == ')' ) || ( c == ',' ) ) {
				symbType = TOKEN_PONCTUATION;
				st = c;
				symbName = st;
				state = STATE_END;
		    } else if( c == '/' ) {
		    	state = STATE_STARTCOMMENT;
	    	} else if( ( ( c >= 'A' ) && ( c <= 'Z' ) ) || ( c == '_' ) ) {
	    		state = STATE_IDENTIFIER;
	    		st = ch;
    		} else if( ( c == '+' ) || ( c == '-' ) || ( ( c >= '0' ) && ( c <= '9' ) ) ) {
    			st = c;
    			state = STATE_NUMBER;
   			} else if( c == '\'' ) {
   				state = STATE_STRING1;
   			} else if( c == '"' ) {
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
  				symbName = "/";
  				symbType = TOKEN_UNKNOWN;
  				state = STATE_END;
			}
			break;
		case STATE_COMMENT1:
			if( c == '*' ) 
				state = STATE_EOCOMMENT1;
			else if( c == 10 )
				m_line++;
			break;
		case STATE_EOCOMMENT1:
			if( c == '/' ) 
				state = STATE_START;
			else if ( c == '*' ) 
				state = STATE_EOCOMMENT1;
			else {
				state = STATE_COMMENT1;
				if( c == 10 )
					m_line++;				
			}
			break;
		case STATE_COMMENT2:
			if( c == 10 ) {
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

void PrivateJavaScriptParser::loadInstruction( QIODevice * buffer, JavaScriptFunction * function, QString & name, PrivateJavaScriptParser::JAVASCRIPT_TOKEN & type ) {
	Q_UNUSED( function );
	// Compter les paranthèses. Aller jusqu'au point virgule.
	// Si Identifier suivis de paranthèse alors appel (1er = identifier).
	// Sinon constante, opération, ...
	
	int bloc = 0;
	
	while( ( bloc > 0 ) || ( TOKEN_PONCTUATION != type ) || ( ( name != "," ) && ( name != ";" ) ) ) {
		if( ( TOKEN_PONCTUATION == type ) && ( name == "(" ) )
			bloc++;
		else
		if( ( TOKEN_PONCTUATION == type ) && ( name == ")" ) )
			bloc--;
		nextIdentifier( buffer, type, name );
		if( type == TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
	};
}


QList<JavaScriptVariables*> PrivateJavaScriptParser::loadVariables( QIODevice * buffer ) {
	QList<JavaScriptVariables*> variables;
	enum PrivateJavaScriptParser::JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw JavaScriptParserException( QObject::tr("I wait an identifier"), m_line );

	variables << new JavaScriptVariables( this->m_parent, name, m_line );
	bool cont = true, loadIdentifier = true;
	do {
		if( loadIdentifier )
			nextIdentifier( buffer, type, name );
		loadIdentifier = true;
		if( type == TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
			
		if( ( type == TOKEN_PONCTUATION ) && ( name == ";" ) ) 
			cont = false;
		else if( ( type == TOKEN_PONCTUATION ) && ( name == "," ) ) {
			nextIdentifier( buffer, type, name );
			if( type != TOKEN_IDENTIFIER )
				throw JavaScriptParserException( QObject::tr("I wait an identifier."), m_line );
			variables << new JavaScriptVariables( this->m_parent, name, m_line );
		} else if ( ( type == TOKEN_PONCTUATION ) && ( name == "=" ) ) {
			loadInstruction( buffer, NULL, name, type );
			loadIdentifier = false;
		}
	} while( cont );
	return variables;
}

JavaScriptFunction * PrivateJavaScriptParser::loadFunction( QIODevice * buffer ) {
	enum PrivateJavaScriptParser::JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw JavaScriptParserException( QObject::tr("I wait an identifier."), m_line );

	JavaScriptFunction * function = new JavaScriptFunction( this->m_parent, name, m_line );

	nextIdentifier( buffer, type, name );
	if( ! ( ( type == TOKEN_PONCTUATION ) && ( name == "(" ) ) ) 
		throw JavaScriptParserException( QObject::tr("I wait a '('"), m_line );
	
	do {		
		nextIdentifier( buffer, type, name );
		if( type == TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
		if( type == TOKEN_IDENTIFIER ) 
			function->d->m_params << new JavaScriptParams( this->m_parent, name, m_line );
		
		while( ( type != TOKEN_PONCTUATION ) || ( ( name != ")" ) && ( name != "," ) ) ) {
			nextIdentifier( buffer, type, name );
			if( type == TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
		} 
	} while( ( type != TOKEN_PONCTUATION ) || ( name != ")" ) );

	return function;	
}


/* JavaScriptParser */

JavaScriptParser::JavaScriptParser( const QString & content ) : JavaScriptElement( NULL, QString(), -1 ) {
	d = new PrivateJavaScriptParser( this );
	load( content );
}

JavaScriptParser::~JavaScriptParser() {
	delete d;
}

void JavaScriptParser::load( const QString & content ) {
	qDeleteAll( d->m_variables );
	d->m_variables.clear();
	qDeleteAll( d->m_functions );
	d->m_functions.clear();
	d->m_line = 1;
	
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
				if( type == PrivateJavaScriptParser::TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), d->m_line );
			} while( ( type != PrivateJavaScriptParser::TOKEN_PONCTUATION ) || ( ( name != ";" ) && ( name != "{" ) && ( name != "}" ) ) );
			if( ( type == PrivateJavaScriptParser::TOKEN_PONCTUATION ) && ( name == "{" ) ) bloc ++;
			if( ( type == PrivateJavaScriptParser::TOKEN_PONCTUATION ) && ( name == "}" ) ) bloc --;
			break;
		case PrivateJavaScriptParser::TOKEN_PONCTUATION:
			if( name == "{" )
				bloc++;
			if( name == "}" )
				bloc--;
			if( bloc < 0 ) 
				throw JavaScriptParserException( QObject::tr("Too many '}'"), d->m_line );
		case PrivateJavaScriptParser::TOKEN_EOF:
			break;
		default:
			throw JavaScriptParserException( QObject::tr("I wait something but i don't know what !"), d->m_line );
		}
	} while( ! buffer.atEnd() );
}
	
const QList<JavaScriptVariables*> & JavaScriptParser::variables() {
	return d->m_variables;
}

const QList<JavaScriptFunction*> & JavaScriptParser::functions() {
	return d->m_functions;
}

int JavaScriptParser::rowCount() {
	return d->m_variables.size() + d->m_functions.size();
}

FileContentElement * JavaScriptParser::element( int index ) {
	if( index < d->m_variables.size() ) 
		return d->m_variables.at( index );
	else
	if( ( index - d->m_variables.size() ) < d->m_functions.size() ) 
		return d->m_functions.at( index - d->m_variables.size() );
	else
		return NULL;
}
