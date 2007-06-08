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
 
#include "rcslogdialogimpl.h"

//
RCSLogDialogImpl::RCSLogDialogImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
	connect( m_abortButton, SIGNAL(clicked()), this, SIGNAL(abort()) );
}
//

void RCSLogDialogImpl::init() {
	m_abortButton->setEnabled( true );
	m_closeButton->setEnabled( false );
	m_rcsLogListWidget->clear();
}

void RCSLogDialogImpl::log( RCS::rcsLog niveau, const QString & info ) {
	QListWidgetItem * item = new QListWidgetItem(info, m_rcsLogListWidget);
	switch( niveau ) {
	case RCS::Error :
		item->setIcon( QIcon(":/button_cancel.png") );
		break;
	case RCS::Warning :
		item->setIcon( QIcon(":/warning.png") );
		break;
	case RCS::Information :
		item->setIcon( QIcon(":/button_ok.png") );
		break;
	default:
		;
	}
	m_rcsLogListWidget->scrollToItem( item );
}

void RCSLogDialogImpl::logTerminated() {
	m_closeButton->setEnabled( true );
	m_abortButton->setEnabled( false );
}
