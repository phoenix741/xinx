/****************************************************************************
**
** Copyright (C) 2006-2009 fullmetalcoder <fullmetalcoder@hotmail.fr>
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

/*!
	\file qdocumentcursor.cpp
	\brief Implementation of the QDocumentCursor class
*/

#include "qdocumentcursor.h"

/*!
	\ingroup document
	@{
*/

#include "qdocument_p.h"

#include "qdocumentline.h"

/*!
	\class QDocumentCursor
	
	\brief A cursor to navigate within documents and edit them
	
	QDocumentCursor is a central class of the public API.
	
	It is the best (as in fastest and easiest) way to iterate over
	the content of a document.
	
	It is also the only class that allows to perform editing operations.
	
	A cursor position is defined by a line number and a text position
	within the line.
	
	Every cursor can have one or two cursor positions. In the later
	case, they delimit a selection. The first position set (before
	selecting) is referred to as the "anchor" and the other (if it
	is different from the anchor) is the actual cursor position.
*/

QDocumentCursor::QDocumentCursor(QDocument *doc)
 : m_handle(new QDocumentCursorHandle(doc))
{
	m_handle->ref();
}

QDocumentCursor::QDocumentCursor(const QDocumentCursor& cursor)
 : m_handle(0)
{
	if ( cursor.m_handle )
	{
		m_handle = cursor.m_handle->clone();
		m_handle->ref();
	}
}

QDocumentCursor::QDocumentCursor(QDocument *doc, int line, int column)
 : m_handle(new QDocumentCursorHandle(doc, line))
{
	m_handle->ref();
	
	m_handle->setColumnNumber(column);
}

/*
QDocumentCursor::QDocumentCursor(const QDocumentLine& line, int column)
 : m_handle(new QDocumentCursorHandle(line.document(), line.lineNumber()))
{
	m_handle->ref();
	
	m_handle->setColumnNumber(column);
	//movePosition(qMin(column, line.length()));
}
*/

QDocumentCursor::QDocumentCursor(QDocumentCursorHandle *handle)
 : m_handle(handle)
{
	if ( m_handle )
		m_handle->ref();
}

QDocumentCursor::~QDocumentCursor()
{
	if ( m_handle )
		m_handle->deref();
}

QDocumentCursor QDocumentCursor::clone() const
{
	return m_handle ? QDocumentCursor(m_handle->clone()) : QDocumentCursor();
}

QDocumentCursor& QDocumentCursor::operator = (const QDocumentCursor& c)
{
	#if 0
	if ( m_handle )
		m_handle->deref();
	
	m_handle = c.m_handle ? c.m_handle->clone() : 0;
	//m_handle = c.m_handle;
	
	if ( m_handle )
		m_handle->ref();
	#endif
	
	if ( c.m_handle )
	{
		if ( m_handle )
		{
			m_handle->copy(c.m_handle);
		} else {
			m_handle = c.m_handle->clone();
			m_handle->ref();
		}
	} else if ( m_handle ) {
		
		//qWarning("Setting a cursor to null");
		
		m_handle->deref();
		m_handle = 0;
	}
	
	return *this;
}

/*!
	\brief comparision operator
	
	\note If any of the operand is an invalid cursor, false is returned
*/
bool QDocumentCursor::operator == (const QDocumentCursor& c) const
{
	if ( !m_handle || !c.m_handle )
		return false;
	
	return m_handle->eq(c.m_handle);
}

/*!
	\brief comparision operator
	
	\note If any of the operand is an invalid cursor, true is returned (to preserve logical consistency with == )
*/
bool QDocumentCursor::operator != (const QDocumentCursor& c) const
{
	if ( !m_handle || !c.m_handle )
		return true;
	
	return !m_handle->eq(c.m_handle);
}

/*!
	\brief comparision operator
	
	\note If any of the operand is an invalid cursor, false is returned
*/
bool QDocumentCursor::operator < (const QDocumentCursor& c) const
{
	if ( !m_handle || !c.m_handle )
		return false;
	
	return m_handle->lt(c.m_handle);
}

/*!
	\brief comparision operator
	
	\note If any of the operand is an invalid cursor, false is returned
*/
bool QDocumentCursor::operator > (const QDocumentCursor& c) const
{
	if ( !m_handle || !c.m_handle )
		return false;
	
	return m_handle->gt(c.m_handle);
}

