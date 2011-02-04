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
#ifndef _XINXCOLORCOMBO_H__
#define _XINXCOLORCOMBO_H__

#include <qcombobox.h>

#include <components-config.h>

class XinxColorComboInternal;

class COMPONENTSEXPORT XinxColorCombo : public QComboBox
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Martin Jones")
	Q_CLASSINFO("Author", "Mario Weilguni")
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
	Q_PROPERTY(QColor color READ color WRITE setColor)
public:
	XinxColorCombo(QWidget *parent);
	virtual ~XinxColorCombo();

	void setColor(const QColor &col);
	QColor color() const;

	void showEmptyList();
signals:
	void activated(const QColor &col);
	void highlighted(const QColor &col);

protected:
	virtual void resizeEvent(QResizeEvent *re);

private slots:
	void slotActivated(int index);
	void slotHighlighted(int index);

private:
	void addColors();
	QColor customColor;
	QColor internalcolor;

private:
	class XinxColorComboPrivate;
	XinxColorComboPrivate *d;
};

#endif  // __XINXCOLORCOMBO_H__
