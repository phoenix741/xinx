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
// KDE color selection dialog.
//
// 1999-09-27 Espen Sand <espensa@online.no>
// KColorDialog is now subclassed from KDialogBase. I have also extended
// KColorDialog::getColor() so that in contains a parent argument. This
// improves centering capability.
//
// layout management added Oct 1997 by Mario Weilguni
// <mweilguni@sime.com>
//

/*
 * Ulrich Van Den Hekke ( 21/03/2007 ) Adapt for Qt4
 */

#include <QPainter>
#include <QColorDialog>
#include <QHash>

#include "xinxcolorcombo_p.h"

#define STANDARD_PAL_SIZE 18

static QColor *standardPalette = 0;
static QHash<QColor,QString> standardPaletteName;

uint qHash(const QColor & key)
{
	return qHash(key.name());
}

static void createStandardPalette()
{
	if (standardPalette) return;

	standardPalette = new QColor [STANDARD_PAL_SIZE];

	int i = 0;

	standardPaletteName[QColor()]        = XinxColorCombo::tr("None");
	standardPalette[i++] = QColor();
	standardPaletteName[Qt::red]         = XinxColorCombo::tr("Red");
	standardPalette[i++] = Qt::red;
	standardPaletteName[Qt::green]       = XinxColorCombo::tr("Green");
	standardPalette[i++] = Qt::green;
	standardPaletteName[Qt::blue]        = XinxColorCombo::tr("Blue");
	standardPalette[i++] = Qt::blue;
	standardPaletteName[Qt::cyan]        = XinxColorCombo::tr("Cyan");
	standardPalette[i++] = Qt::cyan;
	standardPaletteName[Qt::magenta]     = XinxColorCombo::tr("Magenta");
	standardPalette[i++] = Qt::magenta;
	standardPaletteName[Qt::yellow]      = XinxColorCombo::tr("Yellow");
	standardPalette[i++] = Qt::yellow;
	standardPaletteName[Qt::darkRed]     = XinxColorCombo::tr("Dark Red");
	standardPalette[i++] = Qt::darkRed;
	standardPaletteName[Qt::darkGreen]   = XinxColorCombo::tr("Dark Green");
	standardPalette[i++] = Qt::darkGreen;
	standardPaletteName[Qt::darkBlue]    = XinxColorCombo::tr("Dark Blue");
	standardPalette[i++] = Qt::darkBlue;
	standardPaletteName[Qt::darkCyan]    = XinxColorCombo::tr("Dark Cyan");
	standardPalette[i++] = Qt::darkCyan;
	standardPaletteName[Qt::darkMagenta] = XinxColorCombo::tr("Dark Magenta");
	standardPalette[i++] = Qt::darkMagenta;
	standardPaletteName[Qt::darkYellow]  = XinxColorCombo::tr("Dark Yellow");
	standardPalette[i++] = Qt::darkYellow;
	standardPaletteName[Qt::white]       = XinxColorCombo::tr("White");
	standardPalette[i++] = Qt::white;
	standardPaletteName[Qt::lightGray]   = XinxColorCombo::tr("Light Gray");
	standardPalette[i++] = Qt::lightGray;
	standardPaletteName[Qt::gray]        = XinxColorCombo::tr("Gray");
	standardPalette[i++] = Qt::gray;
	standardPaletteName[Qt::darkGray]    = XinxColorCombo::tr("Dark Gray");
	standardPalette[i++] = Qt::darkGray;
	standardPaletteName[Qt::black]       = XinxColorCombo::tr("Black");
	standardPalette[i++] = Qt::black;
}

/* XinxColorComboPrivate */

XinxColorComboPrivate::XinxColorComboPrivate(XinxColorCombo *parent) : _parent(parent)
{
}

XinxColorComboPrivate::~XinxColorComboPrivate()
{
}

void XinxColorComboPrivate::slotActivated(int index)
{
	if (index == 0)
	{
		QColor res = QColorDialog::getColor(customColor, _parent);

		if (res.isValid())
		{
			customColor = res;

			QPainter painter;
			QPen pen;
			QRect rect(0, 0, 32, 32);
			QPixmap pixmap(rect.width(), rect.height());

			if (qGray(customColor.rgb()) < 128)
				pen.setColor(Qt::white);
			else
				pen.setColor(Qt::black);

			painter.begin(&pixmap);
			QBrush brush(customColor);
			painter.fillRect(rect, brush);
			painter.setPen(pen);
			painter.end();

			_parent->setItemIcon(0, QIcon(pixmap));
			pixmap.detach();
		}

		internalcolor = customColor;
	}
	else
		internalcolor = standardPalette[ index - 1 ];

	emit _parent->activated(internalcolor);
}

