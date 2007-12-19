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

// Xinx header
#include "xmlhighlighter.h"

/* Constante */
static const QString EXPR_COMMENT_BEGIN		= "<!--";
static const QString EXPR_COMMENT_TEXT		= "([^-]|(-(?!->)))*";
static const QString EXPR_COMMENT_END		= "-->";
static const QString EXPR_COMMENT			= EXPR_COMMENT_BEGIN + EXPR_COMMENT_TEXT + EXPR_COMMENT_END;

static const QString EXPR_NAME 				= "([A-Za-z_:]|[^\\x00-\\x7F])([A-Za-z0-9_:.-]|[^\\x00-\\x7F])*";

static const QString EXPR_ATTRIBUTE_VALUE	= "[^<%1{]*[%1{]";
static const QString EXPR_XPATH_VALUE       = "[^<%1}]*[%1}]";

void baseplugin_xml::highlightBlock( const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * interface, const QString& text ) {
	int i = 0;
	int pos = 0;
	int brackets = 0;
	QChar quoteType;
	
	ParsingState state = ( interface->xinxPreviousBlockState() == InElement ? ExpectAttributeOrEndOfElement : NoState );
	
	interface->setXinxCurrentBlockState( NoBlock );

	if( interface->xinxPreviousBlockState() == InComment ) {
		// search for the end of the comment
		QRegExp expression( EXPR_COMMENT_END );
		pos = expression.indexIn(text, i);

		if (pos >= 0) {
			// end comment found
			interface->setXinxFormat( 0, pos, formats["xml_comment"] );
			interface->setXinxFormat( pos, 3, formats["xml_syntaxchar"] );
			i += pos + 3; // skip comment
		} else {
			// in comment
			interface->setXinxFormat( 0, text.length(), formats["xml_comment"] );
			interface->setXinxCurrentBlockState( InComment );
			return;
		}
	}

	for(; i < text.length(); i++) {
		switch( text.at(i).toAscii() ) {
		case '<':
			brackets++;
			if( brackets == 1 ) {
				interface->setXinxFormat( i, 1, formats[ "xml_syntaxchar" ] );
				state = ExpectElementNameOrSlash;
			} else {
				// wrong bracket nesting
				interface->setXinxFormat( i, 1, formats[ "xml_error" ] );
			}
			break;

		case '>':
			brackets--;
			if( brackets == 0 ) {
				interface->setXinxFormat( i, 1, formats[ "xml_syntaxchar" ] );
			} else {
				// wrong bracket nesting
				interface->setXinxFormat( i, 1, formats["xml_error"] );
			}
			state = NoState;
			break;

		case '/':
			if( state == ExpectElementNameOrSlash ) {
				state = ExpectElementName;
				interface->setXinxFormat( i, 1, formats[ "xml_syntaxchar" ] );
			} else {
				if( state == ExpectAttributeOrEndOfElement ) {
					interface->setXinxFormat( i, 1, formats[ "xml_syntaxchar" ] );
				} else {
					processDefaultText( formats, interface, state, quoteType, i, text );
				}
			}
			break;

		case '=':
			if( state == ExpectEqual ) {
				state = ExpectAttributeValue;
				interface->setXinxFormat( i, 1, formats["xml_other"] );
			} else {
				processDefaultText( formats, interface, state, quoteType, i, text );  
			}
			break;

		case '\'':
		case '\"':
			if (state == ExpectAttributeValue) {
				quoteType = text.at(i);
				state = ExpectAttributeTextOrPath;
				interface->setXinxFormat( i, 1, formats["xml_syntaxchar"] );
			} else if( ( state == ExpectAttributeTextOrPath ) && ( quoteType == text.at(i) ) ) {
				state = ExpectAttributeOrEndOfElement;
				interface->setXinxFormat( i, 1, formats["xml_syntaxchar"] );
			} else {
				processDefaultText( formats, interface, state, quoteType, i, text );
			}
			break;
		case '{' :
			if( state == ExpectAttributeTextOrPath ) {
				state = ExpectPathTextOrEndOfPath;
				interface->setXinxFormat(i, 1, formats["xml_attributevalue" ] );
			} else
				processDefaultText( formats, interface, state, quoteType, i, text );
			break;
		case '}' :
			if( state == ExpectPathTextOrEndOfPath ) {
				state = ExpectAttributeTextOrPath;
				interface->setXinxFormat( i, 1, formats["xml_attributevalue"] );
			} else
				processDefaultText( formats, interface, state, quoteType, i, text );
			break;
		case '!':
			if( state == ExpectElementNameOrSlash ) {
				// search comment
				QRegExp expression(EXPR_COMMENT);
				pos = expression.indexIn(text, i - 1);

				if( pos == i - 1 ) { // comment found ?
					const int iLength = expression.matchedLength();

					interface->setXinxFormat( pos, 4, formats["xml_syntaxchar"] );
					interface->setXinxFormat( pos + 4, iLength - 7, formats["xml_comment"] );
					interface->setXinxFormat( pos + iLength - 3, 3, formats["xml_syntaxchar"] );
					i += iLength - 2; // skip comment
					state = NoState;
					brackets--;
				} else {
					// Try find multiline comment
					QRegExp expression(EXPR_COMMENT_BEGIN); // search comment start
					pos = expression.indexIn(text, i - 1);

					//if (pos == i - 1) // comment found ?
					if (pos >= i - 1) {
						interface->setXinxFormat( i, 3, formats["xml_syntaxchar"] );
						interface->setXinxFormat( i + 3, text.length() - i - 3, formats["xml_comment"] );
						interface->setXinxCurrentBlockState( InComment );
						return;
					} else {
						processDefaultText( formats, interface, state, quoteType, i, text );
					}
				}
			} else {
				processDefaultText( formats, interface, state, quoteType, i, text );
			}
			break;  

		default:
			const int iLength = processDefaultText( formats, interface, state, quoteType, i, text );
			if( iLength > 0 )
				i += iLength - 1;
			break;
		}
	}

	if( state == ExpectAttributeOrEndOfElement ) {
		interface->setXinxCurrentBlockState( InElement );
	}
}

