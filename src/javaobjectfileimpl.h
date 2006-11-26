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
 
#ifndef JAVAOBJECTFILEIMPL_H
#define JAVAOBJECTFILEIMPL_H
//
#include "ui_JavaObjectFile.h"
//

#include "ObjectView.h"

class JavaObjectFileImpl : public QDialog, public Ui::JavaObjectFile {
Q_OBJECT
public:
	JavaObjectFileImpl( ObjectsView * objects, QWidget * parent = 0, Qt::WFlags f = 0 );
private slots:
	void on_m_javaObjectPathChange_clicked();
	void on_m_javaObjectRefreshButton_clicked();
	void on_okButton_clicked();
	void on_m_javaObjectPathEdit_textChanged(QString);
	
	void addItem(const QString &);
private:
    ObjectsView * m_appObjects;
    
    void fillList(ObjectsView*);
    bool validDir(const QString &);
};
#endif





