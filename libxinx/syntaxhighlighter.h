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

class  SyntaxHighlighter : public QSyntaxHighlighter {
	Q_OBJECT
public:
	SyntaxHighlighter( QObject* parent = NULL, XINXConfig * config = NULL );
	SyntaxHighlighter( QTextDocument* parent, XINXConfig * config = NULL );
	SyntaxHighlighter( QTextEdit* parent, XINXConfig * config = NULL );
	virtual ~SyntaxHighlighter();
	
protected:
	XINXConfig * m_config;
};

static const QString EXPR_TEXT = "[A-Za-z_][A-Za-z0-9_]*";

#endif
