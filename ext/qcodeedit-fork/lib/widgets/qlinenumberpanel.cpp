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

#include "qlinenumberpanel.h"

/*!
	\file qlinenumberpanel.cpp
	\brief Implementation of the QLineNumberPanel class
	
	\see QLineNumberPanel
*/

#include <QPainter>
#include <QScrollBar>

#include "qeditor.h"
#include "qdocument.h"
#include "qdocumentline.h"


/*!
	\ingroup widgets
	@{
	
	\class QLineNumberPanel
	\brief A specific panel in charge of drawing line numbers of an editor
	
	\see QEditorInterface
*/

/*!
	\brief Constructor
*/
QLineNumberPanel::QLineNumberPanel(QWidget *p)
 : QPanel(p)
{
	setFixedWidth(20);
}

/*!
	\brief Empty destructor
*/
QLineNumberPanel::~QLineNumberPanel()
{
	
}

/*!

*/
QString QLineNumberPanel::type() const
{
	return "Line numbers";
}

/*!

*/
void QLineNumberPanel::editorChange(QEditor *e)
{
	if ( editor() )
	{
		disconnect(	editor(), SIGNAL( cursorPositionChanged() ),
					this	, SLOT  ( update() ) );
		
	}
	
	if ( e )
	{
		setFixedWidth(fontMetrics().width(QString::number(e->document()->lines())) + 5);
		
		connect(e	, SIGNAL( cursorPositionChanged() ),
				this, SLOT  ( update() ) );
		
	}
}

/*!

*/
void QLineNumberPanel::paint(QPainter *p, QEditor *e)
{
	QFont f(font());
	f.setWeight(QFont::Bold);
	const QFontMetrics sfm(f);
	const int panelWidth = sfm.width(QString::number(e->document()->lines())) + 5;
	setFixedWidth(panelWidth);
	
	const QFontMetrics fm( e->document()->font() );
	
	int n, posY,
		as = fm.ascent(),
		ls = fm.lineSpacing(),
		pageBottom = e->viewport()->height(),
		contentsY = e->verticalScrollBar()->value();
	
	QString txt;
	const int cursorLine = e->cursor().lineNumber();
	
	n = e->document()->lineNumber(contentsY);
	posY = as + 1 + e->document()->y(n) - contentsY;
	
	//qDebug("first = %i; last = %i", first, last);
	//qDebug("beg pos : %i", posY);
	
	for ( ; ; ++n )
	{
		//qDebug("n = %i; pos = %i", n, posY);
		QDocumentLine line = e->document()->line(n);
		
		if ( line.isNull() || ((posY - as) > pageBottom) )
			break;
		
		if ( line.isHidden() )
			continue;
		
		txt = QString::number(n + 1);
		
		if ( n == cursorLine )
		{
			p->save();
			QFont f = p->font();
			f.setWeight(QFont::Bold);
			
			p->setFont(f);
		}
		
		p->drawText(width() - 2 - sfm.width(txt),
					posY,
					txt);
		
		if ( n == cursorLine )
		{
			p->restore();
		}
		
		posY += ls;
	}
	
	//p->setPen(Qt::DotLine);
	//p->drawLine(width()-1, 0, width()-1, pageBottom);
	
	//setFixedWidth(sfm.width(txt) + 5);
}

/*! @} */
