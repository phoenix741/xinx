/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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
#include "xqhighlighter.h"
#include "xquery_keyword.h"

/* Static */
QStringList XQHighlighter::m_keywords;

/* XQHighlighter */

XQHighlighter::XQHighlighter( QTextDocument* parent, XINXConfig * config ) : SyntaxHighlighter( parent, config ) {
	if( m_keywords.count() == 0 )
		m_keywords = XQueryKeyword::self()->keywords().keys();
}

void XQHighlighter::highlightBlock( const QString & text ) {
	const QHash<QString,QTextCharFormat> & formats = m_config->config().formats;

	int i = 0;
	int pos = 0;

	QRegExp numberExpression("^\\b[\\-\\+]?[0-9]+(\\.[0-9]+)?\\b");
	QRegExp motExpression("^\\w*");

	for (; i < text.length(); i++) {
		char c = text.at(i).toLower().toAscii();
		if( ( c >= '0' ) && ( c <= '9' ) ) {
			pos = numberExpression.indexIn( text, i, QRegExp::CaretAtOffset );

			if( pos == i ) {
				const int iLength = numberExpression.matchedLength();
				setFormat( pos, iLength, formats["xq_numerical"] );
				i += iLength;
			}
		} else if( ( c >= 'a' ) && ( c <= 'z' ) ) {
			pos = motExpression.indexIn( text, i, QRegExp::CaretAtOffset );
			if( pos == i ) {
				const int iLength = motExpression.matchedLength();
				QString matchedText = text.mid( i, iLength );
				if( m_keywords.contains( matchedText ) ) {
					setFormat( pos, iLength, formats["xq_" + XQueryKeyword::self()->keywords()[matchedText] ] );
				} else {
					setFormat( pos, iLength, formats["xq_other"] );
				}
				i += iLength;
			}
		} else {
			setFormat( i, 1, formats["xq_other"] );
		}
	}
}
