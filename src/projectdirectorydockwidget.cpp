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

// Xinx header
#include "projectdirectorydockwidget.h"
#include "ui_projectdirectorywidget.h"

/* PrivateProjectDirectoryDockWidget */

class PrivateProjectDirectoryDockWidget {
public:
	PrivateProjectDirectoryDockWidget( ProjectDirectoryDockWidget * parent );
	~PrivateProjectDirectoryDockWidget();

	Ui::ProjectDirectoryWidget * m_projectDirWidget;
private:
	ProjectDirectoryDockWidget * m_parent;
};

PrivateProjectDirectoryDockWidget::PrivateProjectDirectoryDockWidget( ProjectDirectoryDockWidget * parent ) : m_parent( parent ) {
	QWidget * contentWidget = new QWidget( m_parent );
	m_projectDirWidget = new Ui::ProjectDirectoryWidget();
	m_projectDirWidget->setupUi( contentWidget );
	m_parent->setWidget( contentWidget );
}

PrivateProjectDirectoryDockWidget::~PrivateProjectDirectoryDockWidget() {
	
}

/* ContentDockWidget */
	
ProjectDirectoryDockWidget::ProjectDirectoryDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	d = new PrivateProjectDirectoryDockWidget( this );
}

ProjectDirectoryDockWidget::ProjectDirectoryDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	d = new PrivateProjectDirectoryDockWidget( this );	
}

ProjectDirectoryDockWidget::~ProjectDirectoryDockWidget() {
	delete d;
}

