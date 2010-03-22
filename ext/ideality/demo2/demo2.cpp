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

#include <QApplication>
#include <QApplication>
#include <QTextEdit>
#include <QToolBox>
#include <QTextBrowser>
#include <QLabel>
#include <QTableWidget>
#include <QStatusBar>
#include <QtDebug>

#include "dtabbedmainwindow.h"
#include "dtoolview.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	DTabbedMainWindow mw;
	mw.statusBar()->show();
	
	QTextEdit *textEdit = new QTextEdit("A tool view");
	textEdit->setWindowTitle("Editor");
	
	mw.addToolView( textEdit, Qt::LeftDockWidgetArea )->setDescription("Editor description");;
	
	
	QToolBox *toolBox = new QToolBox;
	toolBox->setWindowTitle("Tool box");
	
	toolBox->addItem( new QLabel("1"), "Label1" );
	toolBox->addItem( new QLabel("2"), "Label2" );
	toolBox->addItem( new QLabel("3"), "Label3" );
	toolBox->addItem( new QLabel("4"), "Label4" );
	
	mw.addToolView( toolBox, Qt::RightDockWidgetArea )->setDescription("Tool box description");
	
	
	QTextEdit *textBrowser = new QTextEdit;
	textBrowser->setWindowTitle("Editor");
	textBrowser->setHtml("<center><h1>Demo 2!</h1></center><br>");
	mw.addWidget( textBrowser );
	
	QLabel *persistantLabel = new QLabel("Unclosable tab!");
	persistantLabel->setWindowTitle("Unclosable");
	
	mw.addWidget( persistantLabel, true );
	
	QTableWidget *table = new QTableWidget(1000,1000);
	table->setWindowTitle("Table");
	
	mw.addWidget( table );
	
	mw.show();
	
	return app.exec();
}



