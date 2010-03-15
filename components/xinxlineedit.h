/*  This file is part of the KDE libraries

    This class was originally inspired by Torben Weis'
    fileentry.cpp for KFM II.

    Copyright (C) 1997 Sven Radej <sven.radej@iname.com>
    Copyright (c) 1999 Patrick Ward <PAT_WARD@HP-USA-om5.om.hp.com>
    Copyright (c) 1999 Preston Brown <pbrown@kde.org>

    Completely re-designed:
    Copyright (c) 2000,2001 Dawit Alemayehu <adawit@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License (LGPL) as published by the Free Software Foundation;
    either version 2 of the License, or (at your option) any later
    version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
/*
 * Ulrich Van Den Hekke ( 21/03/2007 ) Simplified version of KLineEdit from KDE4.
 */

#ifndef XINXLINEEDIT_H
#define XINXLINEEDIT_H

#include <QLineEdit>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class XinxLineEditButton : public QWidget
{
	Q_OBJECT
public:
	XinxLineEditButton(QWidget *parent);

	QSize sizeHint() const;

	void setPixmap(const QPixmap & p);
	QPixmap pixmap();

protected:
	void paintEvent(QPaintEvent *event);

private:
	QPixmap m_pixmap;
};

class XinxLineEdit : public QLineEdit
{
	Q_OBJECT
public:

	/**
	 * Constructs a XinxLineEdit object with a default text, a parent,
	 * and a name.
	 *
	 * @param string Text to be shown in the edit widget.
	 * @param parent The parent widget of the line edit.
	 */
	explicit XinxLineEdit(const QString &string, QWidget *parent = 0);

	/**
	 * Constructs a line edit
	 * @param parent The parent widget of the line edit.
	 */
	explicit XinxLineEdit(QWidget *parent = 0);

	/**
	 *  Destructor.
	 */
	virtual ~XinxLineEdit();


Q_SIGNALS:
	/**
	 * Emitted when the user clicked on the clear button
	 */
	void clearButtonClicked();
protected:
	/**
	* Re-implemented for internal reasons.  API not affected.
	*
	* See QLineEdit::resizeEvent().
	*/
	virtual void resizeEvent(QResizeEvent *);

	/**
	* Re-implemented for internal reasons.  API not affected.
	*
	* See QLineEdit::mousePressEvent().
	*/
	virtual void mousePressEvent(QMouseEvent *);

	/**
	* Re-implemented for internal reasons.  API not affected.
	*
	* See QLineEdit::mouseReleaseEvent().
	*/
	virtual void mouseReleaseEvent(QMouseEvent *);

private Q_SLOTS:
	void updateClearButtonIcon(const QString&);

private:
	void init();
	void updateClearButton();

	bool clickInClear;
	bool wideEnoughForClear;
	XinxLineEditButton *clearButton;
};

#endif
