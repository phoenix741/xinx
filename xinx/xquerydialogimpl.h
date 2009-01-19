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


#ifndef XQUERYDIALOGIMPL_H_
#define XQUERYDIALOGIMPL_H_

// Xinx header
#include "ui_xquery.h"

// Qt header

class XQueryDialogImpl : public QDialog, private Ui::XQueryDialog {
	Q_OBJECT
public:
	XQueryDialogImpl( QWidget * parent = 0, Qt::WindowFlags f = Qt::MSWindowsFixedSizeDialogHint );
	virtual ~XQueryDialogImpl();

	void setFileName( const QString & filename );
	void setCurrentXPath( const QString & currentXPath );
public slots:
	void evaluate();
private:
	QString m_fileName;
};

#endif /* XQUERYDIALOGIMPL_H_ */
