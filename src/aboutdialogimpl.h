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

#ifndef ABOUTDIALOGIMPL_H
#define ABOUTDIALOGIMPL_H

// Xinx header
#include "ui_about.h"

/*!
 * Implementation of about dialog containts informations as the version, the licence,
 * the logo, and authors.
 * The implementation containts only a constructor who defines default dialog 
 * presentation : Windows Style Dialog.
 */
class AboutDialogImpl : public QDialog, public Ui::AboutDialog {
	Q_OBJECT
public:
	/*!
	 * Constructor of The about dialog implementation. We defines a default windows flags.
	 * The windows can be resize.
	 * \param parent The parent of the dialog
	 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
	 */
	AboutDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
private slots:
};
#endif





