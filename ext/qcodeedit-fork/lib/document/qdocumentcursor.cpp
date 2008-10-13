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

#include "qdocumentcursor.h"

#include "qdocument_p.h"

#include "qdocumentline.h"

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

bool QDocumentCursor::operator == (const QDocumentCursor& c) const
{
	return m_handle->eq(c.m_handle);
}

bool QDocumentCursor::operator != (const QDocumentCursor& c) const
{
	return !m_handle->eq(c.m_handle);
}

bool QDocumentCursor::operator < (const QDocumentCursor& c) const
{
	return m_handle->lt(c.m_handle);
}

bool QDocumentCursor::operator > (const QDocumentCursor& c) const
{
	return m_handle->gt(c.m_handle);
}

bool QDocumentCursor::operator <= (const QDocumentCursor& c) const
{
	return m_handle->gt(c.m_handle) || m_handle->eq(c.m_handle);
}

bool QDocumentCursor::operator >= (const QDocumentCursor& c) const
{
	return m_handle->lt(c.m_handle) || m_handle->eq(c.m_handle);
}

bool QDocumentCursor::isNull() const
{
	return !m_handle || !m_handle->document();
}

bool QDocumentCursor::isValid() const
{
	return m_handle && m_handle->document();
}

bool QDocumentCursor::atEnd() const
{
	return m_handle ? m_handle->atEnd() : false;
}

bool QDocumentCursor::atStart() const
{
	return m_handle ? m_handle->atStart() : false;
}

bool QDocumentCursor::atBlockEnd() const
{
	return m_handle ? m_handle->atBlockEnd() : false;
}

bool QDocumentCursor::atBlockStart() const
{
	return m_handle ? m_handle->atBlockStart() : false;
}

bool QDocumentCursor::atLineEnd() const
{
	return m_handle ? m_handle->atLineEnd() : false;
}

bool QDocumentCursor::atLineStart() const
{
	return m_handle ? m_handle->atLineStart() : false;
}

QDocument* QDocumentCursor::document() const
{
	return m_handle ? m_handle->document() : 0;
}

int QDocumentCursor::position() const
{
	return m_handle ? m_handle->position() : -1;
}

int QDocumentCursor::anchorColumn() const
{
	return m_handle ? m_handle->anchorColumn() : -1;
}

int QDocumentCursor::visualColumn() const
{
	return m_handle ? m_handle->visualColumn() : -1;
}

int QDocumentCursor::columnNumber() const
{
	return m_handle ? m_handle->columnNumber() : -1;
}

void QDocumentCursor::setColumnNumber(int c, MoveMode m)
{
	if ( m_handle )
		m_handle->setColumnNumber(c, m);
}

int QDocumentCursor::lineNumber() const
{
	return m_handle ? m_handle->lineNumber() : -1;
}

QDocumentLine QDocumentCursor::line() const
{
	return m_handle ? m_handle->line() : QDocumentLine();
}

void QDocumentCursor::shift(int offset)
{
	if ( m_handle )
		m_handle->shift(offset);
}

void QDocumentCursor::setPosition(int pos, MoveMode m)
{
	if ( m_handle )
		m_handle->setPosition(pos, m);
}

bool QDocumentCursor::movePosition(int offset, MoveOperation op, MoveMode m)
{
	return m_handle ? m_handle->movePosition(offset, op, m) : false;
}

void QDocumentCursor::moveTo(int line, int column)
{
	if ( m_handle )
		m_handle->moveTo(line, column);
}

void QDocumentCursor::moveTo(const QDocumentCursor &c)
{
	if ( m_handle )
		m_handle->moveTo(c);
}

void QDocumentCursor::moveTo(const QDocumentLine &l, int column)
{
	if ( m_handle )
		m_handle->moveTo(l.lineNumber(), column);
}

void QDocumentCursor::deleteChar()
{
	if ( m_handle )
		m_handle->deleteChar();
}

void QDocumentCursor::deletePreviousChar()
{
	if ( m_handle )
		m_handle->deletePreviousChar();
}

void QDocumentCursor::insertLine()
{
	if ( m_handle )
		m_handle->insertText("\n");
}

void QDocumentCursor::insertText(const QString& s)
{
	if ( m_handle )
		m_handle->insertText(s);
}

QDocumentCursor QDocumentCursor::selectionStart() const
{
	return m_handle ? m_handle->selectionStart() : QDocumentCursor();
}

QDocumentCursor QDocumentCursor::selectionEnd() const
{
	return m_handle ? m_handle->selectionEnd() : QDocumentCursor();
}

QString QDocumentCursor::selectedText() const
{
	return m_handle ? m_handle->selectedText() : QString();
}

void QDocumentCursor::removeSelectedText()
{
	if ( m_handle )
		m_handle->removeSelectedText();
}

void QDocumentCursor::beginEditBlock()
{
	if ( m_handle )
		m_handle->beginEditBlock();
}

void QDocumentCursor::endEditBlock()
{
	if ( m_handle )
		m_handle->endEditBlock();
}

bool QDocumentCursor::isSilent() const
{
	return m_handle ? m_handle->isSilent() : true;
}

void QDocumentCursor::setSilent(bool y)
{
	if ( m_handle )
		m_handle->setSilent(y);
	
}

bool QDocumentCursor::isAutoUpdated() const
{
	return m_handle ? m_handle->isAutoUpdated() : true;
}

void QDocumentCursor::setAutoUpdated(bool y)
{
	if ( m_handle )
		m_handle->setAutoUpdated(y);
	
}

bool QDocumentCursor::hasSelection() const
{
	return m_handle ? m_handle->hasSelection() : false;
}

void QDocumentCursor::clearSelection()
{
	if ( m_handle )
		m_handle->clearSelection();
	
}

void QDocumentCursor::select(SelectionType t)
{
	if ( m_handle )
		m_handle->select(t);
	
}

void QDocumentCursor::setSelectionBoundary(const QDocumentCursor& c)
{
	if ( m_handle )
		m_handle->setSelectionBoundary(c);
	
}

bool QDocumentCursor::isWithinSelection(const QDocumentCursor& c) const
{
	return m_handle ? m_handle->isWithinSelection(c) : false;
}

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
