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

#include "qfoldpanel.h"

/*!
	\file qfoldpanel.cpp
	\brief Implementation of the QFoldPanel class.
	
	\see QFoldPanel
*/

#include "qeditor.h"
#include "qmatcher.h"

#include "qdocument.h"
#include "qdocumentline.h"

#include <QBitmap>
#include <QPainter>
#include <QScrollBar>
#include <QMouseEvent>

/*!
	\ingroup widgets
	@{
	
	\class QFoldPanel
*/

/*!
	\brief Constructor
*/
QFoldPanel::QFoldPanel(QWidget *p)
 :	QPanel(p)
{
	setFixedWidth(12);
}

/*!
	\brief Empty destructor
*/
QFoldPanel::~QFoldPanel()
{
	
}

/*!

*/
QString QFoldPanel::type() const
{
	return "Fold panel";
}

/*!

*/
void QFoldPanel::mousePressEvent(QMouseEvent *e)
{
	if ( !editor() || !editor()->matcher() || (e->button() != Qt::LeftButton) )
	{
		QPanel::mousePressEvent(e);
		return;
	}
	
	bool act = false;
	
	for ( int i = 0; i < m_rects.count(); ++i )
	{
		if ( !m_rects.at(i).contains(e->pos()) )
			continue;
		
		int ln = m_lines.at(i);
		QMatcher *m = editor()->matcher();
		QDocumentLine b = editor()->document()->line(ln);
		
		if ( b.hasFlag(QDocumentLine::CollapsedBlockStart) )
			m->expand(ln);
		else if ( m->blockFlags(ln, 0) == QMatcher::Collapsible )
			m->collapse(ln);
		
		act = true;
	}
	
	if ( act )
		editor()->setFocus();
	else
		QPanel::mousePressEvent(e);
	
}

/*!

*/
void QFoldPanel::paint(QPainter *p, QEditor *e)
{
	QMatcher *m = e->matcher();
	
	if ( !m )
		return;
	
	m_rects.clear();
	m_lines.clear();
	
	bool bVisible = false; //,
	//	inCursorBlock = false;
	
	QDocumentLine block;
	QDocument *d = e->document();
	const QFontMetrics fm(d->font());
	
	int n,
		pos,
		depth = 0,
		max = d->lines(),
		h = fm.height(),
		ls = fm.lineSpacing(),
		pageBottom = e->viewport()->height(),
		contentsY = e->verticalScrollBar()->value();
	
	
	pos =  - contentsY;
	
	//qDebug("beg pos : %i", pos);
	
	for ( n = 0; n < max; ++n )
	{
		if ( pos > pageBottom )
			break;
		
		block = d->line(n);
		
		if ( block.isHidden() )
		{
			continue;
		}
		
		bVisible = ((pos + ls) >= 0);
		int flags = m->blockFlags(n, depth);
		
		if ( flags & QMatcher::Closure )
		{
			if ( depth > 0 )
				depth -= flags & QMatcher::DataMask;
			//else
			//	qWarning("negative depth on line %i...", n);
			
			if ( !bVisible )
			{
				pos += ls;
				continue;
			}
			
			int mid = pos + 3 * ls / 4;
			
			p->drawLine(7, pos, 7, mid);
			p->drawLine(7, mid, 12, mid);
			
		} else if ( flags & QMatcher::Collapsed ) {
			
			if ( !bVisible )
			{
				pos += ls;
				continue;
			}
			
			// draw icon
			int bound = (ls - 8) / 2;
			
			if ( depth > 0 && bound > 0 )
			{
				p->drawLine(7, pos, 7, pos + bound);
				p->drawLine(7, pos + 8 + bound, 7, pos + ls);
			}
			
			m_lines << n;
			m_rects << drawIcon(p, e, 3, pos + bound, true);
			
			int sub = 1;
			
			while ( n < max )
			{
				++n;
				block = d->line(n);
				
				if ( !block.isHidden() )
				{
					--n;
					break;
				}
				
				if ( block.hasFlag(QDocumentLine::CollapsedBlockStart) )
					++sub;
				
				if ( block.hasFlag(QDocumentLine::CollapsedBlockEnd) )
				{
					flags = m->blockFlags(n, depth + 1);
					
					if ( flags & QMatcher::Closure )
					{
						sub -= (flags & QMatcher::DataMask);
						
						if ( depth && (sub < 0) )
						{
							depth += sub;
							
							//p->drawLine(7, pos + 8 + bound, 7, pos + ls);
							
							p->drawLine(7, pos + ls, 12, pos + ls);
						}
					}
				}
			}
			
			pos += ls;
			continue;
		} else if ( flags & QMatcher::Collapsible ) {
			++depth;
			
			if ( !bVisible )
			{
				pos += ls;
				continue;
			}
			
			// draw icon
			int bound = (ls - 8) / 2;
			
			if ( depth > 1 && bound > 0 )
				p->drawLine(7, pos, 7, pos + bound);
			
			m_lines << n;
			m_rects << drawIcon(p, e, 3, pos + bound, false);
			
			if ( bound > 0 )
				p->drawLine(7, pos + 8 + bound, 7, pos + ls);
			
			pos += ls;
			continue;
		}
		
		if ( bVisible && (depth > 0) )
			p->drawLine(7, pos, 7, pos + ls);
		
		pos += ls;
	}
}

QRect QFoldPanel::drawIcon(	QPainter *p, QEditor *,
							int x, int y, bool toExpand)
{
	QRect symbolRect(x, y, 8, 8);
	
	p->save();
	//p->setRenderHint(QPainter::Antialiasing);
	p->drawRect(symbolRect);
	p->restore();
	
	if ( toExpand )
	{
		p->drawLine(x + 2, y + 4, x + 6, y + 4);
		p->drawLine(x + 4, y + 2, x + 4, y + 6);
	} else {
		p->drawLine(x + 2, y + 4, x + 6, y + 4);
	}
	
	return symbolRect;
}

/*! @} */
