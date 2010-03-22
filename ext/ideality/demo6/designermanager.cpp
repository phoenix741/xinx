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

#include "designermanager.h"
#include "dtabbedmainwindow.h"

#include <QFile>
#include <QFileInfo>
#include <QTabWidget>
#include <QScrollArea>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>

#include <QDesignerWidgetBoxInterface>
#include <QDesignerFormEditorInterface>
#include <QDesignerObjectInspectorInterface>
#include <QDesignerFormWindowManagerInterface>
#include <QDesignerPropertyEditorInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerActionEditorInterface>
#include <QDesignerFormWindowCursorInterface>

#include <QDesignerComponents>

#include <QtDebug>

DesignerManager::DesignerManager(QObject *parent) : AbstractManager(parent)
{
}


DesignerManager::~DesignerManager()
{
}

void DesignerManager::setupWindow(DTabbedMainWindow *w )
{
	QDesignerComponents::initializeResources();
	
	m_designer = QDesignerComponents::createFormEditor( w );
	m_designer->setTopLevel(w);
	
	m_designer->setWidgetBox(QDesignerComponents::createWidgetBox(m_designer, 0));
	
	m_designer->widgetBox()->setFileName(QLatin1String(":/trolltech/widgetbox/widgetbox.xml"));
	m_designer->widgetBox()->load();
	m_designer->widgetBox()->setWindowTitle(QObject::tr("Components"));
	
	m_designer->setPropertyEditor(QDesignerComponents::createPropertyEditor(m_designer, 0));
	m_designer->propertyEditor()->setWindowTitle(QObject::tr("Properties"));
	m_designer->propertyEditor()->setReadOnly(false);
	
	m_designer->setObjectInspector(QDesignerComponents::createObjectInspector(m_designer, 0));
	m_designer->objectInspector()->setWindowTitle(QObject::tr("Object inspector"));
	
	
	m_designer->setActionEditor(QDesignerComponents::createActionEditor(m_designer, 0));
	m_designer->actionEditor()->setWindowTitle(QObject::tr("Action editor"));
	
	
	w->addToolView(m_designer->objectInspector(), Qt::RightDockWidgetArea, Designer);
	w->addToolView(  m_designer->widgetBox(), Qt::LeftDockWidgetArea, Designer);
	w->addToolView( m_designer->propertyEditor(), Qt::RightDockWidgetArea, Designer);
	w->addToolView(m_designer->actionEditor(), Qt::BottomDockWidgetArea, Designer);
	
	connect(w->tabWidget(), SIGNAL(currentChanged(int)), this, SLOT(setActiveForm(int)));
	
	
	QToolBar *toolBar = new QToolBar(tr("Designer tool bar"), w);
	toolBar->addAction(tr("Undo"), this, SLOT(undo()) )->setIcon( QIcon(":/images/prev.png") );
	toolBar->addAction(tr("Redo"), this, SLOT(redo()) )->setIcon(QIcon(":/images/next.png"));
	
	QMenu *menu = new QMenu(tr("Designer"), w);
	menu->addActions(toolBar->actions());
	
	QAction *a = w->menuBar()->addMenu(menu);
	w->addToPerspective(a, Designer);
	
	w->addToPerspective(toolBar, Designer);
	
	m_mainWindow = w;
	
}

QString DesignerManager::fileFilter() const
{
	return QObject::tr("Designer UI files (*.ui)");
}

void DesignerManager::openFile(const QString &file)
{
	if ( !file.toLower().endsWith(".ui") )
	{
		return;
	}
	
	QFile uiFile(file);
	QDesignerFormWindowManagerInterface* manager = m_designer->formWindowManager();
	
	QDesignerFormWindowInterface* widget = createFormWindow();
	widget->setFileName(file);
	widget->setContents(&uiFile);
	
	manager->setActiveFormWindow(widget);
	m_designer->objectInspector()->setFormWindow(widget);
	
	QFileInfo fi(file);
	
	
	widget->setWindowTitle(QObject::tr("Editing: %1").arg( fi.fileName() ));
	
	m_mainWindow->addWidget( widget, false, Designer );
}

void DesignerManager::newFile()
{
	QDesignerFormWindowInterface *widget = createFormWindow();
	
	widget->setMainContainer(new QWidget());
	
	m_designer->formWindowManager()->setActiveFormWindow(widget);
	
	widget->setWindowTitle(tr("New..."));
	m_mainWindow->addWidget( widget, false, Designer );
}

QDesignerFormWindowInterface *DesignerManager::createFormWindow()
{
	QDesignerFormWindowManagerInterface* manager = m_designer->formWindowManager();
	QDesignerFormWindowInterface* widget = manager->createFormWindow();
	
	widget->setFeatures (QDesignerFormWindowInterface::DefaultFeature);
	
// 	connect(widget, SIGNAL(changed()), this, SLOT(updateControls()));
// 	connect(widget, SIGNAL(activated(QWidget *)), this, SLOT(updateControls( QWidget *)));
	connect(widget, SIGNAL(selectionChanged ()), this, SLOT(updateControls()));
	
	return widget;
}

void DesignerManager::setActiveForm(int index)
{
	if ( QDesignerFormWindowInterface * form = dynamic_cast<QDesignerFormWindowInterface *>(m_mainWindow->tabWidget()->widget(index)) )
	{
		m_designer->formWindowManager()->setActiveFormWindow( form );
		m_designer->objectInspector()->setFormWindow( form );
		m_designer->actionEditor()->setFormWindow(form);
	}
}

void DesignerManager::updateControls()
{
	if ( QDesignerFormWindowManagerInterface *manager = m_designer->formWindowManager() )
	{
		if ( QDesignerFormWindowInterface * form = manager->activeFormWindow() )
		{
			
			QDesignerFormWindowCursorInterface *cursor = form->cursor();
			
			if ( cursor->hasSelection() )
			{
				updateControls(  cursor->selectedWidget(0) );
			}
		}
	}
}

void DesignerManager::updateControls(QWidget *w)
{
	m_designer->propertyEditor()->setObject( w );
	m_designer->objectInspector()->setFormWindow( m_designer->formWindowManager()->activeFormWindow() );
	m_designer->actionEditor()->setFormWindow( m_designer->formWindowManager()->activeFormWindow() );
}


void DesignerManager::undo()
{
	QDesignerFormWindowManagerInterface *manager = m_designer->formWindowManager();
	manager->actionUndo()->trigger();
}

void DesignerManager::redo()
{
	QDesignerFormWindowManagerInterface *manager = m_designer->formWindowManager();
	manager->actionRedo()->trigger();
}




