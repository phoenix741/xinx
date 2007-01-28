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

JsHighlighter::JsHighlighter( QObject* parent ) : QSyntaxHighlighter( parent ) {
	init();
}

JsHighlighter::JsHighlighter( QTextDocument* parent ) : QSyntaxHighlighter( parent ) {
	init();
}

JsHighlighter::JsHighlighter( QTextEdit* parent ) : QSyntaxHighlighter( parent ) {
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
	fmtFunction.setForeground( DEFAULT_OTHER );
    fmtFunction.setFontItalic(true);
	
	keywordPatterns.clear();
	keywordPatterns << "\\babstract\\b" << "\\bboolean\\b" << "\\bbreak\\b" 
					<< "\\bbyte\\b" << "\\bcase\\b" << "\\bcatch\\b" << "\\bchar\\b" 
					<< "\\bclass\\b" << "\\bconst\\b" << "\\bcontinue\\b" 
					<< "\\bdebugger\\b" << "\\bdefault\\b" << "\\bdelete\\b" 
					<< "\\bdo\\b" << "\\bdouble\\b" << "\\belse\\b" << "\\benum\\b" 
					<< "\\bexport\\b" << "\\bextends\\b" << "\\bfalse\\b" << "\\bfinal\\b" 
					<< "\\bfinally\\b" << "\\bfloat\\b" << "\\bfor\\b" << "\\bfunction\\b" 
					<< "\\bgoto\\b" << "\\bif\\b" << "\\bimplements\\b" << "\\bimport\\b" 
					<< "\\bin\\b" << "\\binstanceof\\b" << "\\bint\\b" << "\\binterface\\b" 
					<< "\\blong\\b" << "\\bnative\\b" << "\\bnew\\b" << "\\bnull\\b" 
					<< "\\bpackage\\b" << "\\bprivate\\b" << "\\bprotected\\b" 
					<< "\\bpublic\\b" << "\\breturn\\b" << "\\bshort\\b" << "\\bstatic\\b" 
					<< "\\bsuper\\b" << "\\bswitch\\b" << "\\bsynchronized\\b" 
					<< "\\bthis\\b" << "\\bthrow\\b" << "\\bthrows\\b" << "\\btransient\\b" 
					<< "\\btrue\\b" << "\\btry\\b" << "\\btypeof\\b" << "\\bvar\\b" 
					<< "\\bvoid\\b" << "\\bvolatile\\b" << "\\bwhile\\b" << "\\bwith\\b";
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
		case Function:
			fmtFunction = format;
			break;
	}
	rehighlight();
}

void JsHighlighter::highlightBlock( const QString& text ) {
	foreach ( QString pattern, keywordPatterns ) {
		processDefaultText( pattern, text, fmtReservedWord );
	}
	processDefaultText( "\\b[A-Za-z0-9_]+(?=\\s*\\()", text, fmtFunction );
	processDefaultText( "\\b[\\-\\+]?[0-9]+(\\.[0-9]+)?\\b", text, fmtNumber );
	processDefaultText( "\"[^\"]*\"", text, fmtString );
	processDefaultText( "'[^\']*'", text, fmtString );
	processDefaultText( "//[^\n]*", text, fmtComment );


	QRegExp commentStartExpression("/\\*");
	QRegExp commentEndExpression("\\*/");
     
	setCurrentBlockState( 0 ); 
	int startIndex = 0;
	if ( previousBlockState() != 1 )
		startIndex = text.indexOf( commentStartExpression );
		
	while (startIndex >= 0) {
		int endIndex = text.indexOf( commentEndExpression, startIndex );
		int commentLength;
		if ( endIndex == -1 ) {
			setCurrentBlockState( 1 );
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
		}
		setFormat( startIndex, commentLength, fmtComment );
		startIndex = text.indexOf( commentStartExpression, startIndex + commentLength );
	}	
}

void JsHighlighter::processDefaultText( const QString& pattern, const QString& text, const QTextCharFormat& format ) {
	QRegExp expression( pattern );
	int index = text.indexOf( expression );
	while ( index >= 0 ) {
		int length = expression.matchedLength();
		setFormat( index, length, format );
		index = text.indexOf(expression, index + length);
	}
}

//
