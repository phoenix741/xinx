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

#include "dmainwindowfactory.h"

#include <dtabbedmainwindow.h>
#include <dstackedmainwindow.h>
#include <dworkspacemainwindow.h>

#include <QDockWidget>
#include <QToolBar>
#include <QTabWidget>
#include <QEvent>
#include <QApplication>
#include <QShowEvent>

#include <QtDebug>


class EventFilter : public QObject
{
	public:
		EventFilter(DMainWindow *mw, QObject *parent = 0);
		~EventFilter();
		
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
		
	private:
		DMainWindow *m_mainWindow;
};

EventFilter::EventFilter(DMainWindow *mw, QObject *parent) : QObject(parent), m_mainWindow(mw)
{
}

EventFilter::~EventFilter()
{
}

bool EventFilter::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Close)
	{
		m_mainWindow->close();
		
		return true;
	}
	else if ( event->type() == QEvent::Show )
	{
		m_mainWindow->show();
		
		return true;
	}
	else if ( event->type() == QEvent::Hide )
	{
		m_mainWindow->hide();
		
		return true;
	}
	
	
	return false;
}

/**
 * Construct a factory.
 * @return 
 */
DMainWindowFactory::DMainWindowFactory()
{
}


/**
 * Destructor
 * @return 
 */
DMainWindowFactory::~DMainWindowFactory()
{
}

/**
 * if centralWidget() is a QTabWidget an instance of DTabbedMainWindow will be created
 * @param other 
 * @return 
 */
DMainWindow *DMainWindowFactory::create(QMainWindow *other)
{
	DMainWindow *mainWindow = 0;
	
	if ( other->isVisible() )
	{
		other->hide();
	}
	
	if ( other->inherits( "DMainWindow" ) )
	{
		qWarning() << QObject::tr("Can't create a DMainWindow from DMainWindow");
		return static_cast<DMainWindow *>(other);
	}
	
	if ( QWidget *central = other->centralWidget() )
	{
		if ( QTabWidget *tabWidget = dynamic_cast<QTabWidget *>(central) )
		{
			mainWindow = new DTabbedMainWindow;
			static_cast<DTabbedMainWindow *>(mainWindow)->setTabWidget(tabWidget);
		}
		else
		{
			mainWindow = new DMainWindow;
			central->setParent(mainWindow);
			mainWindow->setCentralWidget( central );
		}
	}
	
	QList<QDockWidget *> docks = other->findChildren<QDockWidget *>();
	
	foreach(QDockWidget *dock, docks )
	{
		dock->widget()->setWindowTitle(dock->windowTitle());
		dock->widget()->setWindowIcon(dock->windowIcon());
		
		Qt::DockWidgetArea area = other->dockWidgetArea(dock);

		if ( area == 0)
		{
			area = Qt::LeftDockWidgetArea;
		}
		
		mainWindow->addToolView( dock->widget(), area);
	}
	
	QList<QToolBar *> toolBars = other->findChildren<QToolBar *>();
	
	foreach(QToolBar *toolBar, toolBars )
	{
		toolBar->setParent(mainWindow);
		
		Qt::ToolBarArea area = other->toolBarArea(toolBar);
		
		if ( area == 0)
		{
			area = Qt::TopToolBarArea;
		}
		
		mainWindow->addToolBar(area, toolBar);
	}
	
	mainWindow->setStatusBar(other->statusBar());
	mainWindow->setMenuBar(other->menuBar());
	mainWindow->setCurrentPerspective( DMainWindow::DefaultPerspective );
	
	
	
	EventFilter *eventFilter = new EventFilter(mainWindow, other);
	other->installEventFilter( eventFilter );
	
	return mainWindow;
}



