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

#ifndef _QMATCHER_INTERFACE_H_
#define _QMATCHER_INTERFACE_H_

/*!
	\file qmatcherinterface.h
	\brief Definition of the QMatcherInterface class.
	
	\see QMatcherInterface
*/

/*!
	\class QMatcherInterface
	\brief The matcher itself
	
	This pure virtual classes features only one function that is called by
	attached matchers when needed.
	
	Note : As other interfaces, QMatcherInterface is "editor-independent", one
	interface only is created by extension and several matchers may share an
	interface, so do NOT store data in it... Or according to the QMatcher*
	passed...
	
	\see QMatcher
	\see QBlockData
*/

class QChar;
class QMatcher;
class QDocumentLine;
class QDocumentCursor;

class QMatcherInterface
{
	public:
		/*!
			\brief Empty destructor needed by the pure virtual callback...
		*/
		virtual ~QMatcherInterface() {}
		
		/*!
			\brief Matching callback
			
			\param c Current text cursor of the "calling editor"
			
			Parenthesis matching is done in this virtual function, according to
			the two characters before and after the cursor \a c
		*/
		virtual void match(const QDocumentCursor& c, QMatcher *m) = 0;
		
		/*!
			\return Wether a character is auto-closable
		*/
		virtual QChar autoClose(QChar c) = 0;
		
		/*!
			\brief Folding information callback
			
			\param b Text block to check
		*/
		virtual int blockFlags(int b, int depth, QMatcher *m) = 0;
		
		/*!
			\brief Folding callback
			
			\param b Text block to expand
		*/
		virtual void expand(int b, QMatcher *m) = 0;
		
		/*!
			\brief Folding callback
			
			\param b Text block to collapse
		*/
		virtual void collapse(int b, QMatcher *m) = 0;
};

#endif // _QMATCHER_INTERFACE_H_
