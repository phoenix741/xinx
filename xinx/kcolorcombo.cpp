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

#include "kcolorcombo.h"

#define STANDARD_PAL_SIZE 17

static QColor *standardPalette = 0;
static QHash<QColor,QString> standardPaletteName;

uint qHash ( const QColor & key ) {
	return qHash( key.name() );	
}

static void createStandardPalette()
{
    if ( standardPalette )
    return;

    standardPalette = new QColor [STANDARD_PAL_SIZE];

    int i = 0;

    standardPaletteName[Qt::red] = QObject::tr("Red"); standardPalette[i++] = Qt::red;
    standardPaletteName[Qt::green] = QObject::tr("Green"); standardPalette[i++] = Qt::green;
    standardPaletteName[Qt::blue] = QObject::tr("Blue"); standardPalette[i++] = Qt::blue;
    standardPaletteName[Qt::cyan] = QObject::tr("Cyan"); standardPalette[i++] = Qt::cyan;
    standardPaletteName[Qt::magenta] = QObject::tr("Magenta"); standardPalette[i++] = Qt::magenta;
    standardPaletteName[Qt::yellow] = QObject::tr("Yellow"); standardPalette[i++] = Qt::yellow;
    standardPaletteName[Qt::darkRed] = QObject::tr("Dark Red"); standardPalette[i++] = Qt::darkRed;
    standardPaletteName[Qt::darkGreen] = QObject::tr("Dark Green"); standardPalette[i++] = Qt::darkGreen;
    standardPaletteName[Qt::darkBlue] = QObject::tr("Dark Blue"); standardPalette[i++] = Qt::darkBlue;
    standardPaletteName[Qt::darkCyan] = QObject::tr("Dark Cyan"); standardPalette[i++] = Qt::darkCyan;
    standardPaletteName[Qt::darkMagenta] = QObject::tr("Dark Magenta"); standardPalette[i++] = Qt::darkMagenta;
    standardPaletteName[Qt::darkYellow] = QObject::tr("Dark Yellow"); standardPalette[i++] = Qt::darkYellow;
    standardPaletteName[Qt::white] = QObject::tr("White"); standardPalette[i++] = Qt::white;
    standardPaletteName[Qt::lightGray] = QObject::tr("Light Gray"); standardPalette[i++] = Qt::lightGray;
    standardPaletteName[Qt::gray] = QObject::tr("Gray"); standardPalette[i++] = Qt::gray;
    standardPaletteName[Qt::darkGray] = QObject::tr("Dark Gray"); standardPalette[i++] = Qt::darkGray;
    standardPaletteName[Qt::black] = QObject::tr("Black"); standardPalette[i++] = Qt::black;
}

class KColorCombo::KColorComboPrivate
{
    protected:
    friend class KColorCombo;
    KColorComboPrivate(){}
    ~KColorComboPrivate(){}
    bool showEmptyList;
};

KColorCombo::KColorCombo( QWidget *parent )
    : QComboBox( parent )
{
    d=new KColorComboPrivate();
    d->showEmptyList=false;

    customColor.setRgb( 255, 255, 255 );
    internalcolor.setRgb( 255, 255, 255 );

    createStandardPalette();

    addColors();

    connect( this, SIGNAL( activated(int) ), SLOT( slotActivated(int) ) );
    connect( this, SIGNAL( highlighted(int) ), SLOT( slotHighlighted(int) ) );
}


KColorCombo::~KColorCombo()
{
    delete d;
}
void KColorCombo::setColor( const QColor &col )
{
    internalcolor = col;
    d->showEmptyList=false;
    addColors();
}


QColor KColorCombo::color() const {
  return internalcolor;
}

void KColorCombo::resizeEvent( QResizeEvent *re )
{
    QComboBox::resizeEvent( re );

    addColors();
}

void KColorCombo::showEmptyList()
{
    d->showEmptyList=true;
    addColors();
}

void KColorCombo::slotActivated( int index )
{
    if ( index == 0 )
    {
    	QColor res = QColorDialog::getColor( customColor, this );
        if ( res.isValid() )
        {
        	customColor = res;
        	
            QPainter painter;
            QPen pen;
            QRect rect( 0, 0, 32, 32 );
            QPixmap pixmap( rect.width(), rect.height() );

            if ( qGray( customColor.rgb() ) < 128 )
                pen.setColor( Qt::white );
            else
                pen.setColor( Qt::black );

            painter.begin( &pixmap );
            QBrush brush( customColor );
            painter.fillRect( rect, brush );
            painter.setPen( pen );
            painter.end();

            setItemIcon( 0, QIcon( pixmap ) );
            pixmap.detach();
        }

        internalcolor = customColor;
    }
    else
        internalcolor = standardPalette[ index - 1 ];

    emit activated( internalcolor );
}

void KColorCombo::slotHighlighted( int index )
{
    if ( index == 0 )
        internalcolor = customColor;
    else
        internalcolor = standardPalette[ index - 1 ];

    emit highlighted( internalcolor );
}

void KColorCombo::addColors()
{
    QPainter painter;
    QPen pen;
    QRect rect( 0, 0, 32, 32 );
    QPixmap pixmap( rect.width(), rect.height() );
    int i;

    clear();
    if (d->showEmptyList) return;

    createStandardPalette();

    for ( i = 0; i < STANDARD_PAL_SIZE; i++ )
        if ( standardPalette[i] == internalcolor ) break;

    if ( i == STANDARD_PAL_SIZE )
        customColor = internalcolor;

    if ( qGray( customColor.rgb() ) < 128 )
        pen.setColor( Qt::white );
    else
        pen.setColor( Qt::black );

    painter.begin( &pixmap );
    QBrush brush( customColor );
    painter.fillRect( rect, brush );
    painter.end();

    addItem( QIcon( pixmap ), tr("Custom...") );
    pixmap.detach();

    for ( i = 0; i < STANDARD_PAL_SIZE; i++ )
    {
        painter.begin( &pixmap );
        QBrush brush( standardPalette[i] );
        painter.fillRect( rect, brush );
        painter.end();

	    addItem( QIcon( pixmap ), standardPaletteName[ standardPalette[i] ] );
        pixmap.detach();

        if ( standardPalette[i] == internalcolor )
            setCurrentIndex( i + 1 );
    }
}

void KColorCombo::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

