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
#ifndef XINXLINEEDIT_P_H
#define XINXLINEEDIT_P_H

#include "xinxlineedit.h"

//! \private
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

//! \private
class XinxLineEditPrivate : public QObject
{
	Q_OBJECT
public slots:
	void updateClearButtonIcon(const QString&);

public:
	void init();
	void updateClearButton();

	bool clickInClear;
	bool wideEnoughForClear;
	XinxLineEditButton *clearButton;
	XinxLineEdit *edit;
};

#endif /* XINXLINEEDIT_P_H */
