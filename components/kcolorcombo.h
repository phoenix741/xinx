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


#ifndef _KCOLORCOMBO_H__
#define _KCOLORCOMBO_H__

#include <qcombobox.h>

class KColorComboInternal;

/*!
 * Combo box to choose a color. The combo box show you the color
 * and an associate text to the color. The showed color, is standard Qt color.
 * The user can also change, and use a custom color. In this case a dialog
 * is open to allow the user to make his choice.
 * 
 * This component is originally written for KDE3. He was adapt to turn on Qt4.
 */ 
class KColorCombo : public QComboBox {
    Q_OBJECT
	Q_CLASSINFO("Author", "Martin Jones")
	Q_CLASSINFO("Author", "Mario Weilguni")
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("Licence", "GPL v2 or later")
    Q_PROPERTY( QColor color READ color WRITE setColor )
public:
	/*!
	 * Create a color combo box
	 * \param parent The parent widget of the color combo box.
	 */
    KColorCombo( QWidget *parent );
    /*!
     * Destroy the ColorComboBox
     */
    ~KColorCombo();

    /*!
     * Change the value of the ComboBox to the color define in bracket.
     * \param col The new color to use. 
     */
    void setColor( const QColor &col );
    /*!
     * Return the color selected in the ComboBox.
     * \return The color selected
     */
    QColor color() const;

    /*!
     * Show an empty color combo box.
     */
    void showEmptyList();

signals:
	/*!
	 * Signal emited when a color is activated by the user.
	 */
    void activated( const QColor &col );
    /*!
     * Signal emited when a color is highlighted by the user.
     */
    void highlighted( const QColor &col );

protected:
    virtual void resizeEvent( QResizeEvent *re );

private slots:
    void slotActivated( int index );
    void slotHighlighted( int index );

private:
    void addColors();
    QColor customColor;
    QColor internalcolor;

protected:
    virtual void virtual_hook( int id, void* data );
private:
    class KColorComboPrivate;
    KColorComboPrivate *d;
};

#endif  // __KCOLORCOMBO_H__
