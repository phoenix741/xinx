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

#ifndef _CODEDIALOGIMPL_H_
#define _CODEDIALOGIMPL_H_

// Xinx header
#include "ui_codedialog.h"

class CodeDialogImpl : public QDialog, public Ui::CodeDialog {
	Q_OBJECT
public:
	CodeDialogImpl( QWidget * p = 0 );
	virtual ~CodeDialogImpl();
public slots:
	void showText( const QString & text );
private:
};

#endif /* _CODEDIALOGIMPL_H_ */