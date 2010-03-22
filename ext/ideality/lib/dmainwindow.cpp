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

#include "dmainwindow.h"
#include "dbuttonbar.h"
#include "dtoolview.h"
#include "dviewbutton.h"
#include "dmainwindowabstractsettings.h"

#include <QTimer>
#include <QMenu>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QApplication>
#include <QtDebug>
#include <QHashIterator>
#include <QMenuBar>

#include <QCloseEvent>

class DefaultSettings : public DMainWindowAbstractSettings
{
	public:
		DefaultSettings(QObject *parent);
		~DefaultSettings();
		
		void save(DMainWindow *w);
		void restore(DMainWindow *w);
};

DefaultSettings::DefaultSettings(QObject *parent) : DMainWindowAbstractSettings(parent)
{
}

DefaultSettings::~DefaultSettings()
{
}

void DefaultSettings::save(DMainWindow *w)
{
	QSettings settings(qApp->applicationName(), "ideality", this);
// 	settings.clear();
	
	QHash<Qt::ToolBarArea, DButtonBar *> buttonBars = w->buttonBars();
	QHash<DButtonBar *, QList<DToolView*> > toolViews = w->toolViews();
	
	foreach( DButtonBar *bar, buttonBars.values())
	{
		settings.beginGroup(bar->windowTitle());
		settings.setValue("exclusive", bar->isExclusive() );
		settings.endGroup();
		
		settings.beginGroup(bar->windowTitle());
		settings.setValue("autohide", bar->autohide() );
		settings.endGroup();
		
		foreach(DToolView *view, toolViews[bar] )
		{
			settings.beginGroup(view->objectName());
			
// 			qDebug() << view->windowTitle() << view->button()->area();
			
			settings.setValue("area", int( view->button()->area() ));
	
			settings.setValue("size", view->fixedSize());
			settings.setValue("style", view->button()->toolButtonStyle() );
			settings.setValue("sensibility", view->button()->isSensible());
	
			settings.setValue("visible", view->isVisible() );
			settings.setValue("floating", view->isFloating());
			settings.setValue("position", view->pos());
			
			settings.endGroup();
		}
	}
	
	settings.beginGroup( "MainWindow" );
	settings.setValue("size", w->size());
	settings.setValue("maximized", w->isMaximized() );
	settings.setValue("position", w->pos());
	
	settings.endGroup();
}

void DefaultSettings::restore(DMainWindow *w)
{
	QSettings settings(qApp->applicationName(), "ideality", this);
	
	QHash<Qt::ToolBarArea, DButtonBar *> buttonBars = w->buttonBars();
	QHash<DButtonBar *, QList<DToolView*> > toolViews = w->toolViews();
	
	QList<DToolView *> toHide;
	
	foreach( DButtonBar *bar, buttonBars.values())
	{
		bar->setExclusive(false);
		
		foreach(DToolView *view, toolViews[bar] )
		{
			settings.beginGroup(view->objectName());
			
			// Restore position
			
			Qt::DockWidgetArea area = Qt::DockWidgetArea(settings.value("area", 0).toInt());
			
			w->moveToolView(view, area);
			
			view->setFixedSize(settings.value("size").toInt());
			
			view->button()->setToolButtonStyle( Qt::ToolButtonStyle(settings.value("style", int(view->button()->toolButtonStyle()) ).toInt()) );
			view->button()->setSensible( settings.value("sensibility", view->button()->isSensible()).toBool() );
			
			
			bool visible = settings.value("visible", false ).toBool();
			
			if ( visible && view->button()->isVisible() )
			{
				view->button()->setChecked(true);
				view->show();
			}
			else
			{
				toHide << view;
			}
			
			view->setFloating(settings.value("floating", false).toBool() );
			if ( view->isFloating() )
			{
				view->move(settings.value("position").toPoint());
			}
			
			settings.endGroup();
		}
		
		settings.beginGroup(bar->windowTitle());
		bar->setExclusive(settings.value("exclusive", true ).toBool());
		settings.endGroup();
		
#if QT_VERSION >= 0x040200
		settings.beginGroup(bar->windowTitle());
		bar->setAutoHide(settings.value("autohide", false ).toBool());
		settings.endGroup();
#endif
	}
	
	foreach(DToolView *v, toHide )
	{
		v->button()->setChecked(false);
		v->setVisible(false);
		v->close();
	}
	
	settings.beginGroup( "MainWindow" );
	w->resize(settings.value("size").toSize());
	bool maximized = settings.value("maximized", false ).toBool();
	if ( maximized )
	{
		w->showMaximized();
	}
	
	w->move(settings.value("position").toPoint());
	
	settings.endGroup();
}

