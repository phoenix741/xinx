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
 
#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <QTextEdit>

class QTabWidget;
class QTextEdit;
class QHBoxLayout;
class QPixmap;
class QRect;
class QPaintEvent;
class QTextCursor;
class StudioTextEdit;

class NumberBar : public QWidget {
     Q_OBJECT
public:
     NumberBar( QWidget *parent );
     ~NumberBar();
 
     void setTextEdit( QTextEdit *edit );
     void paintEvent( QPaintEvent *ev );
private:
     QTextEdit *m_edit;
};

class Editor : public QFrame {
  Q_OBJECT
public:
  Editor( QWidget *parent = 0 );

  const QString & getCurrentFile() const;

  void loadFile(const QString &fileName);
  bool saveFile(const QString &fileName = "");
 
   StudioTextEdit *textEdit() const { return view; }
 
  void duplicateCurrentLine();
  void moveLineUp();
  void moveLineDown();
  void uploSelectedText(bool upper = true);
  void commentSelectedText(bool uncomment = false);

  bool eventFilter( QObject *obj, QEvent *event );
signals:
  void mouseHover( const QString &word );
  void mouseHover( const QPoint &pos, const QString &word );

private:
  void setCurrentFile(const QString & name);
  
  QString m_curFile;

  StudioTextEdit *view;
  NumberBar *numbers;
  QHBoxLayout *box;
  QTextCursor highlight;
};

#endif