int baseplugin_xml::processDefaultText( const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * interface, ParsingState state, const QChar & quoteType, int i, const QString& text ) {
	// length of matched text
	int iLength = 0;

	switch(state) {
	case ExpectElementNameOrSlash:
	case ExpectElementName: {
			// search element name
			QRegExp expression(EXPR_NAME);
			const int pos = expression.indexIn(text, i);

			if (pos == i) { // found ?
				iLength = expression.matchedLength();

				interface->setXinxFormat( pos, iLength, formats["xml_elementname"] );
				state = ExpectAttributeOrEndOfElement;
			} else {
				interface->setXinxFormat( i, 1, formats["xml_other"] );
			}
		}  
		break;
	case ExpectAttributeOrEndOfElement: {
			// search attribute name
			QRegExp expression(EXPR_NAME);
			const int pos = expression.indexIn(text, i);

			if (pos == i) { // found ?
				iLength = expression.matchedLength();

				interface->setXinxFormat( pos, iLength, formats["xml_attributename"] );
				state = ExpectEqual;
			} else {
				interface->setXinxFormat( i, 1, formats["xml_other"] );
			}
		}
		break;
	case ExpectAttributeTextOrPath: {
			QRegExp expression( EXPR_ATTRIBUTE_VALUE.arg( quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				interface->setXinxFormat( pos, iLength, formats["xml_attributevalue"] );
				interface->processText( i, text.mid( pos, iLength ) );
			} else
				interface->setXinxFormat( i, 1, formats["xml_other"] );
		}
		break;
	case ExpectPathTextOrEndOfPath: {
			QRegExp expression( EXPR_XPATH_VALUE.arg( quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				interface->setXinxFormat( pos, iLength, formats["xml_xpathvalue"] );
				interface->processText( i, text.mid( pos, iLength ) );
			} else
				interface->setXinxFormat( i, 1, formats["xml_other"] );
		}
		break;
	default:
		QRegExp expression( EXPR_TEXT );
		const int pos = expression.indexIn( text, i );

		if (pos == i) { // found ?
			iLength = expression.matchedLength();

			interface->setXinxFormat( pos, iLength, formats["xml_other"] );
			interface->processText( i, text.mid( pos, iLength ) );
		} else {
			interface->setXinxFormat( i, 1, formats["xml_other"] );
		}
		break;
	}
	return iLength;
}

