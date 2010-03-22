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

#ifndef ASSISTANTMANAGER_H
#define ASSISTANTMANAGER_H

#include <abstractmanager.h>
#include <QUrl>

class DTabbedMainWindow;
class QTreeWidgetItem;
class QTreeWidget;

/**
	@author David Cuadrado <krawek@gmail.com>
*/
class AssistantManager : public AbstractManager
{
	Q_OBJECT;
	public:
		AssistantManager(QObject *parent = 0);
		virtual ~AssistantManager();
		virtual void openFile(const QString &file);
		virtual void newFile();
		
	private slots:
		void back();
		void next();
		void home();
		
		void updateSource(const QUrl &url);
		void goToBookmark(QTreeWidgetItem *item, int);
		void addBookmark();
		void removeBookmark();
		
	protected:
		virtual void setupWindow(DTabbedMainWindow *w );
		
	private:
		DTabbedMainWindow *m_mainWindow;
		QString m_currentFile;
		
		QTreeWidget *m_bookmarks;
};

#endif
