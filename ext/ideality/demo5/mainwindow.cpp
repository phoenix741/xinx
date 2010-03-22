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

MainWindow::MainWindow(QWidget *parent) : DTabbedMainWindow(parent)
{
	QTextEdit *textEdit = new QTextEdit("A tool view");
	textEdit->setWindowTitle("Editor");
	
	addToolView( textEdit, Qt::LeftDockWidgetArea, All );
	
	QToolBox *toolBox = new QToolBox;
	toolBox->setWindowTitle("Tool box");
	
	toolBox->addItem( new QLabel("1"), "Label1" );
	toolBox->addItem( new QLabel("2"), "Label2" );
	toolBox->addItem( new QLabel("3"), "Label3" );
	toolBox->addItem( new QLabel("4"), "Label4" );
	
	addToolView( toolBox, Qt::RightDockWidgetArea, All );
	
	
	QTextEdit *edit1 = new QTextEdit;
	edit1->setWindowTitle("Write1");
	QTextEdit *edit2 = new QTextEdit;
	edit2->setWindowTitle("Write2");
	QTextEdit *edit3 = new QTextEdit;
	edit3->setWindowTitle("Write3");
	QTextEdit *edit4 = new QTextEdit;
	edit4->setWindowTitle("Write4");
	
	addToolView( edit1, Qt::RightDockWidgetArea, Write );
	addToolView( edit2, Qt::RightDockWidgetArea, Write );
	addToolView( edit3, Qt::RightDockWidgetArea, Write );
	addToolView( edit4, Qt::RightDockWidgetArea, Write );
	
	QTextEdit *browser1 = new QTextEdit;
	browser1->setWindowTitle("Read1");
	QTextEdit *browser2 = new QTextEdit;
	browser2->setWindowTitle("Read2");
	QTextEdit *browser3 = new QTextEdit;
	browser3->setWindowTitle("Read3");
	QTextEdit *browser4 = new QTextEdit;
	browser4->setWindowTitle("Read4");
	
	addToolView( browser1, Qt::RightDockWidgetArea, Read );
	addToolView( browser2, Qt::RightDockWidgetArea, Read );
	addToolView( browser3, Qt::BottomDockWidgetArea, Read );
	addToolView( browser4, Qt::BottomDockWidgetArea, Read );
	
	
	QTextBrowser *textBrowser = new QTextBrowser;
	textBrowser->setHtml("<center><h1>Demo 5: perspectives and tabs!</h1></center><br>Read");
	textBrowser->setWindowTitle("Read tab 1");
	addWidget( textBrowser, false, Read );
	
	QTextBrowser *textBrowser1 = new QTextBrowser;
	textBrowser1->setHtml("<center><h1>Demo 5: perspectives and tabs!</h1></center><br>Read");
	textBrowser1->setWindowTitle("Read tab 2");
	addWidget( textBrowser1, false, Read );
	
	QTextBrowser *textBrowser2 = new QTextBrowser;
	textBrowser2->setHtml("<center><h1>Demo 5: perspectives and tabs!</h1></center><br>Read");
	textBrowser2->setWindowTitle("Read tab 3");
	addWidget( textBrowser2, false, Read );
	
	QTextEdit *textEditor = new QTextEdit;
	textEditor->setWindowTitle("Write tab 1");
	textEditor->setHtml( "<center><h1>Demo 5: perspectives!</h1></center>" );
	addWidget( textEditor, false, Write );
	
	QTextEdit *textEditor2 = new QTextEdit;
	textEditor2->setWindowTitle("Write tab 2");
	textEditor2->setHtml( "<center><h1>Demo 5: perspectives!</h1></center>" );
	addWidget( textEditor2, false, Write );
	
	QTextEdit *textEditor3 = new QTextEdit;
	textEditor3->setWindowTitle("Write tab 3");
	textEditor3->setHtml( "<center><h1>Demo 5: perspectives!</h1></center>" );
	addWidget( textEditor3, false, Write );
	
	createMenu();
	
	setCurrentPerspective( Read );
}


MainWindow::~MainWindow()
{
}

void MainWindow::createMenu()
{
	QMenu *menu = menuBar()->addMenu("Perspective");

	QActionGroup *group = new QActionGroup(this);
	group->setExclusive(true);

	QAction *readWps = new QAction("Read", this);
	readWps->setData(Read);
	group->addAction(readWps);

	QAction *writeWps = new QAction("Write", this);
	group->addAction(writeWps);
	writeWps->setData(Write);
	
	menu->addActions(group->actions());
	
	connect(group, SIGNAL(triggered(QAction *)), this, SLOT(changePerspective(QAction *)));
	
}

void MainWindow::changePerspective(QAction *a)
{
	setCurrentPerspective(a->data().toInt());
}


