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
//
#include <QSyntaxHighlighter>
#include <QHash>
//

class SyntaxHighlighter : public QSyntaxHighlighter {
	Q_OBJECT
public:
	SyntaxHighlighter( QObject* parent = NULL, bool useConfig = true );
	SyntaxHighlighter( QTextDocument* parent, bool useConfig = true );
	SyntaxHighlighter( QTextEdit* parent, bool useConfig = true );
	virtual ~SyntaxHighlighter();

	virtual void setHighlightColor( QString type, QColor color, bool foreground = true );
	virtual void setHighlightFormat( QString type, QTextCharFormat format );
	
	virtual bool isFormat( QString type );
	
	const QHash<QString,QTextCharFormat> & formats() const { return m_syntaxFormats; };
protected:
	virtual void init( bool useConfig );
	QHash<QString,QTextCharFormat> m_syntaxFormats;
};
#endif
