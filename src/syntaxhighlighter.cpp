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

// Xinx header
#include "syntaxhighlighter.h"
#include "globals.h"
#include "xinxconfig.h"

const QString EXPR_TEXT = "[A-Za-z_][A-Za-z0-9_]*";

/* SyntaxHighlighter */

SyntaxHighlighter::SyntaxHighlighter( QObject* parent, XINXConfig * config ) : QSyntaxHighlighter( parent ) {
	if( config )
		m_config = config;
	else
		m_config = global.m_config;
}

SyntaxHighlighter::SyntaxHighlighter( QTextDocument* parent, XINXConfig * config ) : QSyntaxHighlighter( parent ) {
	if( config )
		m_config = config;
	else
		m_config = global.m_config;
}

SyntaxHighlighter::SyntaxHighlighter( QTextEdit* parent, XINXConfig * config ) : QSyntaxHighlighter( parent ) {
	if( config )
		m_config = config;
	else
		m_config = global.m_config;
}

SyntaxHighlighter::~SyntaxHighlighter() {
}

void SyntaxHighlighter::setHighlightText( const QString & text ) {
	m_text = text;
	rehighlight();
}

void SyntaxHighlighter::processText( int pos, const QString& text ) {
	if( ! m_text.isEmpty() ) {
		QRegExp expression( m_text );
		int i = 0;
		do {
			i = expression.indexIn( text, i );
			if( i >= 0 ) {
				const int iLength = expression.matchedLength();
				QTextCharFormat f = format( pos + i );
				f.setBackground( m_config->config().editor.highlightWord );
				setFormat( pos + i, iLength, f );
				i += iLength;
			}
		} while( i >= 0 );
	}
}

