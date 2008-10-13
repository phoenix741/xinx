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

#include "qmatcher.h"
#include "qmatcherinterface.h"

/*!
	\file qmatcher.cpp
	\brief Implementation of QMatcher
	
	\see QMatcher
*/

#include "qeditor.h"
#include "qdocument.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include "qformat.h"
#include "qformatfactory.h"

#include "qlanguagedefinition.h"

#include <QTimer>
#include <QApplication>

/*!
	\ingroup language
	@{
	
	\class QMatcher
	\brief Wrapper class that "connect" a text QEditor to a QMatcherInterface.
	
	\see QMatcherInterface
*/

/*!
	\brief Constructor
	
	\param i interface used when matching triggered
	\param d language definition used to create the interface
*/
QMatcher::QMatcher(QMatcherInterface *i, QLanguageDefinition *d)
: QObject(d), pInterface(i), locked(false), pDefinition(d)
{
	
}

/*!
	\return the text QEditor connected to this matcher
*/
QEditor* QMatcher::editor() const
{
	return pEdit;
}

/*!
	\brief Attach the matcher to a text QEditor
	
	\param e target QEditor
	
	\note Previously attached QEditor is detached.
*/
void QMatcher::setEditor(QEditor *e)
{
	if ( pEdit )
	{
		disconnect(	pEdit	, SIGNAL( cursorPositionChanged() ),
					this	, SLOT  ( cursorPositionChanged() ) );
		
	}
	
	pEdit = e;
	
	if ( pEdit )
	{
		connect(pEdit	, SIGNAL( cursorPositionChanged() ),
				this	, SLOT  ( cursorPositionChanged() ) );
		
	}
}

/*!
	\return the document used by this matcher through QEditor()
*/
QDocument* QMatcher::document() const
{
	return editor() ? editor()->document() : 0;
}

/*!
	\return the language definition that created this matcher
*/
QLanguageDefinition* QMatcher::definition() const
{
	return pDefinition;
}

/*!
	\brief Actual text of block \a b
	
	\param recursive Wether to to virtual unfolding recursively
*/
QChar QMatcher::autoClose(QChar c) const
{
	if ( pInterface )
		return pInterface->autoClose(c);
	
	return QChar();
}

/*!
	\brief expand a text block
*/
void QMatcher::expand(int b)
{
	if ( pInterface )
		pInterface->expand(b, this);
}

/*!
	\brief collapse a text block
*/
void QMatcher::collapse(int b)
{
	if ( pInterface )
		pInterface->collapse(b, this);
}

/*!
	\return the collapse state of a text block
*/
int QMatcher::blockFlags(int b, int depth)
{
	if ( pInterface )
		return pInterface->blockFlags(b, depth, this);
	
	return None;
}

/*!
	\brief Highlight the matched sequences
	
	\note Both position are BEFORE the matched characters (cursor position).
*/
void QMatcher::setMatch(int begline, int begpos, int beglen,
						int endline, int endpos, int endlen)
{
	QDocument *d = document();
	
	if ( !d )
		return;
	
	lBeg = begline;
	
	fBeg.offset = begpos;
	fBeg.length = beglen;
	fBeg.format = formatFactory()->id("match");
	
	d->line(lBeg).addOverlay(fBeg);
	
	lEnd = endline;
	fEnd.offset = endpos;
	fEnd.length = endlen;
	fEnd.format = formatFactory()->id("match");
	
	d->line(lEnd).addOverlay(fEnd);
}

/*!
	\internal
*/
void QMatcher::cursorPositionChanged()
{
	if ( !pEdit || !pInterface || locked )
		return;
	
	locked = true;
	
	//QTimer::singleShot(10, this, SLOT( match() ));
	match();
}

/*!
	\internal
*/
void QMatcher::match()
{
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("Matching...");
	#endif
	
	QDocument *d = document();
	
	if ( lBeg != -1 )
	{
		d->formatsChange(lBeg, 1);
		d->line(lBeg).removeOverlay(fBeg);
	}
	
	if ( lEnd != -1 )
	{
		d->formatsChange(lEnd, 1);
		d->line(lEnd).removeOverlay(fEnd);
	}
	
	lBeg = lEnd = -1;
	
	if ( editor() )
		pInterface->match(editor()->cursor(), this);
	
	if ( lBeg != -1 )
	{
		d->formatsChange(lBeg, 1);
	}
	
	if ( lEnd != -1 )
	{
		d->formatsChange(lEnd, 1);
	}
	
	#ifdef _QCODE_EDIT_DEBUG_
	qDebug("Matched");
	#endif
	
	locked = false;
}

/*! @} */
