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

// Xinx header
#include "rcslogdialogimpl.h"
#include "ui_rcslogform.h"

/* PrivateRCSLogDockWidget */

class PrivateRCSLogDockWidget {
public:
	PrivateRCSLogDockWidget( RCSLogDockWidget * parent );
	~PrivateRCSLogDockWidget();
	
	Ui::RCSLogWidget * m_logwidget;
private:
	RCSLogDockWidget * m_parent;
};

PrivateRCSLogDockWidget::PrivateRCSLogDockWidget( RCSLogDockWidget * parent ) : m_parent( parent ) {
	QWidget * contentWidget = new QWidget( m_parent );
	m_logwidget = new Ui::RCSLogWidget();
	m_logwidget->setupUi( contentWidget );
	m_parent->setWidget( contentWidget );
}

PrivateRCSLogDockWidget::~PrivateRCSLogDockWidget() {
	
}

/* RCSLogDockWidget */

RCSLogDockWidget::RCSLogDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	d = new PrivateRCSLogDockWidget( this );
}

RCSLogDockWidget::RCSLogDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	d = new PrivateRCSLogDockWidget( this );
}

RCSLogDockWidget::~RCSLogDockWidget() {
	delete d;	
}

void RCSLogDockWidget::init() {
	d->m_logwidget->m_rcsLogListWidget->clear();
}

void RCSLogDockWidget::log( RCS::rcsLog niveau, const QString & info ) {
	QListWidgetItem * item = new QListWidgetItem( info, d->m_logwidget->m_rcsLogListWidget );
	switch( niveau ) {
	case RCS::LogError :
	case RCS::LogConflict :
		item->setForeground( Qt::red );
		break;
	case RCS::LogLocallyModified :
		item->setForeground( Qt::darkYellow );
		break;
	case RCS::LogRemotlyModified :
		item->setForeground( Qt::darkGreen );
		break;
	case RCS::LogApplication :
		item->setForeground( Qt::lightGray );
		break;
	case RCS::LogNotManaged :
	case RCS::LogNormal :
	default:
		;
	}
	d->m_logwidget->m_rcsLogListWidget->scrollToItem( item );
}
