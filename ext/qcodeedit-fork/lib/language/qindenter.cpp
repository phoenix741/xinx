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

#include "qindenter.h"
#include "qindenterinterface.h"

/*!
	\file qindenter.cpp
	\brief Implementation of QIndenter
	
	\see QIndenter
*/

/*!
	\ingroup language
	@{
	
	\class QIndenter
	\brief Wrapper class that "connect" a text editor to a QIndenterInterface.
	
	\see QIndenterInterface
*/

#include <QKeyEvent>
#include <QTextCursor>

#include "qeditor.h"
#include "qlanguagedefinition.h"

/*!
	\brief Constructor
	
	\param i interface used when indenting triggered
	\param d language definition used to create the interface
*/
QIndenter::QIndenter(QIndenterInterface *i, QLanguageDefinition *d)
 : QObject(d), pInterface(i), pDefinition(d)
{
}

/*!
	\return the text editor connected to this matcher
*/
QEditor* QIndenter::editor() const
{
	return pEdit;
}

/*!
	\brief Attach the indenter to a text editor
	
	\param e target editor
	
	\note Previously attached editor is detached.
*/
void QIndenter::setEditor(QEditor *e)
{
	if ( pEdit )
	{
		/*
		pEdit->removeEventFilter(this);
		
		disconnect(	pEdit, SIGNAL( clearAutoCloseStack() ),
					this , SLOT  ( clearAutoCloseStack() ) );
		
		*/
	}
	
	pEdit = e;
	
	if ( pEdit )
	{
		/*
		pEdit->installEventFilter(this);
		
		connect(pEdit, SIGNAL( clearAutoCloseStack() ),
				this , SLOT  ( clearAutoCloseStack() ) );
		
		*/
	}
}

/*!
	\return the document used by this matcher through editor() or
	editorinterface()
*/
QDocument* QIndenter::document() const
{
	if ( editor() )
		return editor()->document();
	
	return 0;
}

/*!
	\return the language definition that created this matcher
*/
QLanguageDefinition* QIndenter::definition() const
{
	return pDefinition;
}

QString QIndenter::indent(const QDocumentCursor& c)
{
	return pInterface ? pInterface->indent(c, this) : QString();
}

bool QIndenter::unindent (const QDocumentCursor& c, QKeyEvent *k)
{
	return pInterface ? pInterface->unindent(c, k) : false;
}

/*!
	\internal
*/
bool QIndenter::eventFilter(QObject *o, QEvent *e)
{
	if ( !e || !o || e->type() != QEvent::KeyPress || o != pEdit )
		return false;
	
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("Indenting...");
	#endif
	/*
	QTextCursor c;
	
	if ( editor() )
		c = editor()->textCursor();
	
	QKeyEvent *k = static_cast<QKeyEvent*>(e);
	
	bool b = pInterface ? pInterface->indent(k, c, this) : false;
	
	if ( editor() && b )
		editor()->setTextCursor(c);
	
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("Indented=%i!", (int)b);
	#endif
	
	return b;
	*/
	return false;
}

/*! @} */