// DMainWindow


/**
 * Construct a main window
 * @param parent 
 * @return 
 */
DMainWindow::DMainWindow(QWidget *parent)
	: QMainWindow(parent), m_forRelayout(0), m_currentPerspective(DefaultPerspective), m_autoRestore(false)
{
	setObjectName("DMainWindow");
	
	m_settings = new DefaultSettings(this);
	
	addButtonBar(Qt::LeftToolBarArea);
	addButtonBar(Qt::RightToolBarArea);
	addButtonBar(Qt::TopToolBarArea);
	addButtonBar(Qt::BottomToolBarArea);
	
#if 0
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
#endif

#if QT_VERSION >= 0x040200
	setDockNestingEnabled (false);
#endif
}


/**
 * Default destructor
 * @return 
 */
DMainWindow::~DMainWindow()
{
}


void DMainWindow::addButtonBar(Qt::ToolBarArea area)
{
	DButtonBar *bar = new DButtonBar(area, this);
	
	addToolBar(area, bar);
	m_buttonBars.insert(area, bar);
	
	bar->hide();
}


/**
 * Adds a tool view to the main window in the area and perspective.
 * 
 * @param widget 
 * @param area 
 * @param perspective 
 * @return 
 */
DToolView *DMainWindow::addToolView(QWidget *widget, Qt::DockWidgetArea area, int perspective)
{
	DToolView *toolView = new DToolView(widget->windowTitle(), widget->windowIcon());
	toolView->setWidget(widget);
	toolView->setPerspective( perspective );
	
	toolView->button()->setArea( toToolBarArea( area ) );
	m_buttonBars[toToolBarArea( area) ]->addButton(toolView->button());
	
	addDockWidget(area, toolView);
	widget->show();
	
	m_toolViews[m_buttonBars[toToolBarArea( area ) ]] << toolView;
	
	
	// Show separators
	if ( area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea )
	{
		m_buttonBars[toToolBarArea( area ) ]->showSeparator(! m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
	}
	else if ( area == Qt::LeftDockWidgetArea )
	{
		m_buttonBars[Qt::TopToolBarArea]->showSeparator( m_buttonBars[Qt::TopToolBarArea]->isEmpty() );
	}
	////
	
	connect(toolView, SIGNAL(topLevelChanged(bool)), this, SLOT(relayoutViewButton(bool)));
	
	if ( toolView->isVisible() )
	{
		toolView->button()->click(); // Hide!
	}
	
	return toolView;
}

/**
 * Moves a tool view to newPlace
 * @param view 
 * @param newPlace 
 */
void DMainWindow::moveToolView(DToolView *view, Qt::DockWidgetArea newPlace)
{
	if ( toDockWidgetArea(view->button()->area()) == newPlace || newPlace == Qt::AllDockWidgetAreas || newPlace == 0 )
	{
		return;
	}
	
	addDockWidget(newPlace, view);
	
	m_forRelayout = view;
	
	relayoutToolView();
}

/**
 * Add a widget to perspective
 * @param widget 
 * @param perspective 
 */
void DMainWindow::addToPerspective(QWidget *widget, int perspective)
{
	if ( QToolBar *bar = dynamic_cast<QToolBar*>(widget) )
	{
		if ( toolBarArea(bar) == 0 )
		{
			addToolBar( bar );
		}
	}
	
	if ( ! m_managedWidgets.contains( widget ) )
	{
		m_managedWidgets.insert( widget, perspective );
		
		if ( perspective != m_currentPerspective )
		{
			widget->hide();
		}
	}
}

/**
 * Remove widget from perspective
 * @param widget 
 */
void DMainWindow::removeFromPerspective(QWidget *widget)
{
	m_managedWidgets.remove(widget);
}

/**
 * Adds a QAction list to perspective
 * @param actions 
 * @param perspective 
 */
void DMainWindow::addToPerspective(const QList<QAction *> &actions, int perspective)
{
	foreach(QAction *a, actions)
	{
		addToPerspective(a, perspective);
	}
}

/**
 * Adds an action to perspective
 * @param action 
 * @param perspective 
 */
void DMainWindow::addToPerspective(QAction *action, int perspective)
{
	if ( ! m_managedActions.contains( action ) )
	{
		m_managedActions.insert( action, perspective );
		
		if ( perspective != m_currentPerspective )
		{
			action->setVisible(false);
		}
	}
}

/**
 * Remove an action from perspective
 * @param action 
 */
void DMainWindow::removeFromPerspective(QAction *action)
{
	m_managedActions.remove(action);
}

Qt::DockWidgetArea DMainWindow::toDockWidgetArea(Qt::ToolBarArea area)
{
	switch(area)
	{
		case Qt::LeftToolBarArea:
		{
			return Qt::LeftDockWidgetArea;
		}
		break;
		case Qt::RightToolBarArea:
		{
			return Qt::RightDockWidgetArea;
		}
		break;
		case Qt::TopToolBarArea:
		{
			return Qt::TopDockWidgetArea;
		}
		break;
		case Qt::BottomToolBarArea:
		{
			return Qt::BottomDockWidgetArea;
		}
		break;
		default: qWarning("toDockWidgetArea: Floating... %d", area); break;
	}
	
	return Qt::LeftDockWidgetArea;
}

Qt::ToolBarArea DMainWindow::toToolBarArea(Qt::DockWidgetArea area)
{
	switch(area)
	{
		case Qt::LeftDockWidgetArea:
		{
			return Qt::LeftToolBarArea;
		}
		break;
		case Qt::RightDockWidgetArea:
		{
			return Qt::RightToolBarArea;
		}
		break;
		case Qt::TopDockWidgetArea:
		{
			return Qt::TopToolBarArea;
		}
		break;
		case Qt::BottomDockWidgetArea:
		{
			return Qt::BottomToolBarArea;
		}
		break;
		default: qWarning("toToolBarArea: Floating... %d", area); break;
	}
	
	return Qt::LeftToolBarArea;
}


/**
 * Enable/disable button blending.
 * @param enable 
 */
void DMainWindow::setEnableButtonBlending(bool enable)
{
	foreach( DButtonBar *bar, m_buttonBars.values() )
	{
		bar->setEnableButtonBlending(enable);
	}
}

void DMainWindow::relayoutViewButton(bool topLevel)
{
	if ( !topLevel )
	{
		if ( DToolView *toolView = dynamic_cast<DToolView *>(sender()) )
		{
			m_forRelayout = toolView;
			
			QTimer::singleShot( 0, this, SLOT(relayoutToolView()));
// 			relayoutToolView();
			
			// if a tool view is floating the button bar isn't exclusive
			DButtonBar *bar = m_buttonBars[m_forRelayout->button()->area()];
			
			qDebug() << (m_forRelayout->button()->area());
			bool exclusive = true;
			
			foreach(DToolView *v, m_toolViews[bar] )
			{
				exclusive = exclusive && !v->isFloating();
			}
			
			bar->setExclusive( exclusive );
			
			bar->onlyShow( m_forRelayout, true );
		}
	}
	else
	{
		// Floating tool views aren't exclusive
		if ( DToolView *toolView = dynamic_cast<DToolView *>(sender()) )
		{
			m_buttonBars[toolView->button()->area()]->setExclusive(false);
		}
	}
	
}

void DMainWindow::relayoutToolView()
{
	if ( !m_forRelayout ) return;
	
	bool isVisible = m_forRelayout->isVisible();
	
	if ( !isVisible ) m_forRelayout->show();
	
	DViewButton *button = m_forRelayout->button();
	
	Qt::ToolBarArea area = toToolBarArea( QMainWindow::dockWidgetArea(m_forRelayout) );
	
	if ( !isVisible ) m_forRelayout->close();
	
// 	qDebug() << "Relayout: " << m_forRelayout->windowTitle() << " from " << button->area() << " to " << area;
	
	if ( area != button->area() && !m_forRelayout->isFloating() )
	{
		setUpdatesEnabled( false );
		
		m_buttonBars[button->area()]->removeButton(button);
		// Show separators
		if ( button->area() == Qt::LeftToolBarArea )
		{
			m_buttonBars[Qt::BottomToolBarArea]->showSeparator(! m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
			
			m_buttonBars[Qt::TopToolBarArea]->showSeparator(! m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
		}
		else if ( area == Qt::LeftToolBarArea )
		{
			m_buttonBars[Qt::BottomToolBarArea]->showSeparator(m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
			
			m_buttonBars[Qt::TopToolBarArea]->showSeparator(m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
		}
		//////////
		
		m_toolViews[m_buttonBars[button->area()]].removeAll(m_forRelayout);
		m_toolViews[m_buttonBars[area]] << m_forRelayout;
		button->setArea(area);
		m_buttonBars[area]->addButton(button);
		
		m_buttonBars[area]->repaint();
		setUpdatesEnabled( true );
	}
	
// 	qDebug() << m_forRelayout->windowTitle() << m_forRelayout->button()->isChecked();
	
	m_forRelayout = 0;
}

/**
 * Sets the current perspective.
 * @param wsp 
 */
void DMainWindow::setCurrentPerspective(int wsp)
{
	if ( m_currentPerspective == wsp ) return;
	
	typedef QList<DToolView *> Views;
	
	QList<Views > viewsList = m_toolViews.values();
	
	setUpdatesEnabled( false );
	centralWidget()->setUpdatesEnabled(false);
	
	QHash<DButtonBar *, int> hideButtonCount;
	
	foreach(Views views, viewsList)
	{
		foreach(DToolView *v, views )
		{
			DButtonBar *bar = m_buttonBars[ v->button()->area() ];
			
			bar->setUpdatesEnabled(false);
			v->setUpdatesEnabled(false);
			
			if ( v->perspective() & wsp )
			{
				bar->enable( v->button() );
				
				if ( v->button()->isChecked() )
				{
					v->show();
				}
			}
			else
			{
				bar->disable( v->button() );
				
				if ( v->button()->isChecked() || v->isVisible() )
				{
					v->close();
				}
				
				hideButtonCount[bar]++;
			}
			
			if ( bar->isEmpty() )
			{
				bar->hide();
			}
			else if ( ! bar->isVisible() )
			{
				bar->show();
			}
			
			v->setUpdatesEnabled(true);
			bar->setUpdatesEnabled(true);
		}
	}
	
	QHashIterator<DButtonBar *, int> barIt(hideButtonCount);
	
	while ( barIt.hasNext() )
	{
		barIt.next();
		
		if ( barIt.key()->count() == barIt.value() )
		{
			barIt.key()->hide();
		}
	}
	
	QHashIterator<QWidget *, int> widgetIt(m_managedWidgets);
	
	while ( widgetIt.hasNext() )
	{
		widgetIt.next();
		
		if ( widgetIt.value() & wsp )
		{
			widgetIt.key()->show();
		}
		else
		{
			widgetIt.key()->hide();
		}
	}
	
	QHashIterator<QAction *, int> actionIt(m_managedActions);
	
	while ( actionIt.hasNext() )
	{
		actionIt.next();
		
		if ( actionIt.value() & wsp )
		{
			actionIt.key()->setVisible(true);
		}
		else
		{
			actionIt.key()->setVisible(false);
		}
	}
	
	centralWidget()->setUpdatesEnabled(true);
	setUpdatesEnabled( true );
	
	
	m_currentPerspective = wsp;
	
	emit perspectiveChanged( m_currentPerspective );
}

/**
 * Returns the current perspective
 * @return 
 */
int DMainWindow::currentPerspective() const
{
	return m_currentPerspective;
}

/**
 * if autoRestore is true, the widgets will be loaded when main window is showed (position and properties).
 * @param autoRestore 
 */
void DMainWindow::setAutoRestore(bool autoRestore)
{
	m_autoRestore = autoRestore;
}

/**
 * Returns the autoRestore property.
 * @return 
 */
bool DMainWindow::autoRestore() const
{
	return m_autoRestore;
}

QMenu *DMainWindow::createPopupMenu()
{
	QMenu *menu = QMainWindow::createPopupMenu();
	menu->addSeparator();
	
	return menu;
}

void DMainWindow::setSettingsHandler(DMainWindowAbstractSettings *settings)
{
	delete m_settings;
	
	m_settings = settings;
	m_settings->setParent(this);
}

void DMainWindow::closeEvent(QCloseEvent *e)
{
	saveGUI();
	
	QMainWindow::closeEvent(e);
}

void DMainWindow::showEvent(QShowEvent *e)
{
	QMainWindow::showEvent(e);
	
	if ( !m_autoRestore )
	{
		m_autoRestore = true;
		restoreGUI();
		
		int cwsp = m_currentPerspective;
		
		m_currentPerspective -= 1;
		setCurrentPerspective( cwsp );
	}
}

#if QT_VERSION >= 0x040200
bool DMainWindow::event(QEvent *e)
{
	if ( e->type() == QEvent::HoverMove )
	{
		// Show bar if autohide is enabled
		QPoint pos = mapFromGlobal(QCursor::pos());
		
		DButtonBar *bar = 0;
		
		if ( pos.x() <= m_buttonBars[Qt::LeftToolBarArea]->pos().x() + 3  ) // Left
		{
			bar = m_buttonBars[Qt::LeftToolBarArea];
			
		}
		else if ( pos.y() <= m_buttonBars[Qt::TopToolBarArea]->pos().y() + 3 && m_buttonBars[Qt::TopToolBarArea]->pos().y() <= pos.y())
		{
			bar = m_buttonBars[Qt::TopToolBarArea];
		}
		else if ( pos.x() >= m_buttonBars[Qt::RightToolBarArea]->pos().x() + m_buttonBars[Qt::RightToolBarArea]->width() - 3 )
		{
			bar = m_buttonBars[Qt::RightToolBarArea];
		}
		else if ( pos.y() >= m_buttonBars[Qt::BottomToolBarArea]->pos().y() +  m_buttonBars[Qt::BottomToolBarArea]->height() - 3 && m_buttonBars[Qt::BottomToolBarArea]->pos().y()+m_buttonBars[Qt::BottomToolBarArea]->height() > pos.y()  )
		{
			bar = m_buttonBars[Qt::BottomToolBarArea];
		}
		
		if ( bar )
		{
			if ( bar->autohide() && !bar->isVisible() && !bar->isEmpty() )
			{
				bar->show();
			}
		}
	}
	
	return QMainWindow::event(e);
}

#endif

void DMainWindow::saveGUI()
{
	m_settings->save( this );
}

void DMainWindow::restoreGUI()
{
	setUpdatesEnabled(false);
	m_settings->restore(this);
	setUpdatesEnabled(true);
}

/**
 * Returns the button bars
 * @return 
 */
QHash<Qt::ToolBarArea, DButtonBar *> DMainWindow::buttonBars() const
{
	return m_buttonBars;
}

/**
 * Returs the tool views.
 * @return 
 */
QHash<DButtonBar *, QList<DToolView*> > DMainWindow::toolViews() const
{
	return m_toolViews;
}

