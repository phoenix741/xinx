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

// Xinx header
#include "js/jscontentviewparser.h"
#include <contentview/contentviewnode.h>

// Qt header
#include <QIODevice>
#include <QTextCodec>
#include <QIcon>
#include <QVariant>

/* JsContentViewParser */

JsContentViewParser::JsContentViewParser( bool autoDelete ) : ContentViewParser( autoDelete ) {
}

JsContentViewParser::~JsContentViewParser() {
}

void JsContentViewParser::loadFromDeviceImpl() {
	rootNode()->setData( ":/images/typejs.png", ContentViewNode::NODE_ICON );

	loadAttachedNode( rootNode() );

	m_line = 1;

	enum JAVASCRIPT_TOKEN type;
	QString name;

	ContentViewNode * function = rootNode();
	int bloc = 0;

	do {
		nextIdentifier( inputDevice(), type, name );
		switch( type ) {
		case TOKEN_IDENTIFIER:
			if( name == "var" ) { // variable
				if( bloc == 0 )
					loadVariables( function, inputDevice() );
				else if( function )
					loadVariables( function, inputDevice() );
				else
					throw ContentViewException( tr("Can't attach variable to function ?"), m_line );
			} else
				if( name == "function" ) {
					function = loadFunction( rootNode(), inputDevice() );
					loadAttachedNode( function );
				} else
					do {
						nextIdentifier( inputDevice(), type, name );
						if( type == TOKEN_EOF )
							throw ContentViewException( tr("End of file is prematured"), m_line );
					} while( ( type != TOKEN_PONCTUATION ) || ( ( name != ";" ) && ( name != "{" ) && ( name != "}" ) ) );

			if( ( type == TOKEN_PONCTUATION ) && ( name == "{" ) ) bloc ++;
			if( ( type == TOKEN_PONCTUATION ) && ( name == "}" ) ) {
				if( ( bloc == 1 ) && function )
					function = rootNode();
				bloc --;
			}
			break;
		case TOKEN_PONCTUATION:
			if( name == "{" ) bloc++;
			if( name == "}" ) {
				if( ( bloc == 1 ) && function )
					function = rootNode();
				bloc --;
			}
			if( bloc < 0 )
				throw ContentViewException( tr("Too many '}'"), m_line );
		case TOKEN_EOF:
			break;
		default:
			throw ContentViewException( tr("I wait something but i don't know what !"), m_line );
		}
	} while( ! inputDevice()->atEnd() );

	detachAttachedNode();
}

void JsContentViewParser::nextIdentifier( QIODevice * device, enum JAVASCRIPT_TOKEN & symbType, QString & symbName ) {
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

void JsContentViewParser::loadInstruction( QIODevice * buffer, QString & name, JAVASCRIPT_TOKEN & type ) {
	// Compter les parantheses. Aller jusqu'au point virgule. Si on trouve un identifier suivis de paranthese
	// alors appel (1er = identifier).sinon constante, operation, ...

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
		if( type == TOKEN_EOF )
			throw ContentViewException( tr("End of file is prematured"), m_line );
	};
}


void JsContentViewParser::loadVariables( ContentViewNode * parent, QIODevice * buffer ) {
	enum JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw ContentViewException( tr("I wait an identifier"), m_line );

	attacheNewVariableNode( parent, name, m_line );
	//variables << new JavaScriptVariables( this, name, m_line );
	bool cont = true, loadIdentifier = true;
	do {
		if( loadIdentifier )
			nextIdentifier( buffer, type, name );

		loadIdentifier = true;

		if( type == TOKEN_EOF )
			throw ContentViewException( tr("End of file is prematured"), m_line );

		if( ( type == TOKEN_PONCTUATION ) && ( name == ";" ) )
			cont = false;
		else if( ( type == TOKEN_PONCTUATION ) && ( name == "," ) ) {
			nextIdentifier( buffer, type, name );

			if( type != TOKEN_IDENTIFIER )
				throw ContentViewException( tr("I wait an identifier."), m_line );

			attacheNewVariableNode( parent, name, m_line );
		} else if ( ( type == TOKEN_PONCTUATION ) && ( name == "=" ) ) {
			loadInstruction( buffer, name, type );
			loadIdentifier = false;
		}
	} while( cont );
}

ContentViewNode * JsContentViewParser::loadFunction( ContentViewNode * parent, QIODevice * buffer ) {
	enum JAVASCRIPT_TOKEN type;
	QString name;

	nextIdentifier( buffer, type, name );
	if( type != TOKEN_IDENTIFIER )
		throw ContentViewException( tr("I wait an identifier."), m_line );

	ContentViewNode * function = attacheNewFunctionNode( parent, name, m_line );

	nextIdentifier( buffer, type, name );

	if( ! ( ( type == TOKEN_PONCTUATION ) && ( name == "(" ) ) )
		throw ContentViewException( tr("I wait a '('"), m_line );

	do {
		nextIdentifier( buffer, type, name );
		if( type == TOKEN_EOF )
			throw ContentViewException( tr("End of file is prematured"), m_line );

		if( type == TOKEN_IDENTIFIER )
			attacheNewParamNode( function, name, m_line );

		while( ( type != TOKEN_PONCTUATION ) || ( ( name != ")" ) && ( name != "," ) ) ) {
			nextIdentifier( buffer, type, name );
			if( type == TOKEN_EOF )
				throw ContentViewException( tr("End of file is prematured"), m_line );
		}
	} while( ( type != TOKEN_PONCTUATION ) || ( name != ")" ) );

	return function;
}

ContentViewNode * JsContentViewParser::attacheNewParamNode( ContentViewNode * parent, const QString & name, int line ) {
	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "JsParam", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/html_value.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1" ).arg( line ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}

ContentViewNode * JsContentViewParser::attacheNewVariableNode( ContentViewNode * parent, const QString & name, int line ) {
	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "JsVariable", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/variable.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1" ).arg( line ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}

ContentViewNode * JsContentViewParser::attacheNewFunctionNode( ContentViewNode * parent, const QString & name, int line ) {
	ContentViewNode * node = new ContentViewNode( name, line );
	node->setData( "JsFunction", ContentViewNode::NODE_TYPE );
	node->setData( ":/images/noeud.png", ContentViewNode::NODE_ICON );
	node->setData( name, ContentViewNode::NODE_DISPLAY_NAME );
	node->setData( tr( "Element at line : %1" ).arg( line ), ContentViewNode::NODE_DISPLAY_TIPS );
	node = attachNode( parent, node );

	return node;
}
