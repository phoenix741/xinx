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

#ifndef WEBSERVICESPROPERTYDIALOGIMPL_H
#define WEBSERVICESPROPERTYDIALOGIMPL_H
//
#include "ui_newservicefile.h"
//
class XSLProject;

class WebServicesPropertyDialogImpl : public QDialog, public Ui::WebServicesPropertyDialog {
	Q_OBJECT
public:
	WebServicesPropertyDialogImpl( QWidget * parent = 0, Qt::WFlags f = Qt::MSWindowsFixedSizeDialogHint );
	
	void setProject( XSLProject * project );
	
	QString generateXMLFile();
private slots:
	void on_m_testButton_clicked();
	void on_m_nextParamToolButton_clicked();
	void on_m_nextFieldToolButton_clicked();
	void on_m_previousParamToolButton_clicked();
	void on_m_previousFieldToolButton_clicked();
	void on_m_delParamToolButton_clicked();
	void on_m_delFieldToolButton_clicked();
	void on_m_addParamToolButton_clicked();
	void on_m_addFieldToolButton_clicked();
	void on_m_webServicesNameComboBox_currentIndexChanged(int index);
	
private:
	XSLProject * m_project;
};
#endif











