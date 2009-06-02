/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef SERVICERESULTDIALOGIMPL_H
#define SERVICERESULTDIALOGIMPL_H
#pragma once

// Xinx header
#include "ui_resultatServices.h"

// Qt header
#include <QHash>

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

private slots:
	void inputComboChanged( QString value );
	void outputComboChanged( QString value );

private:
	QHash<QString,QString> m_input;
	QHash<QString,QString> m_output;
};
#endif





