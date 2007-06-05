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

#include "jshighlighter.h"
#include "xinxconfig.h"
#include "globals.h"
//

static const QColor DEFAULT_RESERVEDWORD	= Qt::black;
static const QColor DEFAULT_NUMBER			= Qt::blue;
static const QColor DEFAULT_STRING			= Qt::red;

void JsHighlighter::init( bool useConfig ) {
	SyntaxHighlighter::init( useConfig );
	
	if( ! useConfig ) {
		m_syntaxFormats["ReservedWord"].setForeground( DEFAULT_RESERVEDWORD );
		m_syntaxFormats["ReservedWord"].setFontWeight(QFont::Bold);
		m_syntaxFormats["Number"].setForeground( DEFAULT_NUMBER );
		m_syntaxFormats["String"].setForeground( DEFAULT_STRING );	
	} else {
		foreach( QString key, global.m_xinxConfig->managedStructure()["js"].color.keys() ) {
			m_syntaxFormats[ key ] = global.m_xinxConfig->managedStructure()["js"].color[ key ];
		}
	}
	
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

bool JsHighlighter::isFormat( QString type ) {
	if( type == "ReservedWord" ) 
		return true;
	else 	
	if( type == "Number" ) 
		return true;
	else
	if( type == "String" ) 
		return true;

	return SyntaxHighlighter::isFormat( type );
}


void JsHighlighter::highlightBlock( const QString & text ) {
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
		
	setCurrentBlockState(NoBlock);
	
	if ( previousBlockState() == InComment ) {
		pos = commentEndExpression.indexIn( text, i );

		if (pos >= 0) {
			// end comment found
			const int iLength = commentEndExpression.matchedLength();
			setFormat( 0, pos + iLength, m_syntaxFormats["Comment"] );
			i += pos + iLength; // skip comment
		} else {
			// in comment
			setFormat( 0, text.length(), m_syntaxFormats["Comment"] );
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
				setFormat( pos, iLength, m_syntaxFormats["Number"] );
				i += iLength;
			}
		} else if( ( c >= 'a' ) && ( c <= 'z' ) ) {
			pos = motExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			if( pos == i ) {
				const int iLength = motExpression.matchedLength();
				if( keywordPatterns.contains( text.mid( i, iLength ) ) ) {
					setFormat( pos, iLength, m_syntaxFormats["ReservedWord"] );
				}
				i += iLength;
			}
		} else if ( c == '/' ) {
			pos = commentStartExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				int posEnd = commentEndExpression.indexIn( text, i + 2 );
 				int length = (posEnd >= 0) ? posEnd + 2 : ( text.length() - pos );

				setFormat( pos, length, m_syntaxFormats["Comment"] );
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
					setFormat( pos, iLength, m_syntaxFormats["Comment"] );
				}
			}
		} else if ( c == '\'' ) {
			pos = string1Expression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				const int iLength = string1Expression.matchedLength();
				setFormat( pos, iLength, m_syntaxFormats["String"] );
				i += iLength;
			}
		} else if ( c == '\"' ) {
			pos = string2Expression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				const int iLength = string2Expression.matchedLength();
				setFormat( pos, iLength, m_syntaxFormats["String"] );
				i += iLength;
			}
		}
	}	
}

//
