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

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

// Qt header
#include <QSyntaxHighlighter>
#include <QHash>

// Xinx header
#include "isyntaxhighlighter.h"

class XINXConfig;
class IPluginSyntaxHighlighter;

class SyntaxHighlighter : public QSyntaxHighlighter, protected IXinxSyntaxHighlighter {
	Q_OBJECT
public:
	SyntaxHighlighter( QObject* parent = NULL, const QString & highlighter = QString(), XINXConfig * config = NULL );
	SyntaxHighlighter( QTextDocument* parent, const QString & highlighter, XINXConfig * config = NULL );
	SyntaxHighlighter( QTextEdit* parent, const QString & highlighter, XINXConfig * config = NULL );
	virtual ~SyntaxHighlighter();
	
	void setHighlightText( const QString & text );
	void setHighlighter( const QString & highlighter );
	const QString & highlighter() const;
protected:
	virtual void highlightBlock( const QString& text );

	virtual int currentBlockState () const;
	virtual int previousBlockState () const;
	virtual void setCurrentBlockState ( int newState );
	
	virtual QTextCharFormat format ( int position ) const; 
	virtual void setFormat ( int start, int count, const QTextCharFormat & format );

	void processText( int pos, const QString& text );
	
	IPluginSyntaxHighlighter * m_interface;
	XINXConfig * m_config;
	QString m_text, m_highlighter;
};

#endif
