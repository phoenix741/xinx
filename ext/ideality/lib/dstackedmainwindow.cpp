/***************************************************************************
 *   Copyright (C) 2006 by David Cuadrado                                *
 *   krawek@gmail.com                                                      *
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

#include "dstackedmainwindow.h"

#include <QStackedWidget>

DStackedMainWindow::DStackedMainWindow(QWidget *parent) : DMainWindow(parent)
{
	m_stack = new QStackedWidget;
	setCentralWidget( m_stack );
	
	connect(this, SIGNAL(perspectiveChanged( int )), this, SLOT(setupPerspective(int)));
}


DStackedMainWindow::~DStackedMainWindow()
{
}

void DStackedMainWindow::addWidget(QWidget *widget, int perspective)
{
	if ( m_widgets.contains( perspective ) ) return;
	
	m_widgets.insert(perspective, widget);
	
	m_stack->addWidget(widget);
}

void DStackedMainWindow::removeWidget(QWidget *widget)
{
	m_stack->removeWidget(widget);
	
	m_widgets.remove( m_widgets.key(widget) );
}

void DStackedMainWindow::setupPerspective(int perspective)
{
	if ( m_widgets.contains(perspective) )
	{
		QWidget *w = m_widgets[perspective];
		m_stack->setCurrentWidget(w);
	}
}