/*!
	\brief comparision operator
	
	\note If any of the operand is an invalid cursor, false is returned
*/
bool QDocumentCursor::operator <= (const QDocumentCursor& c) const
{
	if ( !m_handle || !c.m_handle )
		return false;
	
	return m_handle->gt(c.m_handle) || m_handle->eq(c.m_handle);
}

/*!
	\brief comparision operator
	
	\note If any of the operand is an invalid cursor, false is returned
*/
bool QDocumentCursor::operator >= (const QDocumentCursor& c) const
{
	if ( !m_handle || !c.m_handle )
		return false;
	
	return m_handle->lt(c.m_handle) || m_handle->eq(c.m_handle);
}

/*!
	\brief comparision operator
*/
bool QDocumentCursor::isNull() const
{
	return !m_handle || !m_handle->document();
}

/*!
	\brief comparision operator
*/
bool QDocumentCursor::isValid() const
{
	return m_handle && m_handle->document();
}

/*!
	\return whether the cursor is at the end of the document
*/
bool QDocumentCursor::atEnd() const
{
	return m_handle ? m_handle->atEnd() : false;
}

/*!
	\return whether the cursor is at the begining of the document
*/
bool QDocumentCursor::atStart() const
{
	return m_handle ? m_handle->atStart() : false;
}

/*!
	\return whether the cursor is at the end of a block
*/
bool QDocumentCursor::atBlockEnd() const
{
	return m_handle ? m_handle->atBlockEnd() : false;
}

/*!
	\return whether the cursor is at the start of a block
*/
bool QDocumentCursor::atBlockStart() const
{
	return m_handle ? m_handle->atBlockStart() : false;
}

/*!
	\return whether the cursor is at the end of a line
	
	\note this may only differ from atBlockEnd() on wrapped lines
*/
bool QDocumentCursor::atLineEnd() const
{
	return m_handle ? m_handle->atLineEnd() : false;
}

/*!
	\return whether the cursor is at the start of a line
	
	\note this may only differ from atBlockStart() on wrapped lines
*/
bool QDocumentCursor::atLineStart() const
{
	return m_handle ? m_handle->atLineStart() : false;
}

/*!
	\return the document on which this cursor operates
*/
QDocument* QDocumentCursor::document() const
{
	return m_handle ? m_handle->document() : 0;
}

/*!
	\return the text position (within the whole document) at which this cursor is
	
	\note available for compat with QTextCursor and ridiculously slow : avoid whenever possible
*/
int QDocumentCursor::position() const
{
	return m_handle ? m_handle->position() : -1;
}

/*!
	\return the text column of the anchor
*/
int QDocumentCursor::anchorColumn() const
{
	return m_handle ? m_handle->anchorColumn() : -1;
}

/*!
	\return the "visual" text column of the cursor
	
	\note this may only differ from columnNumber() when there are tabs on the line
*/
int QDocumentCursor::visualColumn() const
{
	return m_handle ? m_handle->visualColumn() : -1;
}

/*!
	\return the text column of the cursor
*/
int QDocumentCursor::columnNumber() const
{
	return m_handle ? m_handle->columnNumber() : -1;
}

/*!
	\brief Set the text column of the cursor
	\param c text column to set
	\param m move mode (determines whether text will be selected)
*/
void QDocumentCursor::setColumnNumber(int c, MoveMode m)
{
	if ( m_handle )
		m_handle->setColumnNumber(c, m);
}

/*!
	\return The line number to which the cursor points
*/
int QDocumentCursor::lineNumber() const
{
	return m_handle ? m_handle->lineNumber() : -1;
}

/*!
	\return the document position at which the cursor is
	
	Document position and viewport position are two terms used interchangeably.
	The only difference is the former refers to model perception (QDocument)
	whereas the later refers to view perception (QEditor)
*/
QPoint QDocumentCursor::documentPosition() const
{
	return m_handle ? m_handle->documentPosition() : QPoint();
}

/*!
	\return The line object at which this cursor points
*/
QDocumentLine QDocumentCursor::line() const
{
	return m_handle ? m_handle->line() : QDocumentLine();
}

