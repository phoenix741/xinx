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

#include "qlinemarkpanel.h"

/*!
	\file qlinemarkpanel.cpp
	\brief Implementation of the QLineMarkPanel class.
*/

#include "qeditor.h"

#include "qdocument.h"
#include "qdocumentline.h"

#include "qlanguagedefinition.h"
#include "qlinemarksinfocenter.h"

#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QMessageBox>
#include <QContextMenuEvent>

/*!
	\ingroup widgets
	@{
*/

/*!
	\class QLineMarkPanel
	\brief A specific panel in charge of drawing line marks of an editor
*/

QCE_AUTO_REGISTER(QLineMarkPanel)

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
	if ( !e || !e->document() )
		return;

	m_rects.clear();
	m_lines.clear();
	QDocument *d = e->document();

	int maxMarksPerLine = d->maxMarksPerLine();

	setFixedWidth(maxMarksPerLine ? maxMarksPerLine * 16 + 2 : 16);

	const QFontMetrics fm( d->font() );

	int n, posY,
		maxCount = 0,
		as = fm.ascent(),
		ls = fm.lineSpacing(),
		pageBottom = e->viewport()->height(),
		contentsY = e->verticalScrollBar()->value();

	QString txt;
	const QFontMetrics sfm(fontMetrics());
	QLineMarksInfoCenter *mic = QLineMarksInfoCenter::instance();

	n = d->lineNumber(contentsY);
	posY = 2 + d->y(n) - contentsY;

	//qDebug("first = %i; last = %i", first, last);
	//qDebug("beg pos : %i", posY);
	//qDebug("<session>");
	for ( ; ; ++n )
	{
		//qDebug("n = %i; pos = %i", n, posY);
		QDocumentLine line = d->line(n);

		if ( line.isNull() || ((posY - as) > pageBottom) )
			break;

		if ( line.isHidden() )
			continue;

		m_lines << n;
		m_rects << QRect(0, posY, width(), ls);

		if ( maxMarksPerLine )
		{
			int count = 1;
			QList<int> lm = line.marks();

			foreach ( int id, lm )
			{
				QPixmap pix = mic->markType(id).icon;

				if ( pix.isNull() )
					continue;

				int h = qMin(pix.height(), ls),
					w = qMin(pix.width(), 16),
					x = count,
					y = posY + ( (ls - h) >> 1 );

				p->drawPixmap(x, y, w, h, pix);

				count += 16;
			}
		}

		posY += ls * line.lineSpan();
	}
	//qDebug("</session>");

	//setFixedWidth(sfm.width(txt) + 5);
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
	QPanel::mousePressEvent(e);
 	e->accept();
}

/*!
	\internal
*/
void QLineMarkPanel::mouseReleaseEvent(QMouseEvent *e)
{
	if ( !editor() || !editor()->document() || !editor()->languageDefinition() )
	{
		QPanel::mouseReleaseEvent(e);
		return;
	}

	//QMessageBox::warning(0, 0, "clik.");

	QDocumentLine l;
	QLanguageDefinition *d = editor()->languageDefinition();
	const int id = QLineMarksInfoCenter::instance()->markTypeId(d->defaultLineMark());
	
	if ( id < 0 )
		return;
	
	e->accept();

	for ( int i = 0; i < m_rects.count(); ++i )
	{
		if ( m_rects.at(i).contains(e->pos()) )
		{
			l = editor()->document()->line(m_lines.at(i));
			l.toggleMark(id);
			//m->toggleDefaultMark(l, -1);

			break;
		}
	}

	QPanel::mouseReleaseEvent(e);
}

/*!
	\internal
*/
void QLineMarkPanel::actionBookmarkTriggered()
{
	QAction * s = qobject_cast<QAction*>( sender() );
	if( s && m_currentLine.isValid() ) {
		m_currentLine.toggleMark( QLineMarksInfoCenter::instance()->markTypeId( s->text() ) );
	}
}

/*!
	\internal
*/
void QLineMarkPanel::contextMenuEvent(QContextMenuEvent *e)
{
	if ( !editor() || !editor()->document() )
	{
		e->ignore();
		return;
	}

	m_currentLine = QDocumentLine();
	for ( int i = 0; i < m_rects.count(); ++i )
	{
		if ( m_rects.at(i).contains(e->pos()) )
		{
			m_currentLine = editor()->document()->line(m_lines.at(i));
			break;
		}
	}

	QMenu * menu = new QMenu( editor() );

	QStringList marks = QLineMarksInfoCenter::instance()->availableMarkTypes();
	foreach( QString mark, marks )
	{
		QLineMarkType type = QLineMarksInfoCenter::instance()->markType( mark );
		if( type.user )
			menu->addAction( type.icon, mark, this, SLOT(actionBookmarkTriggered()) );
	}

	menu->exec( e->globalPos() );
	delete menu;
	m_currentLine = QDocumentLine();

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