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

int processDefaultText( const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * interface, baseplugin_xml::ParsingState & state, const QChar & quoteType, int i, const QString& text ) {
	// length of matched text
	int iLength = 0;

	switch(state) {
	case baseplugin_xml::ExpectElementNameOrSlash:
	case baseplugin_xml::ExpectElementName: {
			// search element name
			QRegExp expression(EXPR_NAME);
			const int pos = expression.indexIn(text, i);

			if (pos == i) { // found ?
				iLength = expression.matchedLength();

				interface->setFormat( pos, iLength, formats["xml_elementname"] );
				state = baseplugin_xml::ExpectAttributeOrEndOfElement;
			} else {
				interface->setFormat( i, 1, formats["xml_other"] );
			}
		}  
		break;
	case baseplugin_xml::ExpectAttributeOrEndOfElement: {
			// search attribute name
			QRegExp expression(EXPR_NAME);
			const int pos = expression.indexIn(text, i);

			if (pos == i) { // found ?
				iLength = expression.matchedLength();

				interface->setFormat( pos, iLength, formats["xml_attributename"] );
				state = baseplugin_xml::ExpectEqual;
			} else {
				interface->setFormat( i, 1, formats["xml_other"] );
			}
		}
		break;
	case baseplugin_xml::ExpectAttributeTextOrPath: {
			QRegExp expression( EXPR_ATTRIBUTE_VALUE.arg( quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				interface->setFormat( pos, iLength, formats["xml_attributevalue"] );
				interface->processText( i, text.mid( pos, iLength ) );
			} else
				interface->setFormat( i, 1, formats["xml_other"] );
		}
		break;
	case baseplugin_xml::ExpectPathTextOrEndOfPath: {
			QRegExp expression( EXPR_XPATH_VALUE.arg( quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				interface->setFormat( pos, iLength, formats["xml_xpathvalue"] );
				interface->processText( i, text.mid( pos, iLength ) );
			} else
				interface->setFormat( i, 1, formats["xml_other"] );
		}
		break;
	default:
		QRegExp expression( EXPR_TEXT );
		const int pos = expression.indexIn( text, i );

		if (pos == i) { // found ?
			iLength = expression.matchedLength();

			interface->setFormat( pos, iLength, formats["xml_other"] );
			interface->processText( i, text.mid( pos, iLength ) );
		} else {
			interface->setFormat( i, 1, formats["xml_other"] );
		}
		break;
	}
	return iLength;
}

void baseplugin_xml::highlightBlock( const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * interface, const QString& text ) {
	int i = 0;
	int pos = 0;
	int brackets = 0;
	QChar quoteType;
	
	ParsingState state = ( interface->previousBlockState() == InElement ? ExpectAttributeOrEndOfElement : NoState );
	
	interface->setCurrentBlockState( NoBlock );

	if( interface->previousBlockState() == InComment ) {
		// search for the end of the comment
		QRegExp expression( EXPR_COMMENT_END );
		pos = expression.indexIn(text, i);

		if (pos >= 0) {
			// end comment found
			interface->setFormat( 0, pos, formats["xml_comment"] );
			interface->setFormat( pos, 3, formats["xml_syntaxchar"] );
			i += pos + 3; // skip comment
		} else {
			// in comment
			interface->setFormat( 0, text.length(), formats["xml_comment"] );
			interface->setCurrentBlockState( InComment );
			return;
		}
	}

	for(; i < text.length(); i++) {
		switch( text.at(i).toAscii() ) {
		case '<':
			brackets++;
			if( brackets == 1 ) {
				interface->setFormat( i, 1, formats[ "xml_syntaxchar" ] );
				state = ExpectElementNameOrSlash;
			} else {
				// wrong bracket nesting
				interface->setFormat( i, 1, formats[ "xml_error" ] );
			}
			break;

		case '>':
			brackets--;
			if( brackets == 0 ) {
				interface->setFormat( i, 1, formats[ "xml_syntaxchar" ] );
			} else {
				// wrong bracket nesting
				interface->setFormat( i, 1, formats["xml_error"] );
			}
			state = NoState;
			break;

		case '/':
			if( state == ExpectElementNameOrSlash ) {
				state = ExpectElementName;
				interface->setFormat( i, 1, formats[ "xml_syntaxchar" ] );
			} else {
				if( state == ExpectAttributeOrEndOfElement ) {
					interface->setFormat( i, 1, formats[ "xml_syntaxchar" ] );
				} else {
					processDefaultText( formats, interface, state, quoteType, i, text );
				}
			}
			break;

		case '=':
			if( state == ExpectEqual ) {
				state = ExpectAttributeValue;
				interface->setFormat( i, 1, formats["xml_other"] );
			} else {
				processDefaultText( formats, interface, state, quoteType, i, text );  
			}
			break;

		case '\'':
		case '\"':
			if (state == ExpectAttributeValue) {
				quoteType = text.at(i);
				state = ExpectAttributeTextOrPath;
				interface->setFormat( i, 1, formats["xml_syntaxchar"] );
			} else if( ( state == ExpectAttributeTextOrPath ) && ( quoteType == text.at(i) ) ) {
				state = ExpectAttributeOrEndOfElement;
				interface->setFormat( i, 1, formats["xml_syntaxchar"] );
			} else {
				processDefaultText( formats, interface, state, quoteType, i, text );
			}
			break;
		case '{' :
			if( state == ExpectAttributeTextOrPath ) {
				state = ExpectPathTextOrEndOfPath;
				interface->setFormat(i, 1, formats["xml_attributevalue" ] );
			} else
				processDefaultText( formats, interface, state, quoteType, i, text );
			break;
		case '}' :
			if( state == ExpectPathTextOrEndOfPath ) {
				state = ExpectAttributeTextOrPath;
				interface->setFormat( i, 1, formats["xml_attributevalue"] );
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

					interface->setFormat( pos, 4, formats["xml_syntaxchar"] );
					interface->setFormat( pos + 4, iLength - 7, formats["xml_comment"] );
					interface->setFormat( pos + iLength - 3, 3, formats["xml_syntaxchar"] );
					i += iLength - 2; // skip comment
					state = NoState;
					brackets--;
				} else {
					// Try find multiline comment
					QRegExp expression(EXPR_COMMENT_BEGIN); // search comment start
					pos = expression.indexIn(text, i - 1);

					//if (pos == i - 1) // comment found ?
					if (pos >= i - 1) {
						interface->setFormat( i, 3, formats["xml_syntaxchar"] );
						interface->setFormat( i + 3, text.length() - i - 3, formats["xml_comment"] );
						interface->setCurrentBlockState( InComment );
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
		interface->setCurrentBlockState( InElement );
	}
}

