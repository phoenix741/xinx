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

#ifndef INTERFACES_H_
#define INTERFACES_H_

#include <QtPlugin>

class SyntaxHighlighter;
class XINXConfig;
class QObject;
class QTextDocument;
class QTextEdit;

class SyntaxHighlighterInterface {
public:
	virtual ~SyntaxHighlighterInterface() {};
	
	virtual QStringList highlighters() = 0;
	virtual QString filterOf( const QString & highlighter ) = 0;
	virtual SyntaxHighlighter * newSyntaxHighlighter( QObject* parent = NULL, XINXConfig * config = NULL ) = 0;
	virtual SyntaxHighlighter * newSyntaxHighlighter( QTextDocument* parent, XINXConfig * config = NULL ) = 0;
	virtual SyntaxHighlighter * newSyntaxHighlighter( QTextEdit* parent, XINXConfig * config = NULL ) = 0;
};

Q_DECLARE_INTERFACE(SyntaxHighlighterInterface, "org.shadoware.xinx.SyntaxHighlighterInterface/1.0")

#endif /*INTERFACES_H_*/
