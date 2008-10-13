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

#include "qlinemarkpanel.h"

/*!
	\file qlinemarkpanel.cpp
	\brief Implementation of the QLineMarkPanel class.
*/

#include "qeditor.h"
#include "qmarker.h"

#include "qdocument.h"
#include "qdocumentline.h"

#include "qlinemarksinfocenter.h"

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
QLineMarkPanel::QLineMarkPanel(QWidget *p)
 : QPanel(p)
{
	setFixedWidth(16);
}

/*!
	\brief Empty destructor
*/
QLineMarkPanel::~QLineMarkPanel()
{
	
}

/*!

*/
QString QLineMarkPanel::type() const
{
	return "Line marks";
}

/*!
	\internal
*/
void QLineMarkPanel::paint(QPainter *p, QEditor *e)
{
	if ( !e || !e->marker() || !e->document() )
		return;
	
	m_rects.clear();
	m_lines.clear();
	
	const QFontMetrics fm( e->document()->font() );
	
	int n, posY,
		maxCount = 0,
		as = fm.ascent(),
		ls = fm.lineSpacing(),
		pageBottom = e->viewport()->height(),
		contentsY = e->verticalScrollBar()->value();
	
	QString txt;
	const QFontMetrics sfm(fontMetrics());
	QLineMarksInfoCenter *mic = QLineMarksInfoCenter::instance();
	
	n = e->document()->lineNumber(contentsY);
	posY = 2 + e->document()->y(n) - contentsY;
	
	//qDebug("first = %i; last = %i", first, last);
	//qDebug("beg pos : %i", posY);
	//qDebug("<session>");
	for ( ; ; ++n )
	{
		//qDebug("n = %i; pos = %i", n, posY);
		QDocumentLine line = e->document()->line(n);
		
		if ( line.isNull() || ((posY - as) > pageBottom) )
			break;
		
		if ( line.isHidden() )
			continue;
		
		m_lines << n;
		m_rects << QRect(0, posY, width(), ls);
		
		int count = 4;
		
		foreach ( int id, line.marks() )
		{
			QPixmap pix = mic->markType(id).icon;
			
			//QPixmap pix = QPixmap("/home/frugal/mdr.png");
			
			if ( pix.isNull() )
				continue;
			
			int h = qMin(pix.height(), ls),
				w = qMin(pix.width(), ls),
				x = count,
				y = posY + ( (ls - h) >> 1 );
			
			p->drawPixmap(x, y, w, h, pix);
			
			count += w + 2;
		}
		
		if ( count > maxCount )
			maxCount = count;
		
		posY += ls;
	}
	//qDebug("</session>");
	
	//setFixedWidth(sfm.width(txt) + 5);
	
	if ( maxCount > 4 )
	{
		setFixedWidth(maxCount + 2);
	} else {
		setFixedWidth(16);
	}
}

/*!
	\internal
*/
void QLineMarkPanel::mousePressEvent(QMouseEvent *e)
{
// 	if ( !editor() || !editor()->document() || !editor()->marker() )
// 	{
// 		return QPanel::mousePressEvent(e);
// 	}
// 	
// 	e->accept();
	QPanel::mousePressEvent(e);
}

/*!
	\internal
*/
void QLineMarkPanel::mouseReleaseEvent(QMouseEvent *e)
{
	if ( !editor() || !editor()->document() || !editor()->marker() )
	{
		QPanel::mouseReleaseEvent(e);
		return;
	}
	
	QDocumentLine l;
	QMarker *m = editor()->marker();
	
	e->accept();
	
	for ( int i = 0; i < m_rects.count(); ++i )
	{
		if ( m_rects.at(i).contains(e->pos()) )
		{
			l = editor()->document()->line(m_lines.at(i));
			m->toggleDefaultMark(l, -1);
			
			break;
		}
	}
	
	QPanel::mouseReleaseEvent(e);
}

/*!
	\internal
*/
void QLineMarkPanel::contextMenuEvent(QContextMenuEvent *e)
{
	if ( !editor() || !editor()->document() || !editor()->marker() )
	{
		e->ignore();
		return;
	}
	
	/*
	QTextBlock b;
	QMarker *m = editor()->marker();
	QTextDocument *d = editor()->document();
	
	e->accept();
	
	QHash<int, QRect>::iterator i;
	
	for ( i = rects.begin(); i != rects.end(); i++ )
	{
		b = d->findBlock(i.key());
		
		if ( i->contains( e->pos() ) )
			return m->menu(b, e->globalPos());
	}
	*/
}

/*! @} */
