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

#ifndef __TEXTEDITOR_H__
#define __TEXTEDITOR_H__

#include <QTextEdit>

class QAbstractItemModel;
class TextProcessor;
class QSyntaxHighlighter;

class TextEditor : public QTextEdit {
	Q_OBJECT
public:
	TextEditor( QWidget * parent = 0 );
	virtual ~TextEditor();

	virtual void commentSelectedText( bool uncomment = false );
	virtual void complete();

	virtual QAbstractItemModel * model();
	
	int currentColumn();
	int currentRow();

public slots:
	virtual void updateModel();
	
signals:
	void deleteModel();
	void createModel();

protected:
	void keyPressEvent(QKeyEvent *e);
    void mouseDoubleClickEvent( QMouseEvent * event );

private:
	void key_home( bool );
	void key_enter();
};
#endif // __TEXTEDITOR_H__
