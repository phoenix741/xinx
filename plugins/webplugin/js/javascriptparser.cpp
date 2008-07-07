/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

// Xinx header
#include "javascriptparser.h"

// Qt header
#include <QFileInfo>
#include <QFile>
#include <QBuffer>
#include <QTextStream>

/* 	JavaScriptParserException */

JavaScriptParserException::JavaScriptParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* JavaScriptParams */

JavaScriptParams::JavaScriptParams( FileContentElement * parent, const QString & name, int line ) : FileContentElement( parent, name, line ) {

}

QIcon JavaScriptParams::icon() const {
	return QIcon( ":/images/html_value.png" );
}

/* JavaScriptVariables */

JavaScriptVariables::JavaScriptVariables( FileContentElement * parent, const QString & name, int line ) : JavaScriptParams( parent, name, line ) {

}

QIcon JavaScriptVariables::icon() const {
	return QIcon( ":/images/variable.png" );
}

/* JavaScriptFunction */

JavaScriptFunction::JavaScriptFunction( FileContentElement * parent, const QString & name, int line ) : FileContentElement( parent, name, line ) {

}

QIcon JavaScriptFunction::icon() const {
	return QIcon( ":/images/noeud.png" );
}

/* JavaScriptParser */

JavaScriptParser::JavaScriptParser() : FileContentElement( NULL, QString(), -1 ), m_isLoaded( true ) {
	m_line = 1;
}

JavaScriptParser::JavaScriptParser( FileContentElement * parent, const QString & filename, int lineNumber ) : FileContentElement( parent, QFileInfo( filename ).fileName(), lineNumber ), m_isLoaded( true ) {
	m_line = 1;
}

JavaScriptParser::~JavaScriptParser() {
}

QIcon JavaScriptParser::icon() const {
	return QIcon( ":/images/typejs.png" );
}

void JavaScriptParser::loadFromFileDelayed( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );

	m_isLoaded = false;
}

bool JavaScriptParser::isLoaded() {
	return m_isLoaded;
}

void JavaScriptParser::loadFromDevice( QIODevice * device ) {
	m_isLoaded = true;

	markAllDeleted();

	m_line = 1;

	enum JAVASCRIPT_TOKEN type;
	QString name;

	// Remove because double opening
	//device->open( QIODevice::ReadWrite | QIODevice::Text );
	//device->seek( 0 );

	JavaScriptFunction * function = NULL;
	int bloc = 0;

	do {
		nextIdentifier( device, type, name );
		switch( type ) {
		case TOKEN_IDENTIFIER:
			if( name == "var" ) { // variable
				if( bloc == 0 )
					foreach( JavaScriptVariables * variable, loadVariables( device ) )
						append( variable );
				else if( function )
					foreach( JavaScriptVariables * variable, loadVariables( device ) )
						function->append( variable );
				else
					throw JavaScriptParserException( QObject::tr("Can't attach variable to function ?"), m_line );
			} else
			if( name == "function" ) {
				QList<JavaScriptParams*> params;
				JavaScriptFunction * newFunction = loadFunction( device, &params );
				function = static_cast<JavaScriptFunction*>( append( newFunction ) );
				function->markAllDeleted();
				foreach( JavaScriptParams * param, params ) {
					function->append( param );
				}
			} else
			do {
				nextIdentifier( device, type, name );
				if( type == TOKEN_EOF )
					throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
			} while( ( type != TOKEN_PONCTUATION ) || ( ( name != ";" ) && ( name != "{" ) && ( name != "}" ) ) );

			if( ( type == TOKEN_PONCTUATION ) && ( name == "{" ) ) bloc ++;
			if( ( type == TOKEN_PONCTUATION ) && ( name == "}" ) ) {
				if( ( bloc == 1 ) && function )
					function->removeMarkedDeleted();
				bloc --;
			}
			break;
		case TOKEN_PONCTUATION:
			if( name == "{" )
				bloc++;
			if( name == "}" ) {
				if( ( bloc == 1 ) && function )
					function->removeMarkedDeleted();
				bloc--;
			}
			if( bloc < 0 )
				throw JavaScriptParserException( QObject::tr("Too many '}'"), m_line );
		case TOKEN_EOF:
			break;
		default:
			throw JavaScriptParserException( QObject::tr("I wait something but i don't know what !"), m_line );
		}
	} while( ! device->atEnd() );

	removeMarkedDeleted();
}

