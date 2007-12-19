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

#ifndef ISYNTAXHIGHLIGHTER_H_
#define ISYNTAXHIGHLIGHTER_H_

// Qt header
#include <QTextCharFormat>
#include <QColor>
#include <QFont>

class IXinxSyntaxHighlighter {
public:
	virtual int currentBlockState () const = 0;
	virtual int previousBlockState () const = 0;
	virtual void setCurrentBlockState ( int newState ) = 0;
	
	virtual QTextCharFormat format ( int position ) const = 0; 
	virtual void setFormat ( int start, int count, const QTextCharFormat & format ) = 0;
	virtual void setFormat ( int start, int count, const QColor & color ) = 0;
	virtual void setFormat ( int start, int count, const QFont & font ) = 0;

	virtual void processText( int pos, const QString& text ) = 0;
};

static const QString EXPR_TEXT = "[A-Za-z_][A-Za-z0-9_]*";

#endif /*ISYNTAXHIGHLIGHTER_H_*/
