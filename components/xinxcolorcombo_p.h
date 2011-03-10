/* This file is part of the KDE libraries
Copyright (C) 1997 Martin Jones (mjones@kde.org)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/
//-----------------------------------------------------------------------------
// KDE color selection combo box

// layout management added Oct 1997 by Mario Weilguni
// <mweilguni@sime.com>

/*
* Ulrich Van Den Hekke ( 21/03/2007 ) Adapt for Qt4
*/


#pragma once
#ifndef _XINXCOLORCOMBO_P_H__
#define _XINXCOLORCOMBO_P_H__

#include <xinxcolorcombo.h>
#include <QObject>
#include <QColor>

class XinxColorComboPrivate : public QObject
{
	Q_OBJECT
public:
	XinxColorComboPrivate(XinxColorCombo *parent);
	virtual ~XinxColorComboPrivate();

	void addColors();

	XinxColorCombo * _parent;
	bool showEmptyList;
	QColor customColor;
	QColor internalcolor;
public slots:
	void slotActivated(int index);
	void slotHighlighted(int index);

};

#endif  // _XINXCOLORCOMBO_P_H__
