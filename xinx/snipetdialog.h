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

#ifndef __SNIPETDIALOG_H__
#define __SNIPETDIALOG_H__

#include <QDialog>

#include "ui_snipetproperty.h"
#include "snipet.h"

class PrivateSnipetDialogImpl;

/*!
 * Implementation of snipet dialog. This dialog permit to create, modify.
 * The implementation containts only a constructor who defines default dialog 
 * presentation : Windows Style Dialog.
 */
class SnipetDialogImpl : public QDialog, public Ui::SnipetDialog {
	Q_OBJECT
public:
	/*!
	 * Constructor of the snipet dialog implementation. We defines a default windows flags.
	 * The windows can't be resize.
	 * \param text Text used for the template
	 * \param parent The parent of the dialog
	 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
	 */
	SnipetDialogImpl( const QString & text, QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	/*!
	 * Constructor of the snipet dialog implementation. 
	 * This constructor defines the type of the template.
	 * We defines a default windows flags. The windows can be resize.
	 * \param type Type of snipet
	 * \parap text Text used for the template
	 * \param parent The parent of the dialog
	 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
	 */
	SnipetDialogImpl( enum Snipet::SnipetType type, const QString & text, QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	/*!
	 * Constructor of the snipet dialog implementation, the dialog is created to use a snipet. 
	 * We defines a default windows flags. The windows can be resize.
	 * \param snipet The template to use.
	 * \param parent The parent of the dialog
	 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
	 */
	SnipetDialogImpl( Snipet * snipet, QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	/*!
	 * Destroy the dialog. 
	 */
	virtual ~SnipetDialogImpl();
	
	/*!
	 * Create a template with information given informations.
	 * \return a template
	 */
	Snipet * getSnipet();
	
private slots:

private:
	PrivateSnipetDialogImpl * d;
	friend class PrivateSnipetDialogImpl;
};

#endif // __SNIPETDIALOG_H__