/*!
	\brief Shift cursor position (text column) by a number of columns (characters)
*/
void QDocumentCursor::shift(int offset)
{
	if ( m_handle )
		m_handle->shift(offset);
}

/*!
	\brief Set the text position of the cursor (within the whole document)
	
	Remark made about position() applies.
*/
void QDocumentCursor::setPosition(int pos, MoveMode m)
{
	if ( m_handle )
		m_handle->setPosition(pos, m);
}

/*!
	\brief Moves the cursor position
	\param offset number of times the selected move will be done
	\param op movement type
	\param m movement mode (whether to select)
	\return true on succes
*/
bool QDocumentCursor::movePosition(int offset, MoveOperation op, MoveMode m)
{
	return m_handle ? m_handle->movePosition(offset, op, m) : false;
}

/*!
	\brief Jump to another cursor position
	\param line target line number
	\param colum target text column
*/
void QDocumentCursor::moveTo(int line, int column)
{
	if ( m_handle )
		m_handle->moveTo(line, column);
}

/*!
	\brief Jump to the position of another cursor
	\param c target cursor
*/
void QDocumentCursor::moveTo(const QDocumentCursor &c)
{
	if ( m_handle )
		m_handle->moveTo(c);
}

/*!
	\brief Jump to another cursor position
	\param l target line
	\param column target text column
	
	\note Calls QDocumentLine::lineNumber() => SLOW : avoid whenever possible
*/
void QDocumentCursor::moveTo(const QDocumentLine &l, int column)
{
	if ( m_handle )
		m_handle->moveTo(l.lineNumber(), column);
}

/*!
	\brief Delete the character at the position immediately after the cursor
*/
void QDocumentCursor::deleteChar()
{
	if ( m_handle )
		m_handle->deleteChar();
}

/*!
	\brief Delete the character at the position immediately before the cursor
*/
void QDocumentCursor::deletePreviousChar()
{
	if ( m_handle )
		m_handle->deletePreviousChar();
}

/*!
	\brief erase the whole line the cursor is on
*/
void QDocumentCursor::eraseLine()
{
	if ( m_handle )
		m_handle->eraseLine();
}

/*!
	\brief insert a new line at the cursor position
*/
void QDocumentCursor::insertLine()
{
	if ( m_handle )
		m_handle->insertText("\n");
}

/*!
	\brief insert some text at the cursor position
*/
void QDocumentCursor::insertText(const QString& s)
{
	if ( m_handle )
		m_handle->insertText(s);
}

/*!
	\return A cursor pointing at the position of the selection start.
	
	Selection start is the position of the selection that is nearest to document start.
	
	\note an invalid cursor is returned when the cursor does not have a selection
*/
QDocumentCursor QDocumentCursor::selectionStart() const
{
	return m_handle ? m_handle->selectionStart() : QDocumentCursor();
}

/*!
	\return A cursor pointing at the position of the selection end.
	
	Selection end is the position of the selection that is nearest to document end.
	
	\note an invalid cursor is returned when the cursor does not have a selection
*/
QDocumentCursor QDocumentCursor::selectionEnd() const
{
	return m_handle ? m_handle->selectionEnd() : QDocumentCursor();
}

/*!
	\return The selected text
*/
QString QDocumentCursor::selectedText() const
{
	return m_handle ? m_handle->selectedText() : QString();
}

/*!
	\brief Remove the selected text
*/
void QDocumentCursor::removeSelectedText()
{
	if ( m_handle )
		m_handle->removeSelectedText();
}

/*!
	\brief Begin an edit block
	
	Edit blocks are command groups. All the commands in an edit block
	are executed in a row when the edit block is ended with endEditBlock().
	
	Edit blocks are considered as a single command as far as the undo/redo
	stack is concerned.
	
	Edit blocks can be nested but that isn't of much use
*/
void QDocumentCursor::beginEditBlock()
{
	if ( m_handle )
		m_handle->beginEditBlock();
}

/*!
	\brief End an edit block
*/
void QDocumentCursor::endEditBlock()
{
	if ( m_handle )
		m_handle->endEditBlock();
}

/*!
	\return Whether the cursor is silent
*/
bool QDocumentCursor::isSilent() const
{
	return m_handle ? m_handle->isSilent() : true;
}

/*!
	\brief Set whether the cursor is silent
*/
void QDocumentCursor::setSilent(bool y)
{
	if ( m_handle )
		m_handle->setSilent(y);
	
}

