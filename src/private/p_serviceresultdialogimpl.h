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

#ifndef __P_SERVICERESULTDIALOGIMPL_H__
#define __P_SERVICERESULTDIALOGIMPL_H__

// Xinx header
#include "serviceresultdialogimpl.h"
#include "syntaxhighlighter.h"

class PrivateServiceResultDialogImpl : public QObject {
	Q_OBJECT
public:
	PrivateServiceResultDialogImpl( ServiceResultDialogImpl * parent );
	
	QHash<QString,QString> m_input;
	QHash<QString,QString> m_output;
	
public slots:
	void inputComboChanged( QString value );
	void outputComboChanged( QString value );
	
private:
	ServiceResultDialogImpl * m_parent;	
};

#endif // __P_SERVICERESULTDIALOGIMPL_H__