void JavaScriptParser::loadFromFile( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );

	FileContentParser::loadFromFile( filename );
}

void JavaScriptParser::nextIdentifier( QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName ) {
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
			         ( c == ';' ) || ( c == '=' ) || ( c == '(' ) || ( c == ')' ) || ( c == ',' ) ||
			         ( c == '[' ) || ( c == ']' ) ) {
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

void JavaScriptParser::loadInstruction( QIODevice * buffer, JavaScriptFunction * function, QString & name, JAVASCRIPT_TOKEN & type ) {
	Q_UNUSED( function );
	// Compter les parantheses. Aller jusqu'au point virgule.
	// Si Identifier suivis de paranthese alors appel (1er = identifier).
	// Sinon constante, operation, ...

	int bloc = 0, crochet = 0;

	while( ( bloc > 0 ) || ( crochet > 0 ) || ( TOKEN_PONCTUATION != type ) || ( ( name != "," ) && ( name != ";" ) ) ) {
		if( ( TOKEN_PONCTUATION == type ) && ( name == "(" ) )
			bloc++;
		else
		if( ( TOKEN_PONCTUATION == type ) && ( name == ")" ) )
			bloc--;
		else
		if( ( TOKEN_PONCTUATION == type ) && ( name == "[" ) )
			crochet++;
		else
		if( ( TOKEN_PONCTUATION == type ) && ( name == "]" ) )
			crochet--;
		nextIdentifier( buffer, type, name );
		if( type == TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
	};
}


QList<JavaScriptVariables*> JavaScriptParser::loadVariables( QIODevice * buffer ) {
	QList<JavaScriptVariables*> variables;
	enum JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw JavaScriptParserException( QObject::tr("I wait an identifier"), m_line );

	variables << new JavaScriptVariables( this, name, m_line );
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
			variables << new JavaScriptVariables( this, name, m_line );
		} else if ( ( type == TOKEN_PONCTUATION ) && ( name == "=" ) ) {
			loadInstruction( buffer, NULL, name, type );
			loadIdentifier = false;
		}
	} while( cont );
	return variables;
}

JavaScriptFunction * JavaScriptParser::loadFunction( QIODevice * buffer, QList<JavaScriptParams*> * params ) {
	enum JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw JavaScriptParserException( QObject::tr("I wait an identifier."), m_line );

	JavaScriptFunction * function = new JavaScriptFunction( this, name, m_line );

	nextIdentifier( buffer, type, name );
	if( ! ( ( type == TOKEN_PONCTUATION ) && ( name == "(" ) ) )
		throw JavaScriptParserException( QObject::tr("I wait a '('"), m_line );

	do {
		nextIdentifier( buffer, type, name );
		if( type == TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
		if( type == TOKEN_IDENTIFIER )
			params->append( new JavaScriptParams( function, name, m_line ) );

		while( ( type != TOKEN_PONCTUATION ) || ( ( name != ")" ) && ( name != "," ) ) ) {
			nextIdentifier( buffer, type, name );
			if( type == TOKEN_EOF ) throw JavaScriptParserException( QObject::tr("End of file is prematured"), m_line );
		}
	} while( ( type != TOKEN_PONCTUATION ) || ( name != ")" ) );

	return function;
}

int JavaScriptParser::rowCount() {
	if( ! m_isLoaded )
		try {
			loadFromFile( filename() );
		} catch( FileContentException e ) {
		}
	return FileContentElement::rowCount();
}
