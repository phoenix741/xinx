/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@shadoware.org                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include "codedialogimpl.h"
#include <qlinemarksinfocenter.h>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStringListModel>
#include <QCompleter>

/* CodeDialogImpl */

CodeDialogImpl::CodeDialogImpl( QWidget * p ) : QDialog( p ) {
	setupUi( this );
	connect( codeEdit, SIGNAL(searchWord(QString)), this, SLOT(showText(QString)) );

	QStringListModel * listModel = new QStringListModel( QStringList() << "void" << "int" << "temp" << "test", codeEdit );
	QCompleter * completer = new QCompleter( listModel, codeEdit );
	codeEdit->setCompleter( completer );

	QLineMarksInfoCenter::instance()->loadMarkTypes( ":/qcodeedit/marks.qxm" );

	QFile f( "codedialogimpl.cpp" );
	f.open( QIODevice::ReadOnly );
	QTextStream s( &f );
	codeEdit->setPlainText( s.readAll() );

	codeEdit->setErrors( QList<int>() << 4 << 9 );
}

CodeDialogImpl::~CodeDialogImpl() {

}

void CodeDialogImpl::showText( const QString & text ) {
	QMessageBox::information( this, "Message", text );
}
