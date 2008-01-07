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

#ifndef XMLHIGHLIGHTER_H_
#define XMLHIGHLIGHTER_H_

// Xinx header
#include <isyntaxhighlighter.h>

// Qt header
#include <QHash>
#include <QString>
#include <QTextCharFormat>

namespace baseplugin_xml {
	enum ParsingState {
		NoState = 0,
		ExpectElementNameOrSlash,
		ExpectElementName,
		ExpectAttributeOrEndOfElement,
		ExpectEqual,
		ExpectAttributeValue,
		ExpectAttributeTextOrPath,
		ExpectPathTextOrEndOfPath
	};

	enum BlockState {
		NoBlock = -1,
		InComment,
		InElement
	};

	extern void highlightBlock( const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * interface, const QString& text );
};

#endif /*XMLHIGHLIGHTER_H_*/
