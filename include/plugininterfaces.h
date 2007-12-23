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

// Qt header
#include <QtPlugin>
#include <QHash>
#include <QStringList>
#include <QTextCharFormat>

// Xinx header
#include <isyntaxhighlighter.h>

class SyntaxHighlighter;
class XINXConfig;
class QObject;
class QTextDocument;
class QTextEdit;

/*!
 * This interface represents a plugins. The plugins is used to highlight the text.
 */
class IPluginSyntaxHighlighter {
public:
	//! Destroy the interface. Used to hide warning when using the interface.
	virtual ~IPluginSyntaxHighlighter() {};
	
	//! Return the list of plugins what can process the file for syntax highlighter.
	virtual QStringList plugins() = 0;
	//! Return the description of each plugins
	virtual QHash<QString,QString> descriptionOfPlugins() = 0;
	//! Return all the filters that the plugins knwon.
	virtual QHash<QString,QString> filters() = 0;
	//! Return the highlighter that can manage the extention.
	virtual QString highlighterOfExtention( const QString & extention ) = 0;
	//! Return the default formats of a highlighter
	virtual QHash<QString,QTextCharFormat> formatOfHighlighter( const QString & highlighter ) = 0;
	//! Return an example of highlighter.
	virtual QString exampleOfHighlighter( const QString & highlighter ) = 0;
	/*!
	 * Process a block
	 * \param highlighter The highlighter to used
	 * \param formats Formats to used for the coloration.
	 * \param i The interface with the application.
	 * \param text The text to process.
	 */
	virtual void highlightBlock( const QString & highlighter, const QHash<QString,QTextCharFormat> & formats, IXinxSyntaxHighlighter * i, const QString& text ) = 0;
};

Q_DECLARE_INTERFACE(IPluginSyntaxHighlighter, "org.shadoware.xinx.IPluginSyntaxHighlighter/1.0")

#endif /*INTERFACES_H_*/
