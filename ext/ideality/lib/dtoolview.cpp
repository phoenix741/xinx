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

#include "dtoolview.h"
#include "dviewbutton.h"
#include "dmainwindow.h"

#include <QtDebug>
#include <QAction>
#include <QMainWindow>
#include <QMouseEvent>
#include <QLayout>
#include <QEvent>

DToolView::DToolView(const QString &title, const QIcon &icon, QWidget * parent)
	: QDockWidget(title, parent), m_size(-1), m_perspective(0)
{
	setWindowIcon(icon);
	setup();
	
	setObjectName("DToolView-"+title);
}


DToolView::~DToolView()
{
}

void DToolView::setup()
{
	
#if QT_VERSION < 0x040200
	m_area = Qt::AllDockWidgetAreas;
#endif

	setFeatures(AllDockWidgetFeatures);
	
	m_button = new DViewButton(this);
	
	connect(toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(saveSize(bool)));
}


DViewButton *DToolView::button() const
{
	return m_button;
}

void DToolView::saveSize(bool checked)
{
	if ( m_button->area() == Qt::LeftToolBarArea || m_button->area() == Qt::RightToolBarArea )
	{
		m_size = width();
	}
	else
	{
		m_size = height();
	}
	
	setVisible(checked);
}

QSize DToolView::sizeHint() const
{
	QSize size = QDockWidget::sizeHint();
	
	if ( m_size < 0 ) return size;
	
	if ( m_button->area() == Qt::LeftToolBarArea || m_button->area()  == Qt::RightToolBarArea )
	{
		size.setWidth(m_size);
	}
	else
	{
		size.setHeight(m_size);
	}
	
	return size;
}


void DToolView::setDescription(const QString &description)
{
	m_button->setStatusTip ( description );
}

void DToolView::setPerspective(int wsp)
{
	m_perspective = wsp;
}

int DToolView::perspective() const
{
	return m_perspective;
}

int DToolView::fixedSize() const
{
	return m_size;
}

void DToolView::setFixedSize(int s)
{
	m_size = s;
}

void DToolView::showEvent(QShowEvent *e)
{
	if ( DMainWindow *mw = dynamic_cast<DMainWindow *>(parentWidget() ) )
	{
		if ( !(mw->currentPerspective() & m_perspective) ) 
		{
			e->ignore(); // make sure!
			return;
		}
	}
	
	QDockWidget::showEvent(e);
}

#if QT_VERSION < 0x040200

bool DToolView::event(QEvent *e)
{
	bool toReturn =  QDockWidget::event(e);
	
	if (e->type() == QEvent::MouseButtonPress )
	{
		if ( QMainWindow *mw = dynamic_cast<QMainWindow *>(parentWidget()) )
		{
			m_area = mw->dockWidgetArea(this);
		}
	}
	else if ( e->type() == QEvent::MouseButtonRelease )
	{
		if ( QMainWindow *mw = dynamic_cast<QMainWindow *>(parentWidget()) )
		{
			Qt::DockWidgetArea newArea = mw->dockWidgetArea(this);
			if ( m_area !=  newArea)
			{
				mw->removeDockWidget(this);
				mw->addDockWidget(newArea, this );
				
				emit topLevelChanged(false);
			}
		}
	}
	
	
	return toReturn;
}

#endif





