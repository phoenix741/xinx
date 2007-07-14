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

#ifndef SERVICERESULTDIALOGIMPL_H
#define SERVICERESULTDIALOGIMPL_H
//
#include "ui_resultatServices.h"
//
class PrivateServiceResultDialogImpl;

/*!
 * Class who show the result of the Web Services.
 * The class show the Input part, and the Output part.
 */
class ServiceResultDialogImpl : public QDialog, public Ui::ServiceResultDialog {
	Q_OBJECT
public:
	/*!
	 * Create the Result dialog.
	 * \param parent Parent of the result dialog.
	 * \param f Flag of the result dialog.
	 */
	ServiceResultDialogImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	virtual ~ServiceResultDialogImpl();

	/*!
	 * Set the input parameter list. The param is in the ComboBox, and value in the text dialog,
	 * for the choosen parameter.
	 * \param input List of parameter and value of the parameter.
	 */
	void setInputStreamText( const QHash<QString,QString> & input );
	/*!
	 * Set the output parameter list. The param is in the ComboBox, and value in the text dialog,
	 * for the choosen parameter.
	 * \param output List of parameter and value of the parameter.
	 */
	void setOutputStreamText( const QHash<QString,QString> & output );
private:
	PrivateServiceResultDialogImpl * d;
	friend class PrivateServiceResultDialogImpl;
};
#endif





