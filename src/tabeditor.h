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
 
#ifndef _TABEDITOR_H_
#define _TABEDITOR_H_

#include <QTabWidget>

class Editor;

class TabEditor : public QTabWidget {
	Q_OBJECT
public:
  TabEditor(QWidget * parent);

  Editor * newTab(const QString & title = "untilted.xsl");
  void updateTitle(int);
  Editor * currentEditor() const;
  Editor * editor(int index) const;
  Editor * editor(QString filename) const;


  static QString strippedName(const QString &fullFileName);
public slots:
  void loadTab(const QString & filename);

  void copy();
  void cut();
  void paste();
  
  void undo();
  void redo(); 
  
  void selectAll();
  
  void duplicateCurrentLine();

  void moveLineUp();
  void moveLineDown();

  void upperSelectedText();
  void lowerSelectedText();

  void commentSelectedText();
  void uncommentSelectedText();

  void complete();
signals:
  void copyAvailable ( bool available );
  void undoAvailable ( bool available );
  void redoAvailable ( bool available );
  void editAvailable ( bool available );
protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  void tabRemoved ( int index );
private slots:
  void slotDocumentWasModified();
  void slotCurrentTabChanged(int);
private:
  Editor * prec;
};

#endif
