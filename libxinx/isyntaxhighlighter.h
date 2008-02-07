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

/*!
 * Interface used by plugins to communicate with XINX for the syntax higlighter.
 * The interface is similare to QSyntaxHighlighter of Qt. 
 */
class IXinxSyntaxHighlighter {
public:
	//! Destroy the interface. Used to hide warning when using the interface. 
	virtual ~IXinxSyntaxHighlighter() {};
	
	//! Return the state of the current block state.
	virtual int currentBlockState () const = 0;
	//! Return the state of the previous block state.
	virtual int previousBlockState () const = 0;
	//! Set the current state of the block.
	virtual void setCurrentBlockState ( int newState ) = 0;
	
	//! Return the format for the position \e position of the text.
	virtual QTextCharFormat format ( int position ) const = 0; 
	//! Set the format \e format, from \e start to \e start + \e count
	virtual void setFormat ( int start, int count, const QTextCharFormat & format ) = 0;

	//! Call this method to highlight in yellow a word in the text.
	virtual void processText( int pos, const QString& text ) = 0;
};

static const QString EXPR_TEXT = "[A-Za-z_][A-Za-z0-9_]*";

#endif /*ISYNTAXHIGHLIGHTER_H_*/
