/* This file is part of the KDE libraries

   Copyright (C) 1997 Sven Radej (sven.radej@iname.com)
   Copyright (c) 1999 Patrick Ward <PAT_WARD@HP-USA-om5.om.hp.com>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>

   Re-designed for KDE 2.x by
   Copyright (c) 2000, 2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (c) 2000, 2001 Carsten Pfeiffer <pfeiffer@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License (LGPL) as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later
   version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
/*
 * Ulrich Van Den Hekke ( 21/03/2007 ) Simplified version of KLineEdit from KDE4.
 */

#include "xinxlineedit.h"

#include <QStyleOption>
#include <QApplication>

/* XinxLineEditButton */

XinxLineEditButton::XinxLineEditButton(QWidget *parent) : QWidget(parent)
{
}

QSize XinxLineEditButton::sizeHint() const
{
	return m_pixmap.size();
}

void XinxLineEditButton::setPixmap(const QPixmap & p)
{
	m_pixmap = p;
}

QPixmap XinxLineEditButton::pixmap()
{
	return m_pixmap;
}

void XinxLineEditButton::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter p(this);
	p.drawPixmap((width() - m_pixmap.width()) / 2, (height() - m_pixmap.height()) / 2, m_pixmap);
}


/* XinxLineEdit */

XinxLineEdit::XinxLineEdit(const QString &string, QWidget *parent) : QLineEdit(string, parent)
{
	init();
}

XinxLineEdit::XinxLineEdit(QWidget *parent) : QLineEdit(parent)
{
	init();
}


XinxLineEdit::~XinxLineEdit()
{

}

void XinxLineEdit::init()
{
	clearButton = 0;
	clickInClear = false;
	wideEnoughForClear = true;

	clearButton = new XinxLineEditButton(this);
	clearButton->setCursor(Qt::ArrowCursor);

	updateClearButtonIcon(text());
	//updateClearButton();
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(updateClearButtonIcon(QString)));
}

void XinxLineEdit::updateClearButtonIcon(const QString& text)
{
	if (isReadOnly())
	{
		return;
	}

	if (text.length() > 0)
	{
		clearButton->setVisible(true);
	}
	else
	{
		clearButton->setVisible(false);
	}

	if (! clearButton->pixmap().isNull())
	{
		return;
	}

	if (layoutDirection() == Qt::LeftToRight)
	{
		clearButton->setPixmap(QPixmap(":/images/edit-clear-locationbar-rtl.png"));
	}
	else
	{
		clearButton->setPixmap(QPixmap(":/images/edit-clear-locationbar-ltr.png"));
	}

	clearButton->setVisible(text.length());
}

void XinxLineEdit::updateClearButton()
{
	if (isReadOnly())
	{
		return;
	}

	const QSize geom = size();
	const int buttonWidth = clearButton->sizeHint().width();
	const QSize newButtonSize(buttonWidth, geom.height());

	const int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);

	const QFontMetrics fm(font());
	const int em = fm.width("m");

	const bool wideEnough = geom.width() > 4 * em + buttonWidth + frameWidth;

	if (newButtonSize != clearButton->size())
	{
		clearButton->resize(newButtonSize);
	}

	if (layoutDirection() == Qt::LeftToRight)
	{
		clearButton->move(geom.width() - frameWidth - buttonWidth - 1, 0);
	}
	else
	{
		clearButton->move(frameWidth + 1, 0);
	}

	if (wideEnough != wideEnoughForClear)
	{
		wideEnoughForClear = wideEnough;
		updateClearButtonIcon(text());
	}
}

void XinxLineEdit::resizeEvent(QResizeEvent * ev)
{
	updateClearButton();
	QLineEdit::resizeEvent(ev);
}

void XinxLineEdit::mousePressEvent(QMouseEvent* e)
{
	if ((e->button() == Qt::LeftButton || e->button() == Qt::MidButton) && clearButton)
	{
		clickInClear = clearButton->underMouse();
	}

	QLineEdit::mousePressEvent(e);
}

void XinxLineEdit::mouseReleaseEvent(QMouseEvent* e)
{
	if (clickInClear)
	{
		if (clearButton->underMouse())
		{
			setSelection(0, text().size());
			del();
			emit clearButtonClicked();
			emit textChanged(QString());
		}

		clickInClear = false;
		e->accept();
		return;
	}

	QLineEdit::mouseReleaseEvent(e);
}



