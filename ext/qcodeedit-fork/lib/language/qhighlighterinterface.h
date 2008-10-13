/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _QHIGHLIGHTER_INTERFACE_H_
#define _QHIGHLIGHTER_INTERFACE_H_

/*!
	\file qhighlighterinterface.h
	\brief Definition of the QHighlighterInterface class.
	
	\see QHighlighterInterface
*/

/*!
	\class QHighlighterInterface
	\brief The highlighter itself
	
	This pure virtual classes features only one function that is called by
	attached matchers when needed.
	
	Note : As other interfaces, QHighlighterInterface is "editor-independent",
	one interface only is created by extension and several matchers may share
	an interface, so do NOT store data in it...
	
	\see QHighlighter
	\see QBlockData
*/

class QColor;
class QHighlighter;

class QHighlighterInterface
{
	public:
		/*!
			\brief Empty destructor needed by the pure virtual callback...
		*/
		virtual ~QHighlighterInterface() {}
		
		/*!
			\brief Highlighting callback
			
			\param line Text line to be highlighted
			\param format QHighlighter object attached to the "calling editor"
			
			Highlighting is done in this virtual function, on the content of
			the text line \a line and through the \a format factory
		*/
		virtual void highlight(QDocumentLine& line, QFormatFactory *format) = 0;
		
		/*!
			\brief Hackish way to enable comment/uncomment feature in the editor
		*/
		virtual QString singleLineComment() const = 0;
};

#endif // _QHIGHLIGHTER_INTERFACE_H_
