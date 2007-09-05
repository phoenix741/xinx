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

#ifndef JSHIGHLIGHTER_H
#define JSHIGHLIGHTER_H

#include "syntaxhighlighter.h"

/*!
 * Syntax highlighter on Javascript.
 */
class JsHighlighter  : public SyntaxHighlighter {
	Q_OBJECT
public:
	/*!
	 * Create a SyntaxHighlighter used to customize color on JavaScript.
	 * If the config is not defined, the config use a global scope.
	 * \param parent The parent of the object
	 * \param config The config to use.
	 */
	JsHighlighter( QObject* parent = NULL, XINXConfig * config = NULL ) : SyntaxHighlighter( parent, config ) { init(); };
	/*!
	 * Create a SyntaxHighlighter used to customize color on JavaScript.
	 * If the config is not defined, the config use a global scope.
	 * \param parent The parent of the object
	 * \param config The config to use.
	 */
	JsHighlighter( QTextDocument* parent, XINXConfig * config = NULL ) : SyntaxHighlighter( parent, config ) { init(); };
	/*!
	 * Create a SyntaxHighlighter used to customize color on JavaScript.
	 * If the config is not defined, the config use a global scope.
	 * \param parent The parent of the object
	 * \param config The config to use.
	 */
	JsHighlighter( QTextEdit* parent, XINXConfig * config = NULL ) : SyntaxHighlighter( parent, config ) { init(); };
	virtual ~JsHighlighter() {};
protected:
	void highlightBlock( const QString& rstrText );
	virtual void init();
private:
	enum BlockState {
		NoBlock = -1,
		InComment
	};
	
	QStringList keywordPatterns;
};
#endif
