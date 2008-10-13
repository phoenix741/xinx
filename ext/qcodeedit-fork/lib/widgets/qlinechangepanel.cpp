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

#include "qlinechangepanel.h"

/*!
	\file qlinechangepanel.cpp
	\brief Implementation of the QLineChangePanel class.
*/

#include "qeditor.h"

#include "qdocument.h"
#include "qdocumentline.h"

#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QContextMenuEvent>

/*!
	\ingroup widgets
	@{
	
	\class QLineMarkPanel
	\brief A specific panel in charge of drawing line numbers of an editor
	
	\see QEditorInterface
*/

/*!
	\brief Constructor
*/
QLineChangePanel::QLineChangePanel(QWidget *p)
 : QPanel(p)
{
	setFixedWidth(4);
}

/*!
	\brief Empty destructor
*/
QLineChangePanel::~QLineChangePanel()
{
	
}

/*!

*/
QString QLineChangePanel::type() const
{
	return "Line changes";
}

/*!
	\internal
*/
void QLineChangePanel::paint(QPainter *p, QEditor *e)
{
	if ( !e || !e->marker() || !e->document() )
		return;
	
	const QFontMetrics fm( e->document()->font() );
	
	int n, posY,
		maxCount = 0,
		as = fm.ascent(),
		ls = fm.lineSpacing(),
		pageBottom = e->viewport()->height(),
		contentsY = e->verticalScrollBar()->value();
	
	QString txt;
	
	n = e->document()->lineNumber(contentsY);
	posY = 2 + e->document()->y(n) - contentsY;
	
	QPixmap m_pix(2, ls);
	m_pix.fill(Qt::red);
	
	for ( ; ; ++n )
	{
		//qDebug("n = %i; pos = %i", n, posY);
		QDocumentLine line = e->document()->line(n);
		
		if ( line.isNull() || ((posY - as) > pageBottom) )
			break;
		
		if ( line.isHidden() )
			continue;
		
		
		if ( e->document()->isLineModified(line) )
		{
			int h = qMin(m_pix.height(), ls),
				w = 2,
				x = 1,
				y = posY + ( (ls - h) >> 1 );
			
			p->drawPixmap(x, y, w, h, m_pix);
		}
		
		posY += ls;
	}
}

/*! @} */