void XinxColorComboPrivate::slotHighlighted(int index)
{
	if (index == 0)
		internalcolor = customColor;
	else
		internalcolor = standardPalette[ index - 1 ];

	emit _parent->highlighted(internalcolor);
}

void XinxColorComboPrivate::addColors()
{
	_parent->clear();
	if (showEmptyList) return;

	createStandardPalette();

	int i;
	for (i = 0; i < STANDARD_PAL_SIZE; i++)
		if (standardPalette[i] == internalcolor) break;

		if (i == STANDARD_PAL_SIZE)
			customColor = internalcolor;

		QPen pen;
	if (qGray(customColor.rgb()) < 128)
		pen.setColor(Qt::white);
	else
		pen.setColor(Qt::black);

	QRect rect(0, 0, 32, 32);

	QPixmap pixmap(rect.width(), rect.height());

	QPainter painter;
	painter.begin(&pixmap);
	painter.fillRect(rect, QBrush(customColor));
	painter.end();

	_parent->addItem(QIcon(pixmap), tr("Custom..."));
	pixmap.detach();

	for (i = 0; i < STANDARD_PAL_SIZE; i++)
	{
		if (standardPalette[i].isValid())
		{
			painter.begin(&pixmap);
			QBrush brush(standardPalette[i]);
			painter.fillRect(rect, brush);
			painter.end();

			_parent->addItem(QIcon(pixmap), standardPaletteName[ standardPalette[i] ]);
		}
		else
		{
			_parent->addItem(standardPaletteName[ standardPalette[i] ]);
		}

		pixmap.detach();

		if (standardPalette[i] == internalcolor)
			_parent->setCurrentIndex(i + 1);
	}
}

/* XinxColorCombo */

/*!
 * \ingroup Components
 * \class XinxColorCombo
 * \since 0.8.0.0
 *
 * \brief The XinxColorCombo provides a QComboBox used to select color.
 *
 * \bc 0.10.0.0
 *
 * Combo box to choose a color. The combo box shows you the color
 * and an associated text to the color. The shown color, is standard Qt color.
 * The user can also change, and use a customed color. In this case a dialog
 * is open to allow the user to make his choice.
 *
 * This component is originally written for KDE3. It was adapted to turn on Qt4.
 *
 * \image html xinxcolorcombobox.png
 * \image latex xinxcolorcombobox.png
 */


/*!
 * \brief Creates a color combo box
 * \param parent The parent widget of the color combo box.
 */
XinxColorCombo::XinxColorCombo(QWidget *parent) : QComboBox(parent), d(new XinxColorComboPrivate(this))
{

	d->showEmptyList = false;

	d->customColor.setRgb(255, 255, 255);
	d->internalcolor.setRgb(255, 255, 255);

	createStandardPalette();

	d->addColors();

	connect(this, SIGNAL(activated(int)), d.data(), SLOT(slotActivated(int)));
	connect(this, SIGNAL(highlighted(int)), d.data(), SLOT(slotHighlighted(int)));
}

//! Destroy the ColorComboBox
XinxColorCombo::~XinxColorCombo()
{

}

/*!
 * \fn void XinxColorCombo::activated(const QColor &col);
 * \brief Signal emited when a color is activated by the user.
 */

/*!
 * \fn void XinxColorCombo::highlighted(const QColor &col);
 * \brief Signal emited when a color is highlighted by the user.
 */

/*!
 * \brief Change the value of the ComboBox to the color define in bracket.
 * \param col The new color to use.
 * \sa color()
 */
void XinxColorCombo::setColor(const QColor &col)
{
	d->internalcolor = col;
	d->showEmptyList=false;
	d->addColors();
}

/*!
 * \brief Returns the color selected in the ComboBox.
 * \return The color selected
 * \sa setColor()
 */
QColor XinxColorCombo::color() const
{
	return d->internalcolor;
}

/*!
 * Re-implemented for internal reasons.  API not affected.
 * \sa QComboBox::resizeEvent().
 */
void XinxColorCombo::resizeEvent(QResizeEvent *re)
{
	QComboBox::resizeEvent(re);
	d->addColors();
}

//! Shows an empty color combo box.
void XinxColorCombo::showEmptyList()
{
	d->showEmptyList=true;
	d->addColors();
}


