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

#ifndef _QINDENTER_INTERFACE_H_
#define _QINDENTER_INTERFACE_H_

/*!
	\file qindenterinterface.h
	\brief Definition of the QIndenterInterface class.
	
	\see QIndenterInterface
*/

/*!
	\class QIndenterInterface
	\brief The indenter itself
	
	This pure virtual classes features only one function that is called by
	attached matchers when needed.
	
	Note : As other interfaces, QIndenterInterface is "editor-independent", one
	interface only is created by extension and several matchers may share an
	interface, so do NOT store data in any other place that a QBlockData...
	
	\see QIndenter
	\see QBlockData
*/

class QIndenter;

class QKeyEvent;
class QDocumentCursor;

class QIndenterInterface
{
	public:
		/*!
			\brief Empty destructor needed by the pure virtual callback...
		*/
		virtual ~QIndenterInterface() {}
		
		/*!
			\brief Indenting callback
			
			\param k Key event that "caused" the callback 
			\param c Current text cursor of the "calling editor"
			
			Text indentation is done in this virtual function, according to
			the key event \a k and the cursor \a c
		*/
		//virtual bool indent(QKeyEvent *k, QDocumentCursor& c, QIndenter *i) = 0;
		
		virtual QString indent(const QDocumentCursor& c, QIndenter *i) = 0;
		virtual bool unindent (const QDocumentCursor& c, QKeyEvent *k) = 0;
};

#endif // _QINDENTER_INTERFACE_H_