/*!
	\return whether the cursor is auto updated
	
	An auto updated cursor will remain on the actual line it points
	to when the document is modified.
	
	\code
	QDocumentCursor c1(10, 0, document), c2(10, 0, document), c(5, 0, document);
	
	c1.setAutoUpdated(true);
	
	c.insertLine();
	
	// at this point c2 still points to line 10 whereas c1 points to line 11
	\endcode
*/
bool QDocumentCursor::isAutoUpdated() const
{
	return m_handle ? m_handle->isAutoUpdated() : true;
}

/*!
	\brief Set whether the cursor is aut updated
*/
void QDocumentCursor::setAutoUpdated(bool y)
{
	if ( m_handle )
		m_handle->setAutoUpdated(y);
	
}

/*!
	\brief Refresh the column memory of the cursor
	
	This set the current column memory to the current column position.
	
	\note It is not recommended to call that yourself. The various
	movement method should do that perfectly fine.
*/
void QDocumentCursor::refreshColumnMemory()
{
	if ( m_handle )
		m_handle->refreshColumnMemory();
	
}

/*!
	\return Whether the cursor has column memory
	
	The column memory is a feature that allow a cursor
	to remember its biggest column number so that moving
	back and forth (with movePosition()) on lines of
	different width does not result in the column to change.
	
*/
bool QDocumentCursor::hasColumnMemory() const
{
	return m_handle ? m_handle->hasColumnMemory() : false;
}

/*!
	\brief Set whether the cursor use column memory
*/
void QDocumentCursor::setColumnMemory(bool y)
{
	if ( m_handle )
		m_handle->setColumnMemory(y);
	
}

/*!
	\return whether the cursor has a selection
*/
bool QDocumentCursor::hasSelection() const
{
	return m_handle ? m_handle->hasSelection() : false;
}

/*!
	\brief clear the selection
*/
void QDocumentCursor::clearSelection()
{
	if ( m_handle )
		m_handle->clearSelection();
	
}

/*!
	\brief Select something
*/
void QDocumentCursor::select(SelectionType t)
{
	if ( m_handle )
		m_handle->select(t);
	
}

/*!
	\brief Set the selection boundary
*/
void QDocumentCursor::setSelectionBoundary(const QDocumentCursor& c)
{
	if ( m_handle )
		m_handle->setSelectionBoundary(c);
	
}

/*!
	\return whether a given cursor is within the selection
*/
bool QDocumentCursor::isWithinSelection(const QDocumentCursor& c) const
{
	return m_handle ? m_handle->isWithinSelection(c) : false;
}

/*!
	\return selection information
	
	\note The QDocumentSelection object is not updated if the selection
	changes later on : use it right away and do not store it.
*/
QDocumentSelection QDocumentCursor::selection() const
{
	QDocumentSelection s;
	
	if ( isNull() || !hasSelection() )
	{
		qDebug("NULL selection");
		
		s.startLine = -1;
		s.endLine = -1;
		
		s.start = -1;
		s.end = -1;
	} else if ( m_handle->m_begLine == m_handle->m_endLine ) {
		
		s.startLine = m_handle->m_begLine;
		s.endLine = m_handle->m_begLine;
		
		s.start = qMin(m_handle->m_begOffset, m_handle->m_endOffset);
		s.end = qMax(m_handle->m_begOffset, m_handle->m_endOffset);
		
	} else if ( m_handle->m_begLine > m_handle->m_endLine ) {
		
		s.startLine = m_handle->m_endLine;
		s.endLine = m_handle->m_begLine;
		
		s.start = m_handle->m_endOffset;
		s.end = m_handle->m_begOffset;
		
		//qDebug("[(%i,%i);(%i,%i)]", s.startLine.lineNumber(), s.start, s.endLine.lineNumber(), s.end);
	} else {
		s.startLine = m_handle->m_begLine;
		s.endLine = m_handle->m_endLine;
		
		s.start = m_handle->m_begOffset;
		s.end = m_handle->m_endOffset;
		
		//qDebug("[(%i,%i);(%i,%i)]", s.startLine.lineNumber(), s.start, s.endLine.lineNumber(), s.end);
	}
	
	return s;
}

/*! @} */
