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
#include <QTextEdit>
#include <QToolBox>
#include <QTextBrowser>
#include <QLabel>

#include "dmainwindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	DMainWindow mw;
	
// 	for(int i = 0; i < 100; i++ )
	{
	QTextEdit *textEdit = new QTextEdit("A tool view");
	textEdit->setWindowTitle("Editor");
	
	mw.addToolView( textEdit, Qt::LeftDockWidgetArea );
	}
	
	QToolBox *toolBox = new QToolBox;
	toolBox->setWindowTitle("Tool box");
	
	toolBox->addItem( new QLabel("1"), "Label1" );
	toolBox->addItem( new QLabel("2"), "Label2" );
	toolBox->addItem( new QLabel("3"), "Label3" );
	toolBox->addItem( new QLabel("4"), "Label4" );
	
	mw.addToolView( toolBox, Qt::RightDockWidgetArea );
	
	
	QTextBrowser *textBrowser = new QTextBrowser;
	
	textBrowser->setHtml("<center><h1>Demo 1!</h1></center>");
	
	mw.setCentralWidget( textBrowser );
	
	
	mw.show();
	
	return app.exec();
}



