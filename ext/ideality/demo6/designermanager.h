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

#ifndef DESIGNERMANAGER_H
#define DESIGNERMANAGER_H

#include <abstractmanager.h>

#include "defs.h"

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;

/**
 * @author David Cuadrado <krawek@gmail.com>
*/
class DesignerManager : public AbstractManager
{
	Q_OBJECT;
	
	public:
		DesignerManager(QObject *parent = 0);
		virtual ~DesignerManager();
		virtual void openFile(const QString &file);
		virtual void newFile();
		virtual QString fileFilter() const;
		
	protected:
		virtual void setupWindow(DTabbedMainWindow *w );
		
	private slots:
		void setActiveForm(int index);
		void updateControls();
		void updateControls(QWidget *w);
		void undo();
		void redo();
		
	private:
		QDesignerFormWindowInterface* createFormWindow();
		
	private:
		QDesignerFormEditorInterface *m_designer; 
		
		DTabbedMainWindow *m_mainWindow;
};

#endif
