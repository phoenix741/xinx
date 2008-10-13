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

#include "qdocumentline.h"

#include "qdocument_p.h"

//QList<QDocumentLine*> QDocumentLine::m_lines_backing_store;

QDocumentLine::QDocumentLine(QDocument *doc)
 : m_handle(doc ? doc->impl()->at(0) : 0)
{
	//m_lines_backing_store << this;
	
	if ( m_handle )
		m_handle->ref();
	
}

QDocumentLine::QDocumentLine(const QDocumentLine& line)
 : m_handle(line.m_handle)
{
	//m_lines_backing_store << this;
	if ( m_handle )
		m_handle->ref();
	
}

QDocumentLine::QDocumentLine(QDocumentLineHandle* handle)
 : m_handle(handle)
{
	//m_lines_backing_store << this;
	if ( m_handle )
		m_handle->ref();
	
}

QDocumentLine::~QDocumentLine()
{
	if ( m_handle )
		m_handle->deref();
	
	//m_lines_backing_store.removeAll(this);
}

bool QDocumentLine::operator == (const QDocumentLine& l) const
{
	return m_handle == l.m_handle;
	//return lineNumber() == l.lineNumber();
}

bool QDocumentLine::operator != (const QDocumentLine& l) const
{
	return m_handle != l.m_handle;
	//return lineNumber() != l.lineNumber();
}

bool QDocumentLine::operator < (const QDocumentLine& l) const
{
	return lineNumber() < l.lineNumber();
}

bool QDocumentLine::operator >= (const QDocumentLine& l) const
{
	return lineNumber() >= l.lineNumber();
}

bool QDocumentLine::operator > (const QDocumentLine& l) const
{
	return lineNumber() > l.lineNumber();
}

bool QDocumentLine::operator <= (const QDocumentLine& l) const
{
	return lineNumber() <= l.lineNumber();
}

QDocumentLine& QDocumentLine::operator ++ ()
{
	operator = (next());
	return *this;
}

QDocumentLine& QDocumentLine::operator -- ()
{
	operator = (previous());
	return *this;
}

void QDocumentLine::operator ++ (int)
{
	operator = (next());
}

void QDocumentLine::operator -- (int)
{
	operator = (previous());
}

QDocumentLine& QDocumentLine::operator = (const QDocumentLine& l)
{
	if ( m_handle )
		m_handle->deref();
	
	m_handle = l.m_handle;
	
	if ( m_handle )
		m_handle->ref();
	
	return *this;
}

QDocument* QDocumentLine::document() const
{
	return m_handle ? m_handle->document() : 0;
}

int QDocumentLine::lineNumber() const
{
	return m_handle ? m_handle->line() : -1;
}

int QDocumentLine::position() const
{
	return m_handle ? m_handle->position() : -1;
}

bool QDocumentLine::hasFlag(State s) const
{
	return m_handle ? m_handle->hasFlag(s) : false;
}

void QDocumentLine::setFlag(State s, bool y)
{
	if ( m_handle )
		m_handle->setFlag(s, y);
	
}

bool QDocumentLine::isNull() const
{
	if ( m_handle )
		return !m_handle->document();
	
	return true;
}

bool QDocumentLine::isValid() const
{
	if ( m_handle )
		return m_handle->document();
	
	return false;
}

QString QDocumentLine::text() const
{
	return m_handle ? m_handle->text() : QString();
}

int QDocumentLine::length() const
{
	return m_handle ? m_handle->text().length() : 0;
}

/*!
	\brief Returns the position of the first non-whitespace character
	\return position of first non-whitespace char or -1 if there is none
*/
int QDocumentLine::firstChar() const
{
	return nextNonSpaceChar(0);
}

/*!
	\brief Returns the position of the last non-whitespace character
	\return position of last non-whitespace char or -1 if there is none
*/
int QDocumentLine::lastChar() const
{
	return previousNonSpaceChar(length() - 1);
}

/*!
	Find the position of the next char that is not a space.
	\param pos Column of the character which is examined first.
	\return True if the specified or a following character is not a space
	Otherwise false.
*/
int QDocumentLine::nextNonSpaceChar(int pos) const
{
	return m_handle ? m_handle->nextNonSpaceChar(pos) : -1;
}

/*!
	\brief Find the position of the previous char that is not a space.
	\param pos Column of the character which is examined first.
	\return The position of the first non-whitespace character preceding pos,
   or -1 if none is found.
*/
int QDocumentLine::previousNonSpaceChar(int pos) const
{
	return m_handle ? m_handle->previousNonSpaceChar(pos) : -1;
}

QDocumentLine QDocumentLine::next() const
{
	return QDocumentLine(m_handle->next());
}

QDocumentLine QDocumentLine::previous() const
{
	return QDocumentLine(m_handle->previous());
}

int QDocumentLine::cursorToX(int cpos) const
{
	return m_handle ? m_handle->cursorToX(cpos) : -1;
}

int QDocumentLine::xToCursor(int xpos, int xoff) const
{
	return m_handle ? m_handle->xToCursor(xpos, xoff) : -1;
}

void QDocumentLine::addMark(int id)
{
	if ( !document() )
		return;
	
	document()->impl()->addMark(m_handle, id);
}

void QDocumentLine::toggleMark(int id)
{
	if ( !document() )
		return;
	
	document()->impl()->toggleMark(m_handle, id);
}

void QDocumentLine::removeMark(int id)
{
	if ( !document() )
		return;
	
	document()->impl()->removeMark(m_handle, id);
}

QList<int> QDocumentLine::marks() const
{
	return document() ? document()->impl()->marks(m_handle) : QList<int>();
}

void QDocumentLine::setFormats(const QVector<quint8>& formats)
{
	if ( !m_handle )
		return;
	
	m_handle->setFormats(formats);
}

void QDocumentLine::clearOverlays()
{
	if ( !m_handle )
		return;
	
	m_handle->clearOverlays();
}

void QDocumentLine::addOverlay(const QFormatRange& over)
{
	if ( !m_handle )
		return;
	
	m_handle->addOverlay(over);
}

void QDocumentLine::removeOverlay(const QFormatRange& over)
{
	if ( !m_handle )
		return;
	
	m_handle->removeOverlay(over);
}

const QVector<QParenthesis>& QDocumentLine::parentheses() const
{
	Q_CHECK_PTR(m_handle);
	
	return m_handle->m_parens;
}

void QDocumentLine::setParentheses(const QVector<QParenthesis>& parentheses)
{
	if ( !m_handle )
		return;
	
	m_handle->m_parens = parentheses;
}

QNFAMatchContext* QDocumentLine::matchContext()
{
	return m_handle ? &m_handle->m_context : 0;
}
