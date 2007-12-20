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

#ifndef CSSHIGHLIGHTER_H_
#define CSSHIGHLIGHTER_H_

// Xinx header
#include <isyntaxhighlighter.h>

// Qt header
#include <QHash>
#include <QString>
#include <QTextCharFormat>

namespace baseplugin_css {
/*
#define SCE_CSS_DEFAULT 0
#define SCE_CSS_TAG 1
#define SCE_CSS_CLASS 2
#define SCE_CSS_PSEUDOCLASS 3
#define SCE_CSS_UNKNOWN_PSEUDOCLASS 4
#define SCE_CSS_OPERATOR 5
#define SCE_CSS_IDENTIFIER 6
#define SCE_CSS_UNKNOWN_IDENTIFIER 7
#define SCE_CSS_VALUE 8
#define SCE_CSS_COMMENT 9
#define SCE_CSS_ID 10
#define SCE_CSS_IMPORTANT 11
#define SCE_CSS_DIRECTIVE 12
#define SCE_CSS_DOUBLESTRING 13
#define SCE_CSS_SINGLESTRING 14
#define SCE_CSS_IDENTIFIER2 15
#define SCE_CSS_ATTRIBUTE 16
*/
	enum ParsingState {
		Default = 0
	};

	enum BlockState {
		NoBlock = -1,
		InComment,
		InElement
	};

	extern void highlightBlock( const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * interface, const QString& text );
};

#endif /*CSSHIGHLIGHTER_H_*/
