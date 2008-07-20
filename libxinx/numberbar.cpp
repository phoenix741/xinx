/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "numberbar.h"

// Qt header
#include <QAbstractTextDocumentLayout>
#include <QTextEdit>
#include <QPaintEvent>
#include <QTextDocumentFragment>
#include <QScrollBar>
#include <QPainter>
#include <QTextBlock>

NumberBar::NumberBar( QWidget *parent ) : QWidget( parent ), m_edit(0) {
	setFixedWidth( fontMetrics().width( QString("0000") ) + 32 );
}

NumberBar::~NumberBar() {
}

void NumberBar::setTextEdit( QTextEdit *edit ) {
	this->m_edit = edit;
	connect( m_edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ), this, SLOT( update() ) );
	connect( m_edit->verticalScrollBar(), SIGNAL( valueChanged(int) ), this, SLOT( update() ) );
	connect( m_edit, SIGNAL( cursorPositionChanged() ), this, SLOT( update() ) );
}

void NumberBar::paintEvent( QPaintEvent * event ) {
	Q_UNUSED( event );

	QAbstractTextDocumentLayout *layout = m_edit->document()->documentLayout();
	const QFontMetrics fm = fontMetrics();
	QPainter p(this);

	QTextBlock currentBlock = m_edit->textCursor().block();
	int contentsY = m_edit->verticalScrollBar()->value();
	qreal pageBottom = contentsY + m_edit->viewport()->height();
	const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
	int lineCount = 1;

	for ( QTextBlock block = m_edit->document()->begin(); block.isValid(); block = block.next(), ++lineCount ) {
		const QRectF boundingRect = layout->blockBoundingRect( block );

		QPointF position = boundingRect.topLeft();
		if ( position.y() + boundingRect.height() < contentsY ) continue;
		if ( position.y() > pageBottom ) break;

		const QString txt = QString::number( lineCount );
		if( block == currentBlock ) {
			QFont f = p.font();
			f.setBold( true );
			p.setFont( f );
		}
		p.drawText( width() - fm.width(txt) - 7, qRound( position.y() ) - contentsY + ascent, txt );
		if( block == currentBlock ) {
			QFont f = p.font();
			f.setBold( false );
			p.setFont( f );
	 	}
		
		if( m_errors.contains( lineCount ) ) {
			p.drawPixmap( 3 + 20, qRound( position.y() ) -contentsY-6,QPixmap(":/images/warning.png").scaled(20,20));
		}
		if( m_lineBookmark.contains( lineCount ) ) {
			p.drawPixmap( 3, qRound( position.y() ) -contentsY-6,QPixmap(":/images/bookmark.png").scaled(20,20));
		}
	}
}

void NumberBar::mousePressEvent ( QMouseEvent * event ) {
	QTextCursor cursor = m_edit->cursorForPosition( event->pos() );
	if( cursor.isNull() ) return;

	int currentLine = 1;
	for ( QTextBlock block = m_edit->document()->begin(); block.isValid() && block != cursor.block(); block = block.next(), currentLine++ ) ;

	if( event->button() == Qt::LeftButton )	{
		setBookmark( currentLine, !m_lineBookmark.contains( currentLine ) );
	}
}

void NumberBar::setBookmark( int line, bool enabled ) {
	int index = m_lineBookmark.indexOf( line );
	if( enabled && index < 0  ) {
		m_lineBookmark.append( line );
		qSort( m_lineBookmark );
	} else if( !enabled && index >= 0 ) {
		m_lineBookmark.removeAt( index );
	}
	repaint();
	emit bookmarkToggled( line, enabled );
}

QList<int> & NumberBar::listOfBookmark() {
	return m_lineBookmark;
}

void NumberBar::setErrors( QList<int> errors ) {
	if( errors != m_errors ) {
		m_errors = errors;
		repaint();
	}
}