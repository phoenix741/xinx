/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
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

#ifndef EDITOR_H
#define EDITOR_H
//
#include <QFrame>
//

class QAbstractItemModel;
class XSLProject;

class Editor : public QFrame {
	Q_OBJECT
public:
	Editor( QWidget * parent = 0, XSLProject * project = NULL );
	virtual ~Editor();

	virtual QString getTitle() const = 0;
	virtual bool hasName() const = 0;
	
	virtual bool canCopy() = 0;
	virtual bool canPaste() = 0;
	virtual bool canUndo() = 0;
	virtual bool canRedo() = 0;
	virtual bool isModified() = 0;
	
	virtual QAbstractItemModel * model() = 0;

public Q_SLOTS : 
	virtual void undo() = 0;
	virtual void redo() = 0;

	virtual void cut() = 0;
	virtual void copy() = 0;
	virtual void paste() = 0;
	
	virtual void setModified( bool ) = 0;
	
Q_SIGNALS:
	void undoAvailable( bool available );
	void redoAvailable( bool available );
	void copyAvailable( bool available );
	void pasteAvailable( bool available );
	
	void modificationChanged( bool changed );

	void deleteModel();
	void createModel();

protected:
	XSLProject * m_project;
};
#endif
