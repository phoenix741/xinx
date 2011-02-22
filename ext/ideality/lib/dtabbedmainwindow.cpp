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

#include "dtabbedmainwindow_p.h"

TabWidgetPrivate::TabWidgetPrivate(QWidget *parent) : QTabWidget(parent)
{
}

TabWidgetPrivate::~TabWidgetPrivate()
{
}

void TabWidgetPrivate::wheelEvent( QWheelEvent *ev )
{
	wheelMove( ev->delta() );
}

void TabWidgetPrivate::wheelMove( int delta )
{
	if ( count() > 1 )
	{
		int current = currentIndex();
		if ( delta < 0 )
		{
			current = (current + 1) % count();
		}
		else 
		{
			current--;
			if ( current < 0 )
				current = count() - 1;
		}
		setCurrentIndex( current );
	}
}


// DTabbedMainWindow

/**
 * Construct a tabbed main window.
 * @param parent 
 * @return 
 */
DTabbedMainWindow::DTabbedMainWindow(QWidget *parent) : DMainWindow(parent)
{
	m_tabWidget = new TabWidgetPrivate;
	
	setupTabWidget( m_tabWidget );
	setCentralWidget(m_tabWidget);
	
	connect(this, SIGNAL(perspectiveChanged(int)), this, SLOT(setupPerspective(int)));
}


/**
 * Destructor
 * @return 
 */
DTabbedMainWindow::~DTabbedMainWindow()
{
	
}

/**
 * Setup the tab widget.
 * @param w 
 */
void DTabbedMainWindow::setupTabWidget(QTabWidget *w)
{
	w->setFocusPolicy(Qt::NoFocus);
	
	if ( ! w->cornerWidget( Qt::TopRightCorner ) )
	{
		QToolButton *closeButton = new QToolButton(w);
		QPixmap closepx(16, 16);
		closepx.fill(Qt::transparent);
		
		QPainter p(&closepx);
		p.initFrom(this);
		
		p.setPen(QPen(palette().buttonText() , 4));
		p.drawLine(0, 0, 15,15);
		p.drawLine(0,15, 15, 0);
		
		
		p.end();
		
		closeButton->setIcon(closepx);
		closeButton->adjustSize();
		closeButton->hide();
		
		w->setCornerWidget(closeButton, Qt::TopRightCorner);
		
		connect(closeButton, SIGNAL(clicked()), this, SLOT(closeCurrentTab()));
	}
	
	connect(w, SIGNAL(currentChanged ( int)), this, SLOT(emitWidgetChanged( int )));
}

void DTabbedMainWindow::addWidget(QWidget *widget, bool persistant, int perspective)
{
	if ( perspective == currentPerspective() )
	{
		m_tabWidget->addTab(widget, widget->windowIcon(), widget->windowTitle() );
	}
	
	if ( persistant )
	{
		m_persistantWidgets << widget;
	}
	
	m_pages << widget;
	m_tabs[widget] = perspective;
	
	if ( QToolButton *button = dynamic_cast<QToolButton *>(m_tabWidget->cornerWidget(Qt::TopRightCorner)) )
	{
		if ( !button->isVisible() )
		{
			button->show();
		}
	}
}

/**
 * Remove a widget from the window.
 * @param widget 
 */
void DTabbedMainWindow::removeWidget(QWidget *widget)
{
	if ( m_persistantWidgets.contains(widget) ) return;
	
	int index = m_tabWidget->indexOf(widget);
	if ( index >= 0 )
	{
		m_tabWidget->removeTab( index );
	}
	
	m_tabs.remove(widget);
	m_pages.removeAll(widget);
	
	if ( m_tabWidget->count() == 0 )
	{
		if ( QToolButton *button = dynamic_cast<QToolButton *>(m_tabWidget->cornerWidget(Qt::TopRightCorner)) )
		{
			if ( button->isVisible() )
			{
				button->hide();
			}
		}
	}
}


/**
 * Close the current tab.
 */
void DTabbedMainWindow::closeCurrentTab()
{
	int index = m_tabWidget->currentIndex();
	
	if ( index >= 0 )
	{
		removeWidget(m_tabWidget->widget(index));
	}
}

void DTabbedMainWindow::emitWidgetChanged(int index)
{
	QWidget *w = m_tabWidget->widget(index);
	
	if ( m_persistantWidgets.contains(w) )
	{
		if ( QToolButton *button = dynamic_cast<QToolButton *>(m_tabWidget->cornerWidget(Qt::TopRightCorner)) )
		{
			button->setEnabled( false );
		}
	}
	else
	{
		if ( QToolButton *button = dynamic_cast<QToolButton *>(m_tabWidget->cornerWidget(Qt::TopRightCorner) ))
		{
			button->setEnabled( true );
		}
	}
	
	emit widgetChanged( w );
}


/**
 * Sets other tab widget.
 * @param w 
 */
void DTabbedMainWindow::setTabWidget(QTabWidget *w)
{
	m_tabWidget->close();
	
	setupTabWidget( w );
	
	delete m_tabWidget;
	m_tabWidget = 0;
	
	setCentralWidget( w );
	
	m_tabWidget = w;
}

/**
 * Return the current tab widget.
 * @return 
 */
QTabWidget *DTabbedMainWindow::tabWidget() const
{
	return m_tabWidget;
}

/**
 * Setup the perspective. Shows the pages in wps and hide others.
 * @param wps 
 */
void DTabbedMainWindow::setupPerspective(int wps)
{
	// FIXME: Flickr = (
	
	m_tabWidget->setUpdatesEnabled( false );
	setUpdatesEnabled( false );
	
	foreach(QWidget *w, m_pages )
	{
		int perspective = m_tabs[w];
		
		if ( wps & perspective )
		{
			m_tabWidget->addTab(w, w->windowIcon(), w->windowTitle() );
			w->show();
		}
		else
		{
			w->hide();
			m_tabWidget->removeTab( m_tabWidget->indexOf(w) );
		}
	}
	
	if ( QToolButton *button = dynamic_cast<QToolButton *>(m_tabWidget->cornerWidget(Qt::TopRightCorner)) )
	{
		if ( !button->isVisible() )
		{
			button->show();
		}
	}
	
	m_tabWidget->setUpdatesEnabled( true);
	setUpdatesEnabled( true);
	
#if 0
	int count = m_tabWidget->count();
	
	for(int index = 0; index < count; index++ )
	{
		QWidget *w = m_tabWidget->widget(index);
		
		if ( m_tabs[w] == wps )
		{
			w->show();
			m_tabWidget->setTabEnabled( index, true);
			
		}
		else
		{
			w->hide();
			m_tabWidget->setTabEnabled( index, false);
		}
	}
#endif
}


