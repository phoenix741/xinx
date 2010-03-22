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

#include "mainwindow.h"
#include <QTextEdit>
#include <QToolBox>
#include <QTextBrowser>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include "abstractmanager.h"
#include "designermanager.h"
#include "assistantmanager.h"
#include "codemanager.h"
#include "defs.h"

MainWindow::MainWindow(QWidget *parent) : DTabbedMainWindow(parent), m_currentManager(0)
{
	Q_INIT_RESOURCE(demo6);
	
	createMenu();
	
	m_managers.insert(Designer, new DesignerManager(this));
	m_managers.insert(Assistant, new AssistantManager(this) );
	m_managers.insert(Code, new CodeManager(this)); // FIXME
	
	m_managers[Designer]->initialize(this);
	m_managers[Assistant]->initialize(this);
	m_managers[Code]->initialize(this);
	
	
	connect(this, SIGNAL(perspectiveChanged( int )), this, SLOT(changeManager(int)));
	setCurrentPerspective( Designer );
	
	QMenu *menu = menuBar()->addMenu(tr("Help"));
	menu->addAction(tr("About"), this, SLOT(showAbout()));
	
	
	m_log = new QTextEdit;
	m_log->setWindowTitle(tr("Log"));
	addToolView(m_log, Qt::BottomDockWidgetArea, All );
}


MainWindow::~MainWindow()
{
}

void MainWindow::createMenu()
{
	QMenu *file = menuBar()->addMenu("&File");
	file->addAction(tr("New file"), this, SLOT(newFile()));
	file->addAction(tr("Open file..."), this, SLOT(openFile()));
	
	QMenu *menu = menuBar()->addMenu("Perspective");

	QActionGroup *group = new QActionGroup(this);
	group->setExclusive(true);
	
	QAction *codeWps = new QAction("Code", this);
	codeWps->setData(Code);
	group->addAction(codeWps);
	
	QAction *designerWps = new QAction("Designer", this);
	
	designerWps->setIcon( QIcon(":/images/designer.png") );
	designerWps->setData(Designer);
	group->addAction(designerWps);

	QAction *assistantWps = new QAction("Assistant", this);
	
	assistantWps->setIcon( QIcon(":/images/assistant.png") );
	group->addAction(assistantWps);
	assistantWps->setData(Assistant);
	
	menu->addActions(group->actions());
	
	connect(group, SIGNAL(triggered(QAction *)), this, SLOT(changePerspective(QAction *)));
	
}

void MainWindow::changePerspective(QAction *a)
{
	setCurrentPerspective(a->data().toInt());
	
	switch(a->data().toInt())
	{
		case Designer:
		{
			m_log->append( tr("Switched to designer perspective"));
		}
		break;
		case Code:
		{
			m_log->append( tr("Switched to code perspective"));
		}
		break;
		case Assistant:
		{
			m_log->append( tr("Switched to assistant perspective"));
		}
		break;
		default: break;
	}
}

void MainWindow::openFile()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Open file"), QString(), m_currentManager->fileFilter() );
	
	m_currentManager->openFile( file );
}

void MainWindow::newFile()
{
	m_currentManager->newFile();
}


void MainWindow::changeManager(int w)
{
	if ( m_managers.contains(w) )
	{
		m_currentManager = m_managers[w];
		m_currentManager->initialize( this );
	}
}

void MainWindow::showAbout()
{
	QMessageBox::about(this, tr("About demo6"), tr("<h3>DEMO 6: perspectives and widget/action management.</h3>This demo shows how to use perspectives.\nDemo also shows how to use widget and actions management") );
}


