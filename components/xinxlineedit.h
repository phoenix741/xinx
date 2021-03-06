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

#pragma once
#ifndef XINXLINEEDIT_H
#define XINXLINEEDIT_H

#include <QLineEdit>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

#include <components-config.h>

class XinxLineEditPrivate;

class COMPONENTSEXPORT XinxLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit XinxLineEdit(const QString &string, QWidget *parent = 0);
	explicit XinxLineEdit(QWidget *parent = 0);
	virtual ~XinxLineEdit();

signals:
	void clearButtonClicked();

protected:
	virtual void resizeEvent(QResizeEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);

private:
	QSharedPointer<XinxLineEditPrivate> d;
	friend class XinxLineEditPrivate;
};

#endif
