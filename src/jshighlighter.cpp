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
//

static const QColor DEFAULT_RESERVEDWORD	= Qt::black;
static const QColor DEFAULT_NUMBER			= Qt::blue;
static const QColor DEFAULT_STRING			= Qt::red;
static const QColor DEFAULT_COMMENT			= Qt::darkGreen;
static const QColor DEFAULT_ERROR			= Qt::darkMagenta;
static const QColor DEFAULT_OTHER			= Qt::black;

JsHighlighter::JsHighlighter( QObject* parent ) : SyntaxHighlighter( parent ) {
	init();
}

JsHighlighter::JsHighlighter( QTextDocument* parent ) : SyntaxHighlighter( parent ) {
	init();
}

JsHighlighter::JsHighlighter( QTextEdit* parent ) : SyntaxHighlighter( parent ) {
	init();
}

JsHighlighter::~JsHighlighter() {
}

void JsHighlighter::init() {
	fmtReservedWord.setForeground( DEFAULT_RESERVEDWORD );
	fmtReservedWord.setFontWeight(QFont::Bold);
	fmtNumber.setForeground( DEFAULT_NUMBER );
	fmtString.setForeground( DEFAULT_STRING );
	fmtComment.setForeground( DEFAULT_COMMENT );
	fmtError.setForeground( DEFAULT_ERROR );
	fmtOther.setForeground( DEFAULT_OTHER );
	
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

void JsHighlighter::setHighlightColor( HighlightType type, QColor color, bool foreground ) {
	QTextCharFormat format;
	if (foreground)
		format.setForeground(color);
	else
		format.setBackground(color);
	setHighlightFormat(type, format);
}

void JsHighlighter::setHighlightFormat(HighlightType type, QTextCharFormat format) {
	switch (type)
	{
		case ReservedWord:
			fmtReservedWord = format;
			break;
		case Number:
			fmtNumber = format;
			break;
		case String:
			fmtString = format;
			break;
		case Comment:
			fmtComment = format;
			break;
		case Error:
			fmtError = format;
			break;
		case Other:
			fmtOther = format;
			break;
	}
	rehighlight();
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
			setFormat( 0, pos + iLength, fmtComment );
			i += pos + iLength; // skip comment
		} else {
			// in comment
			setFormat( 0, text.length(), fmtComment );
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
				setFormat( pos, iLength, fmtNumber );
				i += iLength;
			}
		} else if( ( c >= 'a' ) && ( c <= 'z' ) ) {
			pos = motExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			if( pos == i ) {
				const int iLength = motExpression.matchedLength();
				if( keywordPatterns.contains( text.mid( i, iLength ) ) ) {
					setFormat( pos, iLength, fmtReservedWord );
				}
				i += iLength;
			}
		} else if ( c == '/' ) {
			pos = commentStartExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				int posEnd = commentEndExpression.indexIn( text, i + 2 ) + 2;
 				int length = (posEnd >= 0) ? posEnd : ( text.length() - pos );

				setFormat( pos, length, fmtComment );
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
					setFormat( pos, iLength, fmtComment );
				}
			}
		} else if ( c == '\'' ) {
			pos = string1Expression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				const int iLength = string1Expression.matchedLength();
				setFormat( pos, iLength, fmtString );
				i += iLength;
			}
		} else if ( c == '\"' ) {
			pos = string2Expression.indexIn( text, i, QRegExp::CaretAtOffset );
			
			if( pos == i ) {
				const int iLength = string2Expression.matchedLength();
				setFormat( pos, iLength, fmtString );
				i += iLength;
			}
		}
	}	
}

//
