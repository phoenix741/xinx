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

#ifndef __JSEDITOR_H__
#define __JSEDITOR_H__

#include "texteditor.h"
#include <QTextCursor>
#include <QModelIndex>

class PrivateJSEditor;

/*!
 * Editor based on a TextEditor (from a QTextEdit) who have JavaScript syntaxique 
 * coloration.
 */
class JSEditor : public TextEditor {
	Q_OBJECT
public:
	/*!
	 * Create the Javascript editor.
	 * \param parent Parent of the QTextEdit.
	 */
	JSEditor( QWidget * parent = 0 );
	/*!
	 * Destroy the editor.
	 */
	virtual ~JSEditor();
	
	virtual QAbstractItemModel * model();
	virtual void complete();
public slots:
	virtual void updateModel();
protected:
	void keyPressEvent( QKeyEvent * e );
private:
	PrivateJSEditor * d;
	friend class PrivateJSEditor;
};

#endif // __JSEDITOR_H__
