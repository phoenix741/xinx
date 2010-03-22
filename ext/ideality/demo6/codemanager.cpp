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

#include "codemanager.h"

#include "dtabbedmainwindow.h"

#include <QTextEdit>
#include <QFile>
#include <QFileInfo>
#include <QListWidget>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>

#include "defs.h"

CodeManager::CodeManager(QObject *parent)
 : AbstractManager(parent)
{
}


CodeManager::~CodeManager()
{
}


QString CodeManager::fileFilter() const
{
	return tr("Source files (*.cpp *.h)");
}

void CodeManager::newFile()
{
	QTextEdit *editor = new QTextEdit;
	
	editor->setWindowTitle(tr("New...") );
	
	m_mainWindow->addWidget( editor, false, Code );
	
}

void CodeManager::openFile(const QString& file)
{
	QTextEdit *editor = new QTextEdit;
	
	QFile source(file);
	
	if (!source.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	editor->setPlainText(source.readAll());
	
	QFileInfo fi(source);
	editor->setWindowTitle(fi.fileName());
	
	m_mainWindow->addWidget( editor, false, Code );
	
	m_files->addItem(file);
	m_editors.insert(file, editor);
	
	
}

void CodeManager::setupWindow(DTabbedMainWindow* w)
{
	m_files = new QListWidget;
	m_files->setWindowTitle(tr("Files"));
	
	w->addToolView( m_files, Qt::LeftDockWidgetArea, Code);
	
	
	QToolBar *toolBar = new QToolBar(tr("Code tool bar"), w);
	toolBar->addAction(tr("Undo"), this, SLOT(undo()))->setIcon( QIcon(":/images/prev.png") );
	toolBar->addAction(tr("Redo"), this, SLOT(redo()))->setIcon(QIcon(":/images/next.png"));
	
	w->addToPerspective(toolBar, Code);
	
	QMenu *menu = new QMenu(tr("Code"), w);
	menu->addActions(toolBar->actions());
	
	QAction *a = w->menuBar()->addMenu(menu);
	w->addToPerspective(a, Code);
	
	connect(m_files, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(showEditorFromItem(QListWidgetItem *)));
	m_mainWindow = w;
}

void CodeManager::showEditorFromItem(QListWidgetItem *item)
{
	if (!item) return;
	
	m_mainWindow->tabWidget()->setCurrentIndex( m_mainWindow->tabWidget()->indexOf( m_editors[item->text()] ) );
}


void CodeManager::undo()
{
	if ( QTextEdit *editor = dynamic_cast<QTextEdit*>(m_mainWindow->tabWidget()->currentWidget()) )
	{
		editor->document()->undo();
	}
}

void CodeManager::redo()
{
	if ( QTextEdit *editor = dynamic_cast<QTextEdit*>(m_mainWindow->tabWidget()->currentWidget()) )
	{
		editor->document()->redo();
	}
}
