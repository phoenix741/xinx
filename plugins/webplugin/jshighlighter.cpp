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
#include <xinxconfig.h>
#include "jshighlighter.h"

/* Static member */
QStringList webplugin_js::keywordPatterns;

/* webplugin_js */

webplugin_js::webplugin_js( QObject* parent, XINXConfig * config ) : SyntaxHighlighter( parent, config ) {
	
}

webplugin_js::webplugin_js( QTextDocument* parent, XINXConfig * config ) : SyntaxHighlighter( parent, config ) {
	
}

webplugin_js::webplugin_js( QTextEdit* parent, XINXConfig * config ) : SyntaxHighlighter( parent, config ) {
	
}

void webplugin_js::init() {
	keywordPatterns.clear();
	keywordPatterns << "abstract" << "boolean" << "break" 
					<< "byte" << "case" << "catch" << "char" 
					<< "class" << "const" << "continue" 
					<< "debugger" << "default" << "delete" 
					<< "do" << "double" << "else" << "enum" 
					<< "export" << "extends" << "false" << "final" 
					<< "finally" << "float" << "for" << "function" 
					<< "goto" << "if" << "implements" << "import" 
					<< "in" << "instanceof" << "int" << "interface" 
					<< "long" << "native" << "new" << "null" 
					<< "package" << "private" << "protected" 
					<< "public" << "return" << "short" << "static" 
					<< "super" << "switch" << "synchronized" 
					<< "this" << "throw" << "throws" << "transient" 
					<< "true" << "try" << "typeof" << "var" 
					<< "void" << "volatile" << "while" << "with";
	keywordPatterns.sort();
}

void webplugin_js::highlightBlock( const QString & text ) {
	const QHash<QString,QTextCharFormat> & formats = m_config->config().formats;
	
	int i = 0;
	int pos = 0;

	QRegExp commentStartExpression("^/\\*");
	QRegExp commentEndExpression("\\*/");
	QRegExp numberExpression("^\\b[\\-\\+]?[0-9]+(\\.[0-9]+)?\\b");
	QRegExp functionNameExpression("^\\b[A-Za-z0-9_]+(?=\\s*\\()");
	QRegExp motExpression("^\\w*");
	QRegExp commentExpression("^//[^\n]*");
	QRegExp string1Expression("^'[^\']*'");
	QRegExp string2Expression("^\"[^\"]*\"");
		
	setCurrentBlockState( NoBlock );
	
	if ( previousBlockState() == InComment ) {
		pos = commentEndExpression.indexIn( text, i );

		if (pos >= 0) {
			// end comment found
			const int iLength = commentEndExpression.matchedLength();
			setFormat( 0, pos + iLength, formats["js_comment"] );
			i += pos + iLength; // skip comment
		} else {
			// in comment
			setFormat( 0, text.length(), formats["js_comment"] );
			setCurrentBlockState( InComment );
			return;
		}
	}
	
	for (; i < text.length(); i++) {
		char c = text.at(i).toLower().toAscii();
		if( ( c >= '0' ) && ( c <= '9' ) ) {
			pos = numberExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				const int iLength = numberExpression.matchedLength();
				setFormat( pos, iLength, formats["js_number"] );
				i += iLength;
			} 
		} else if( ( c >= 'a' ) && ( c <= 'z' ) ) {
			pos = motExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			if( pos == i ) {
				const int iLength = motExpression.matchedLength();
				if( keywordPatterns.contains( text.mid( i, iLength ) ) ) {
					setFormat( pos, iLength, formats["js_reservedword"] );
				} else {
					setFormat( pos, iLength, formats["js_other"] );
				}
				i += iLength;
			} 
		} else if ( c == '/' ) {
			pos = commentStartExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				int posEnd = commentEndExpression.indexIn( text, i + 2 );
 				int length = (posEnd >= 0) ? posEnd + 2 : ( text.length() - pos );

 				setFormat( pos, length, formats["js_comment"] );
				i += length;
				if( posEnd == -1 ) {
					setCurrentBlockState( InComment );
					return;
				}
			} else {
				pos = commentExpression.indexIn( text, i, QRegExp::CaretAtOffset );
				if( pos == i ) {
					const int iLength = commentExpression.matchedLength() + 2;
					i += iLength;
					setFormat( pos, iLength, formats["js_comment"] );
				}
			}
		} else if ( ( c == '\'' ) || ( c == '"' ) ) {
			int iLength;
			if( c == '\'' ) {
				pos = string1Expression.indexIn( text, i, QRegExp::CaretAtOffset );
				iLength = string1Expression.matchedLength();
			} else { 
				pos = string2Expression.indexIn( text, i, QRegExp::CaretAtOffset );
				iLength = string2Expression.matchedLength();
			}
			
			if( pos == i ) {
				setFormat( pos, iLength, formats["js_string"] );
				i += iLength;
			} 
		} else {
			setFormat( i, 1, formats["js_other"] );
		}
	}	
}
