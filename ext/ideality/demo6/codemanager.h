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
#ifndef CODEMANAGER_H
#define CODEMANAGER_H

#include <abstractmanager.h>
#include <QMap>

class QListWidget;
class QTextEdit;
class QListWidgetItem;

/**
 * @author David Cuadrado <krawek@gmail.com>
*/
class CodeManager : public AbstractManager
{
	Q_OBJECT;
	
	public:
		CodeManager(QObject *parent = 0);
		~CodeManager();

		virtual QString fileFilter() const;
		virtual void newFile();
		virtual void openFile(const QString& file);

	protected:
		virtual void setupWindow(DTabbedMainWindow* w);
		
	private slots:
		void showEditorFromItem(QListWidgetItem *item);
		void undo();
		void redo();
		
	private:
		DTabbedMainWindow *m_mainWindow;
		QListWidget *m_files;
		
		QMap<QString, QTextEdit *> m_editors;

};

#endif
