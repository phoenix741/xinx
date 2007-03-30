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

#include "syntaxhighlighter.h"

static const QColor DEFAULT_COMMENT			= Qt::darkGreen;
static const QColor DEFAULT_ERROR			= Qt::darkMagenta;
static const QColor DEFAULT_OTHER			= Qt::black;

SyntaxHighlighter::SyntaxHighlighter( QObject* parent, bool useConfig ) : QSyntaxHighlighter( parent ) {
	init( useConfig );
}

SyntaxHighlighter::SyntaxHighlighter( QTextDocument* parent, bool useConfig ) : QSyntaxHighlighter( parent ) {
	init( useConfig );
}

SyntaxHighlighter::SyntaxHighlighter( QTextEdit* parent, bool useConfig ) : QSyntaxHighlighter( parent ) {
	init( useConfig );
}

SyntaxHighlighter::~SyntaxHighlighter() {
}

bool SyntaxHighlighter::isFormat( QString type ) {
	if( type == "Comment" ) 
		return true;
	else 	
	if( type == "Error" ) 
		return true;
	else
	if( type == "Other" ) 
		return true;
	return false;
}

void SyntaxHighlighter::init( bool useConfig ) {
	if( ! useConfig ) {
		m_syntaxFormats["Comment"].setForeground( DEFAULT_COMMENT );
		m_syntaxFormats["Error"].setForeground( DEFAULT_ERROR );
		m_syntaxFormats["Other"].setForeground( DEFAULT_OTHER );
	}
}

void SyntaxHighlighter::setHighlightColor( QString type, QColor color, bool foreground ) {
	QTextCharFormat format;
	if ( foreground )
		format.setForeground( color );
	else
		format.setBackground( color );

	setHighlightFormat( type, format );
}

void SyntaxHighlighter::setHighlightFormat( QString type, QTextCharFormat format ) {
	if( isFormat( type ) ) {
		m_syntaxFormats[ type ] = format;
		rehighlight();
	}
}

