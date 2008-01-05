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
#include "xinxpluginsloader.h"
#include "xinxconfig.h"

/* SyntaxHighlighter */

SyntaxHighlighter::SyntaxHighlighter( QObject* parent, const QString & highlighter, XINXConfig * config ) : QSyntaxHighlighter( parent ), m_interface( 0 ) {
	if( config ) 
		m_config = config;
	else
		m_config = global.m_config;
	setHighlighter( highlighter );
}

SyntaxHighlighter::SyntaxHighlighter( QTextDocument* parent, const QString & highlighter, XINXConfig * config ) : QSyntaxHighlighter( parent ), m_interface( 0 ) {
	if( config )
		m_config = config;
	else
		m_config = global.m_config;
	setHighlighter( highlighter );
}

SyntaxHighlighter::SyntaxHighlighter( QTextEdit* parent, const QString & highlighter, XINXConfig * config ) : QSyntaxHighlighter( parent ), m_interface( 0 ) {
	if( config )
		m_config = config;
	else
		m_config = global.m_config;
	setHighlighter( highlighter );
}

SyntaxHighlighter::~SyntaxHighlighter() {
}

void SyntaxHighlighter::setHighlightText( const QString & text ) {
	m_text = text;
	rehighlight();
}

void SyntaxHighlighter::setHighlighter( const QString & highlighter ) {
	m_highlighter = highlighter;
	m_interface   = NULL;
	foreach( IPluginSyntaxHighlighter * interface, global.m_pluginsLoader->syntaxPlugins() ) 
		if( interface->highlighters().contains( highlighter, Qt::CaseInsensitive ) ) 
			m_interface = interface;
	rehighlight();
}

const QString & SyntaxHighlighter::highlighter() const {
	return m_highlighter;
}

void SyntaxHighlighter::highlightBlock( const QString& text ) {
	if( m_interface )
		m_interface->highlightBlock( m_highlighter, m_config->config().formats, this, text );
	else  {
		processText( 0, text );
	}
}

int SyntaxHighlighter::currentBlockState () const {
	return QSyntaxHighlighter::currentBlockState();
}

int SyntaxHighlighter::previousBlockState () const {
	return QSyntaxHighlighter::previousBlockState();
}

void SyntaxHighlighter::setCurrentBlockState ( int newState ) {
	QSyntaxHighlighter::setCurrentBlockState( newState );
}

QTextCharFormat SyntaxHighlighter::format ( int position ) const {
	return QSyntaxHighlighter::format( position );
}

void SyntaxHighlighter::setFormat ( int start, int count, const QTextCharFormat & format ) {
	QSyntaxHighlighter::setFormat( start, count, format );
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

