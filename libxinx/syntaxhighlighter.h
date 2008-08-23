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

// Xinx header
#include <xinxcore.h>

class XINXConfig;
class IPluginSyntaxHighlighter;

/*! The Syntax highlighter used by XINX where configuration is based on \e XINXConfig */
class SyntaxHighlighter : public QSyntaxHighlighter {
	Q_OBJECT
public:
	/*! Create a Syntax Highlighter with the \e QTextDocument and a \e XINXConfig */
	SyntaxHighlighter( QTextDocument* parent, XINXConfig * config = NULL );
	/*! Destroy the Syntax Highlighter */
	virtual ~SyntaxHighlighter();
protected:
	XINXConfig * m_config;
};

/*! Regular expression of an identifier */
static const QString EXPR_TEXT = "[A-Za-z_][A-Za-z0-9_]*";

#endif
