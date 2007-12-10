/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#ifndef __P_SNIPETDIALOG_H__
#define __P_SNIPETDIALOG_H__

// Xinx header
#include "globals.h"
#include "snipetdialog.h"
#include "snipetlist.h"
#include "xmlhighlighter.h"
#include "jshighlighter.h"

// Qt header
#include <QApplication>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

class PrivateSnipetDialogImpl : public QObject {
	Q_OBJECT
public:
	PrivateSnipetDialogImpl( SnipetDialogImpl * parent );
	virtual ~PrivateSnipetDialogImpl();
	
	void setupUi();

	void addParamLine();

	QTextEdit * m_textEdit;
	QGridLayout * m_paramGrid;
	
	QList< QPair<QLabel*,QLineEdit*> > m_paramList;
	
	enum Snipet::SnipetType m_snipetType;
	Snipet * m_snipet;
public slots:
	void textChanged();
private:
	SnipetDialogImpl * m_parent;
};

#endif // __P_SNIPETDIALOG_H__
