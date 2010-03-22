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

#include "assistantmanager.h"

#include "dtabbedmainwindow.h"

#include <QTextBrowser>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QTreeWidget>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <QToolBar>

#include <QtDebug>

#include "defs.h"


AssistantManager::AssistantManager(QObject *parent)
 : AbstractManager(parent)
{
}


AssistantManager::~AssistantManager()
{
}

void AssistantManager::setupWindow(DTabbedMainWindow *w )
{
	QWidget *cont = new QWidget;
	cont->setWindowIcon(QIcon(":/images/book.png"));
	cont->setWindowTitle(tr("Bookmarks") );
	
	QVBoxLayout *layout = new QVBoxLayout(cont);
	
	m_bookmarks = new QTreeWidget;
	m_bookmarks->setHeaderLabels(QStringList() << tr("Bookmark") );
	
	layout->addWidget(m_bookmarks);
	
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	
	QPushButton *addBookmark = new QPushButton(tr("Add"));
	connect(addBookmark, SIGNAL(clicked()), this, SLOT(addBookmark()));
	buttonLayout->addWidget(addBookmark);
	QPushButton *removeBookmark = new QPushButton(tr("Remove"));
	connect(removeBookmark, SIGNAL(clicked()), this, SLOT(removeBookmark()));
	buttonLayout->addWidget(removeBookmark);
	
	layout->addLayout(buttonLayout);
	
	w->addToolView( cont, Qt::LeftDockWidgetArea, Assistant);
	
	connect(m_bookmarks, SIGNAL(itemActivated(QTreeWidgetItem *,int)), this, SLOT(goToBookmark(QTreeWidgetItem *,int)));
	
	QToolBar *toolBar = new QToolBar(tr("Assistant tool bar"), w);
	toolBar->addAction(tr("Back"), this, SLOT(back()) )->setIcon(QIcon(":/images/prev.png"));
	toolBar->addAction(tr("Home"), this, SLOT(home()))->setIcon(QIcon(":/images/home.png"));
	toolBar->addAction(tr("Next"), this, SLOT(next()) )->setIcon(QIcon(":/images/next.png"));
	
	
	w->addToPerspective(toolBar, Assistant);
	
	QMenu *menu = new QMenu(tr("Assistant"), w);
	menu->addActions(toolBar->actions());
	
	QAction *a = w->menuBar()->addMenu(menu);
	w->addToPerspective(a, Assistant);
	
	m_mainWindow = w;
}

void AssistantManager::openFile(const QString &file)
{
	QTextBrowser *browser = new QTextBrowser;
	
	QFileInfo fi(file);
	
	browser->setWindowTitle(fi.fileName());
	
	browser->setSearchPaths( browser->searchPaths() << fi.absolutePath() );
	browser->setSource(QUrl(file));
	
	connect(browser, SIGNAL(sourceChanged(const QUrl &)), this, SLOT(updateSource(const QUrl &)));
	
	m_mainWindow->addWidget(browser, false, Assistant );
	
	m_currentFile = file;
}

void AssistantManager::newFile()
{
	openFile( QLibraryInfo::location(QLibraryInfo::DocumentationPath)+"/html/index.html" );
}


void AssistantManager::back()
{
	if ( QTextBrowser *browser = dynamic_cast<QTextBrowser*>(m_mainWindow->tabWidget()->currentWidget()) )
	{
		browser->backward();
	}
}

void AssistantManager::next()
{
	if ( QTextBrowser *browser = dynamic_cast<QTextBrowser*>(m_mainWindow->tabWidget()->currentWidget()) )
	{
		browser->forward();
	}
}

void AssistantManager::home()
{
	if ( QTextBrowser *browser = dynamic_cast<QTextBrowser*>(m_mainWindow->tabWidget()->currentWidget()) )
	{
		browser->home();
	}
}

void AssistantManager::updateSource(const QUrl &url)
{
	m_currentFile = url.path();
}

void AssistantManager::goToBookmark(QTreeWidgetItem *item, int)
{
	if ( QTextBrowser *browser = dynamic_cast<QTextBrowser*>(m_mainWindow->tabWidget()->currentWidget()) )
	{
		browser->setSource(QUrl(item->text(0)));
	}
}

void AssistantManager::addBookmark()
{
	QTreeWidgetItem *bookmark = new QTreeWidgetItem(m_bookmarks);
	bookmark->setText(0, m_currentFile);
}

void AssistantManager::removeBookmark()
{
	delete m_bookmarks->currentItem();
}

